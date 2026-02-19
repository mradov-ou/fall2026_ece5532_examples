#include <rclcpp/rclcpp.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

namespace dead_reckoning_odom {

  class DeadReckoning : public rclcpp::Node {
    public:
      DeadReckoning()
      : rclcpp::Node("dead_reckoning")
      {
        parent_frame_ = declare_parameter<std::string>("parent_frame", "odom");
        child_frame_ = declare_parameter<std::string>("child_frame", "base_footprint");
        x_ = declare_parameter<double>("initial_x", 0.0);
        y_ = declare_parameter<double>("initial_y", 0.0);
        psi_ = declare_parameter<double>("initial_heading_deg", 0.0) * M_PI / 180.0;

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        sub_twist_ = create_subscription<geometry_msgs::msg::TwistStamped>("twist", 1, std::bind(&DeadReckoning::recv_twist, this, std::placeholders::_1));
        pub_odom_ = create_publisher<nav_msgs::msg::Odometry>("odom", 1);
      }

    private:
      // Topics and TF broadcaster
      rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_odom_;
      rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr sub_twist_;
      std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

      // Frame IDs that will be used to broadcast TF
      std::string parent_frame_;
      std::string child_frame_;

      // 2D position and heading values that will be updated over time with the discrete state space model
      double x_;     // X position
      double y_;     // Y position
      double psi_;   // Heading angle

      rclcpp::Time last_twist_time_;

      void recv_twist(const geometry_msgs::msg::TwistStamped::ConstSharedPtr msg) {
        // Compute time difference from the last time a twist message was received
        rclcpp::Time current_time = msg->header.stamp;
        if (last_twist_time_.nanoseconds() == 0) {
          last_twist_time_ = current_time;
          return;
        }
        double ts = (current_time - last_twist_time_).seconds();
        last_twist_time_ = current_time;

        // Integrate discrete vehicle state space model one step with the measured
        //     time difference and the latest speed and yaw rate data
        x_ += ts * msg->twist.linear.x * cos(psi_);
        y_ += ts * msg->twist.linear.x * sin(psi_);
        psi_ += ts * msg->twist.angular.z;

        // Copy dead reckoning position and orientation estimate into transform message
        geometry_msgs::msg::TransformStamped transform_msg;
        transform_msg.header.stamp = current_time;
        transform_msg.header.frame_id = parent_frame_;
        transform_msg.child_frame_id = child_frame_;
        transform_msg.transform.translation.x = x_;
        transform_msg.transform.translation.y = y_;

        tf2::Quaternion q;
        q.setRPY(0, 0, psi_);
        transform_msg.transform.rotation.w = q.w();
        transform_msg.transform.rotation.x = q.x();
        transform_msg.transform.rotation.y = q.y();
        transform_msg.transform.rotation.z = q.z();

        // Broadcast TF frame from world to base_footprint
        tf_broadcaster_->sendTransform(transform_msg);

        // Publish odometry message with the latest dead reckoning estimate
        nav_msgs::msg::Odometry odom_msg;
        odom_msg.header.stamp = current_time;
        odom_msg.header.frame_id = parent_frame_;
        odom_msg.child_frame_id = child_frame_;
        odom_msg.pose.pose.position.x = x_;
        odom_msg.pose.pose.position.y = y_;
        odom_msg.pose.pose.orientation = transform_msg.transform.rotation;
        odom_msg.twist.twist = msg->twist;
        pub_odom_->publish(odom_msg);
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<dead_reckoning_odom::DeadReckoning>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}