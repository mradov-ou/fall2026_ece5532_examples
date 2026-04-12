#include <eigen3/Eigen/Dense>

#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <sensor_msgs/msg/imu.hpp>

namespace imu_ekf_example {

  typedef Eigen::Matrix<double, 7, 1> StateVector;
  typedef Eigen::Matrix<double, 7, 7> StateMatrix;
  enum { QW=0, QX, QY, QZ, RATE_X, RATE_Y, RATE_Z };

  class ImuEkf : public rclcpp::Node {
    public:
      ImuEkf()
      : rclcpp::Node("imu_ekf")
      {
        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        sub_imu_ = create_subscription<sensor_msgs::msg::Imu>("imu", 1, std::bind(&ImuEkf::recv_imu, this, std::placeholders::_1));

        // Initialize Kalman filter state
        X_ << 1.0, 0, 0, 0, 0, 0, 0;
        P_ = StateMatrix::Identity();

        q_quat_ = declare_parameter<double>("q_quat", 1.0);
        q_rate_ = declare_parameter<double>("q_rate", 1.0);
        r_accel_ = declare_parameter<double>("r_accel", 1.0);
        r_gyro_ = declare_parameter<double>("r_gyro", 1.0);
        param_cb_ = add_on_set_parameters_callback(std::bind(&ImuEkf::param_update, this, std::placeholders::_1));

        Q_.setZero();
        Q_(QW, QW) = q_quat_ * q_quat_;
        Q_(QX, QX) = q_quat_ * q_quat_;
        Q_(QY, QY) = q_quat_ * q_quat_;
        Q_(QZ, QZ) = q_quat_ * q_quat_;
        Q_(RATE_X, RATE_X) = q_rate_ * q_rate_;
        Q_(RATE_Y, RATE_Y) = q_rate_ * q_rate_;
        Q_(RATE_Z, RATE_Z) = q_rate_ * q_rate_;

        R_.setZero();
        R_(0, 0) = r_accel_ * r_accel_;
        R_(1, 1) = r_accel_ * r_accel_;
        R_(2, 2) = r_accel_ * r_accel_;
        R_(3, 3) = r_gyro_ * r_gyro_;
        R_(4, 4) = r_gyro_ * r_gyro_;
        R_(5, 5) = r_gyro_ * r_gyro_;
      }

    private:
      std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
      rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_imu_;
      rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_;

      // Estimate state, covariance, and current time stamp
      StateVector X_;
      StateMatrix P_;
      rclcpp::Time estimate_stamp_;

      // Process noise covariance
      double q_quat_;
      double q_rate_;
      StateMatrix Q_;

      // Measurement noise covariance
      double r_accel_;
      double r_gyro_;
      Eigen::Matrix<double, 6, 6> R_;

      void recv_imu(const sensor_msgs::msg::Imu::ConstSharedPtr msg) {
        rclcpp::Time msg_stamp = rclcpp::Time(msg->header.stamp);
        if (estimate_stamp_.get_clock_type() != msg_stamp.get_clock_type()) {
          X_ << 1.0, 0, 0, 0, 0, 0, 0;
          P_ = StateMatrix::Identity();
          estimate_stamp_ = msg_stamp;
          return;
        }

        // Compute amount of time to advance the state prediction
        double dt = (msg_stamp - estimate_stamp_).seconds();
        estimate_stamp_ = msg_stamp;

        StateMatrix A = state_jacobian(dt, X_);
        StateVector predicted_state = state_prediction(dt, X_);
        StateMatrix predicted_cov = cov_prediction(A, Q_, P_);

        // Construct C matrix for an IMU update (3D accelerometer and gyro measurements)
        Eigen::Matrix<double, 6, 7> C;
        C.row(0) << -2 * predicted_state(QY), 2 * predicted_state(QZ), -2 * predicted_state(QW), 2  * predicted_state(QX), 0, 0, 0;
        C.row(1) << 2 * predicted_state(QX), 2 * predicted_state(QW), 2 * predicted_state(QZ), 2 * predicted_state(QY), 0, 0, 0;
        C.row(2) << 2 * predicted_state(QW), -2 * predicted_state(QX), -2 * predicted_state(QY), 2 * predicted_state(QZ), 0, 0, 0;
        C.row(3) << 0, 0, 0, 0, 1, 0, 0;
        C.row(4) << 0, 0, 0, 0, 0, 1, 0;
        C.row(5) << 0, 0, 0, 0, 0, 0, 1;

        // Use C and predicted state to compute expected measurement
        Eigen::Matrix<double, 6, 1> expected_meas;
        expected_meas << 2 * predicted_state(QX) * predicted_state(QZ) - 2 * predicted_state(QW) * predicted_state(QY),
                        2 * predicted_state(QW) * predicted_state(QX) + 2 * predicted_state(QY) * predicted_state(QZ),
                        predicted_state(QW) * predicted_state(QW) - predicted_state(QX) * predicted_state(QX) - predicted_state(QY) * predicted_state(QY) + predicted_state(QZ) * predicted_state(QZ),
                        predicted_state(RATE_X),
                        predicted_state(RATE_Y),
                        predicted_state(RATE_Z);

        // Put measurements in an Eigen object
        Eigen::Matrix<double, 6, 1> real_meas;
        Eigen::Vector3d accel_vect(msg->linear_acceleration.x, msg->linear_acceleration.y, msg->linear_acceleration.z);
        accel_vect.normalize();
        real_meas << accel_vect.x(), accel_vect.y(), accel_vect.z(),
                    msg->angular_velocity.x, msg->angular_velocity.y, msg->angular_velocity.z;

        // Compute Kalman gain
        Eigen::Matrix<double, 6, 6> S;
        S = C * predicted_cov * C.transpose() + R_;
        Eigen::Matrix<double, 7, 6> K;
        K = predicted_cov * C.transpose() * S.inverse();

        // Update filter estimate based on difference between actual and expected measurements
        X_ = predicted_state + K * (real_meas - expected_meas);
        X_.head<4>().normalize();

        // Update estimate error covariance using Kalman gain matrix
        P_ = (StateMatrix::Identity() - K * C) * predicted_cov;

        // Update TF transform with current estimate
        geometry_msgs::msg::TransformStamped ekf_transform;
        ekf_transform.header.stamp = estimate_stamp_;
        ekf_transform.header.frame_id = "map";
        ekf_transform.child_frame_id = "filter";
        ekf_transform.transform.rotation.w = X_(QW);
        ekf_transform.transform.rotation.x = X_(QX);
        ekf_transform.transform.rotation.y = X_(QY);
        ekf_transform.transform.rotation.z = X_(QZ);
        ekf_transform.transform.translation.x = 0;
        ekf_transform.transform.translation.y = 0;
        ekf_transform.transform.translation.z = 0;
        broadcaster_->sendTransform(ekf_transform);
      }

      StateVector state_prediction(double dt, const StateVector& old_state) {
        StateVector new_state;
        new_state(QW) = old_state(QW) + 0.5 * dt * (-old_state(QX) * old_state(RATE_X) - old_state(QY) * old_state(RATE_Y) - old_state(QZ) * old_state(RATE_Z));
        new_state(QX) = old_state(QX) + 0.5 * dt * ( old_state(QW) * old_state(RATE_X) + old_state(QZ) * old_state(RATE_Y) - old_state(QY) * old_state(RATE_Z));
        new_state(QY) = old_state(QY) + 0.5 * dt * ( old_state(QW) * old_state(RATE_Y) - old_state(QZ) * old_state(RATE_X) + old_state(QX) * old_state(RATE_Z));
        new_state(QZ) = old_state(QZ) + 0.5 * dt * ( old_state(QW) * old_state(RATE_Z) + old_state(QY) * old_state(RATE_X) - old_state(QX) * old_state(RATE_Y));
        new_state(RATE_X) = old_state(RATE_X);
        new_state(RATE_Y) = old_state(RATE_Y);
        new_state(RATE_Z) = old_state(RATE_Z);
        return new_state;
      }

      StateMatrix state_jacobian(double dt, const StateVector& state) {
        StateMatrix A;
        A.row(QW) << 0, -state(RATE_X), -state(RATE_Y), -state(RATE_Z), -state(QX), -state(QY), -state(QZ);
        A.row(QX) << state(RATE_X), 0, -state(RATE_Z), state(RATE_Y), state(QW), state(QZ), -state(QY);
        A.row(QY) << state(RATE_Y), state(RATE_Z), 0, -state(RATE_X), -state(QZ), state(QW), state(QX);
        A.row(QZ) << state(RATE_Z), -state(RATE_Y), state(RATE_X), 0, state(QY), -state(QX), state(QW);
        A.row(RATE_X).setZero();
        A.row(RATE_Y).setZero();
        A.row(RATE_Z).setZero();
        return StateMatrix::Identity() + 0.5 * dt * A;
      }

      StateMatrix cov_prediction(const StateMatrix& A, const StateMatrix& Q, const StateMatrix& old_cov) {
        StateMatrix new_cov;
        new_cov = A * old_cov * A.transpose() + Q;
        return new_cov;
      }

      rcl_interfaces::msg::SetParametersResult param_update(const std::vector<rclcpp::Parameter>& parameters) {
        for (const rclcpp::Parameter& p : parameters) {
          if (p.get_name() == "q_quat") {
            q_quat_ = p.as_double();
          } else if (p.get_name() == "q_rate") {
            q_rate_ = p.as_double();
          } else if (p.get_name() == "r_accel") {
            r_accel_ = p.as_double();
          } else if (p.get_name() == "r_gyro") {
            r_gyro_ = p.as_double();
          }
        }

        Q_.setZero();
        Q_(QW, QW) = q_quat_ * q_quat_;
        Q_(QX, QX) = q_quat_ * q_quat_;
        Q_(QY, QY) = q_quat_ * q_quat_;
        Q_(QZ, QZ) = q_quat_ * q_quat_;
        Q_(RATE_X, RATE_X) = q_rate_ * q_rate_;
        Q_(RATE_Y, RATE_Y) = q_rate_ * q_rate_;
        Q_(RATE_Z, RATE_Z) = q_rate_ * q_rate_;

        R_.setZero();
        R_(0, 0) = r_accel_ * r_accel_;
        R_(1, 1) = r_accel_ * r_accel_;
        R_(2, 2) = r_accel_ * r_accel_;
        R_(3, 3) = r_gyro_ * r_gyro_;
        R_(4, 4) = r_gyro_ * r_gyro_;
        R_(5, 5) = r_gyro_ * r_gyro_;

        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
      }
  };

}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<imu_ekf_example::ImuEkf>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}