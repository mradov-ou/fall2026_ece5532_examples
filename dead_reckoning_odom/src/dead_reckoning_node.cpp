#include <rclcpp/rclcpp.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

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

      void recv_twist(const geometry_msgs::msg::TwistStamped::ConstSharedPtr msg) {
        // TODO: Compute time difference from the last time a twist message was received

        // TODO: Integrate discrete vehicle state space model one step with the measured
        //     time difference and the latest speed and yaw rate data

        // TODO: Copy dead reckoning position and orientation estimate into transform message

        // TODO: Broadcast TF frame from world to base_footprint

        // TODO: Publish odometry message with the latest dead reckoning estimate
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