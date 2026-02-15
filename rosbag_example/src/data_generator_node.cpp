#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

namespace rosbag_example {

  class DataGenerator : public rclcpp::Node {
    public:
      DataGenerator()
      : rclcpp::Node("data_generator")
      {
        sine_wave_time_ = 0.0;

        pub_twist_ = create_publisher<geometry_msgs::msg::TwistStamped>("twist", 1);
        timer_ = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(100), std::bind(&DataGenerator::timer_callback, this));
        start_time_ = get_clock()->now();
      }

    private:
      rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr pub_twist_;
      rclcpp::TimerBase::SharedPtr timer_;
      rclcpp::Time start_time_;
      double sine_wave_time_;

      void timer_callback() {

        rclcpp::Time current_time = get_clock()->now();

        sine_wave_time_ = (current_time - start_time_).seconds();

        geometry_msgs::msg::TwistStamped twist_msg;
        twist_msg.header.stamp = current_time;

        // Generate a 20-second sine wave in forward speed field
        twist_msg.twist.linear.x = 5 * (1 - cos(2 * M_PI / 20.0 * sine_wave_time_));

        // Constant yaw rate
        twist_msg.twist.angular.z = 0.1;

        pub_twist_->publish(twist_msg);
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<rosbag_example::DataGenerator>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}