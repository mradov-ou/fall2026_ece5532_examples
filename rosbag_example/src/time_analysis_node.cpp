#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

namespace rosbag_example {

  class TimeAnalysis : public rclcpp::Node {
    public:
      TimeAnalysis()
      : rclcpp::Node("time_analysis")
      {
        sub_twist_ = create_subscription<geometry_msgs::msg::TwistStamped>("twist", 1, std::bind(&TimeAnalysis::recv_twist, this, std::placeholders::_1));
        timer_ = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(1000), std::bind(&TimeAnalysis::timer_cb, this));
      }

    private:
      rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr sub_twist_;

      void recv_twist(const geometry_msgs::msg::TwistStamped::ConstSharedPtr msg) {
        rclcpp::Time msg_time_stamp = rclcpp::Time(msg->header.stamp);

        RCLCPP_INFO_STREAM(get_logger(), "Received message with stamp " << std::fixed << msg_time_stamp.seconds());

        rclcpp::Duration msg_age = get_clock()->now() - msg->header.stamp;
        RCLCPP_INFO_STREAM(get_logger(), "Message stamp is " << std::fixed << msg_age.seconds() << " seconds old");
      }

      rclcpp::TimerBase::SharedPtr timer_;
      void timer_cb() {
        double current_time = get_clock()->now().seconds();
        RCLCPP_INFO_STREAM(get_logger(), "Current time in seconds: " << std::fixed << current_time);
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<rosbag_example::TimeAnalysis>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}