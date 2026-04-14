#include <eigen3/Eigen/Dense>

#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include "enu.hpp"

namespace gnss_ekf_example {

  typedef Eigen::Matrix<double, 5, 1> StateVector;
  typedef Eigen::Matrix<double, 5, 5> StateMatrix;
  enum { POS_X=0, POS_Y, HEADING, SPEED, YAW_RATE };

  class GnssEkf : public rclcpp::Node {
    public:
      GnssEkf()
      : rclcpp::Node("imu_ekf")
      {
        double ref_lat = declare_parameter<double>("ref_lat", INFINITY);
        double ref_lon = declare_parameter<double>("ref_lon", INFINITY);
        ref_alt_ = declare_parameter<double>("ref_alt", INFINITY);
        if (!std::isfinite(ref_lat) || !std::isfinite(ref_lon) || !std::isfinite(ref_alt_)) {
          RCLCPP_ERROR(get_logger(), "Reference coordinate parameters not set!");
          rclcpp::shutdown();
          return;
        }
        ref_ecef_ = llh_to_ecef(ref_lat, ref_lon, ref_alt_);
        enu_rot_mat_ = llh_to_rot_mat(ref_lat, ref_lon);

        sub_fix_ = create_subscription<sensor_msgs::msg::NavSatFix>("gps_fix", 1, std::bind(&GnssEkf::recv_fix, this, std::placeholders::_1));
        sub_twist_ = create_subscription<geometry_msgs::msg::TwistStamped>("twist", 1, std::bind(&GnssEkf::recv_twist, this, std::placeholders::_1));

        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

        q_pos_ = declare_parameter<double>("q_pos", 0.1);
        q_heading_ = declare_parameter<double>("q_heading", 0.1);
        q_speed_ = declare_parameter<double>("q_speed", 0.1);
        q_yaw_rate_ = declare_parameter<double>("q_yaw_rate", 0.1);
        r_gnss_ = declare_parameter<double>("r_gnss", 1.0);
        r_speed_ = declare_parameter<double>("r_speed", 0.01);
        r_yaw_rate_ = declare_parameter<double>("r_yaw_rate", 0.01);
        param_cb_ = add_on_set_parameters_callback(std::bind(&GnssEkf::param_update, this, std::placeholders::_1));

        Q_.setZero();
        Q_(POS_X, POS_X) = q_pos_ * q_pos_;
        Q_(POS_Y, POS_Y) = q_pos_ * q_pos_;
        Q_(HEADING, HEADING) = q_heading_ * q_heading_;
        Q_(SPEED, SPEED) = q_speed_ * q_speed_;
        Q_(YAW_RATE, YAW_RATE) = q_yaw_rate_ * q_yaw_rate_;
      }

    private:
      std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
      rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr sub_fix_;
      rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr sub_twist_;
      rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_;

      // GPS parameters
      double ref_alt_;
      Eigen::Vector3d ref_ecef_;
      Eigen::Matrix3d enu_rot_mat_;

      // Estimate state, covariance, and current time stamp
      StateVector X_;
      StateMatrix P_;
      rclcpp::Time estimate_stamp_;

      // Process noise covariance
      double q_pos_;
      double q_heading_;
      double q_speed_;
      double q_yaw_rate_;
      StateMatrix Q_;

      // Measurement noise covariance
      double r_gnss_;
      double r_speed_;
      double r_yaw_rate_;

      void recv_fix(const sensor_msgs::msg::NavSatFix::ConstSharedPtr msg) {
        double altitude;
        if (!std::isfinite(msg->altitude)) {
          altitude = ref_alt_;
        }
        Eigen::Vector3d current_ecef = llh_to_ecef(msg->latitude, msg->longitude, altitude);
        Eigen::Vector3d enu_position = enu_rot_mat_ * (current_ecef - ref_ecef_);

        update_filter_gnss(msg->header.stamp, enu_position);

        // Update TF transform with raw ENU position
        geometry_msgs::msg::TransformStamped raw_enu_transform;
        raw_enu_transform.header.stamp = msg->header.stamp;
        raw_enu_transform.header.frame_id = "map";
        raw_enu_transform.child_frame_id = "gnss";
        raw_enu_transform.transform.rotation.w = 1.0;
        raw_enu_transform.transform.translation.x = enu_position.x();
        raw_enu_transform.transform.translation.y = enu_position.y();
        raw_enu_transform.transform.translation.z = 0;
        broadcaster_->sendTransform(raw_enu_transform);
      }

      void update_filter_gnss(const rclcpp::Time& msg_stamp, const Eigen::Vector3d& enu_position) {
        if (estimate_stamp_.get_clock_type() != msg_stamp.get_clock_type()) {
          X_ << enu_position.x(), enu_position.y(), 0, 0, 0;
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

        // Construct C matrix for a GNSS update (ENU x and y measurements)
        Eigen::Matrix<double, 2, 5> C;
        C.row(0) << 1, 0, 0, 0, 0;
        C.row(1) << 0, 1, 0, 0, 0;

        // Use C and predicted state to compute expected measurement
        Eigen::Matrix<double, 2, 1> expected_meas;
        expected_meas << predicted_state(POS_X), predicted_state(POS_Y);

        // Put measurements in an Eigen object
        Eigen::Matrix<double, 2, 1> real_meas;
        real_meas << enu_position.x(), enu_position.y();

        // Construct measurement covariance matrix
        Eigen::Matrix<double, 2, 2> R;
        R.row(0) << r_gnss_ * r_gnss_ , 0;
        R.row(1) << 0, r_gnss_ * r_gnss_;

        // Compute Kalman gain
        Eigen::Matrix<double, 2, 2> S;
        S = C * predicted_cov * C.transpose() + R;
        Eigen::Matrix<double, 5, 2> K;
        K = predicted_cov * C.transpose() * S.inverse();

        // Update filter estimate based on difference between actual and expected measurements
        X_ = predicted_state + K * (real_meas - expected_meas);

        // Wrap heading estimate between -pi and +pi
        if (X_(HEADING) > M_PI) {
          X_(HEADING) -= 2 * M_PI;
        } else if (X_(HEADING) < -M_PI) {
          X_(HEADING) += 2 * M_PI;
        }

        // Update estimate error covariance using Kalman gain matrix
        P_ = (StateMatrix::Identity() - K * C) * predicted_cov;
      }

      void recv_twist(const geometry_msgs::msg::TwistStamped::ConstSharedPtr msg) {
        update_filter_twist(msg->header.stamp, msg->twist);

        // Update TF transform with current estimate
        geometry_msgs::msg::TransformStamped ekf_transform;
        ekf_transform.header.stamp = estimate_stamp_;
        ekf_transform.header.frame_id = "map";
        ekf_transform.child_frame_id = "base_footprint";
        ekf_transform.transform.rotation.w = cos(0.5 * X_(HEADING));
        ekf_transform.transform.rotation.x = 0;
        ekf_transform.transform.rotation.y = 0;
        ekf_transform.transform.rotation.z = sin(0.5 * X_(HEADING));
        ekf_transform.transform.translation.x = X_(POS_X);
        ekf_transform.transform.translation.y = X_(POS_Y);
        ekf_transform.transform.translation.z = 0;
        broadcaster_->sendTransform(ekf_transform);
      }

      void update_filter_twist(const rclcpp::Time& msg_stamp, const geometry_msgs::msg::Twist& twist) {
        if (estimate_stamp_.get_clock_type() != msg_stamp.get_clock_type()) {
          return;
        }

        // Compute amount of time to advance the state prediction
        double dt = (msg_stamp - estimate_stamp_).seconds();
        estimate_stamp_ = msg_stamp;

        StateMatrix A = state_jacobian(dt, X_);
        StateVector predicted_state = state_prediction(dt, X_);
        StateMatrix predicted_cov = cov_prediction(A, Q_, P_);

        // Construct C matrix for a twist update (speed and yaw rate measurements)
        Eigen::Matrix<double, 2, 5> C;
        C.row(0) << 0, 0, 0, 1, 0;
        C.row(1) << 0, 0, 0, 0, 1;

        // Use C and predicted state to compute expected measurement
        Eigen::Matrix<double, 2, 1> expected_meas;
        expected_meas << predicted_state(SPEED), predicted_state(YAW_RATE);

        // Put measurements in an Eigen object
        Eigen::Matrix<double, 2, 1> real_meas;
        real_meas << twist.linear.x, twist.angular.z;

        // Construct measurement covariance matrix
        Eigen::Matrix<double, 2, 2> R;
        R.row(0) << r_speed_ * r_speed_ , 0;
        R.row(1) << 0, r_yaw_rate_ * r_yaw_rate_;

        // Compute Kalman gain
        Eigen::Matrix<double, 2, 2> S;
        S = C * predicted_cov * C.transpose() + R;
        Eigen::Matrix<double, 5, 2> K;
        K = predicted_cov * C.transpose() * S.inverse();

        // Update filter estimate based on difference between actual and expected measurements
        X_ = predicted_state + K * (real_meas - expected_meas);

        // Update estimate error covariance using Kalman gain matrix
        P_ = (StateMatrix::Identity() - K * C) * predicted_cov;

        // Wrap heading estimate between -pi and +pi
        if (X_(HEADING) > M_PI) {
          X_(HEADING) -= 2 * M_PI;
        } else if (X_(HEADING) < -M_PI) {
          X_(HEADING) += 2 * M_PI;
        }
      }

      StateVector state_prediction(double dt, const StateVector& old_state) {
        StateVector new_state;
        new_state(POS_X) = old_state(POS_X) + dt * old_state(SPEED) * cos(old_state(HEADING));
        new_state(POS_Y) = old_state(POS_Y) + dt * old_state(SPEED) * sin(old_state(HEADING));
        new_state(HEADING) = old_state(HEADING) + dt * old_state(YAW_RATE);
        new_state(SPEED) = old_state(SPEED);
        new_state(YAW_RATE) = old_state(YAW_RATE);
        return new_state;
      }

      StateMatrix state_jacobian(double dt, const StateVector& state) {
        StateMatrix A;
        A.row(POS_X)    << 1, 0, -dt * state(SPEED) * sin(state(HEADING)), dt * cos(state(HEADING)), 0;
        A.row(POS_Y)    << 0, 1,  dt * state(SPEED) * cos(state(HEADING)), dt * sin(state(HEADING)), 0;
        A.row(HEADING)  << 0, 0, 1, 0, dt;
        A.row(SPEED)    << 0, 0, 0, 1, 0;
        A.row(YAW_RATE) << 0, 0, 0, 0, 1;
        return A;
      }

      StateMatrix cov_prediction(const StateMatrix& A, const StateMatrix& Q, const StateMatrix& old_cov) {
        StateMatrix new_cov;
        new_cov = A * old_cov * A.transpose() + Q;
        return new_cov;
      }

      rcl_interfaces::msg::SetParametersResult param_update(const std::vector<rclcpp::Parameter>& parameters) {
        for (const rclcpp::Parameter& p : parameters) {
          if (p.get_name() == "q_pos") {
            q_pos_ = p.as_double();
          } else if (p.get_name() == "q_heading") {
            q_heading_ = p.as_double();
          } else if (p.get_name() == "q_speed") {
            q_speed_ = p.as_double();
          } else if (p.get_name() == "q_yaw_rate") {
            q_yaw_rate_= p.as_double();
          } else if (p.get_name() == "r_gnss") {
            r_gnss_ = p.as_double();
          } else if (p.get_name() == "r_speed") {
            r_speed_ = p.as_double();
          } else if (p.get_name() == "r_yaw_rate") {
            r_yaw_rate_= p.as_double();
          }
        }

        Q_(POS_X, POS_X) = q_pos_ * q_pos_;
        Q_(POS_Y, POS_Y) = q_pos_ * q_pos_;
        Q_(HEADING, HEADING) = q_heading_ * q_heading_;
        Q_(SPEED, SPEED) = q_speed_ * q_speed_;
        Q_(YAW_RATE, YAW_RATE) = q_yaw_rate_ * q_yaw_rate_;

        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
      }
  };

}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<gnss_ekf_example::GnssEkf>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}