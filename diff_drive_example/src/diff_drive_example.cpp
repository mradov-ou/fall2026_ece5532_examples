#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/float64.hpp>

namespace diff_drive_example {

  class DiffDriveExample : public rclcpp::Node {
    public:
      DiffDriveExample()
      : rclcpp::Node("diff_drive_example")
      {
        sub_twist_ = create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 1,
            std::bind(&DiffDriveExample::recv_twist, this, std::placeholders::_1));

        pub_left_ = create_publisher<std_msgs::msg::Float64>("left_speed", 1);
        pub_right_ = create_publisher<std_msgs::msg::Float64>("right_speed", 1);
      }

    private:
      rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_twist_;
      rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_left_;
      rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_right_;

      void recv_twist(const geometry_msgs::msg::Twist::ConstSharedPtr msg) {
        double rw = 0.3;
        double W = 1.2;

        double v = msg->linear.x;
        double pdot = msg->angular.z;

        double left_speed = (v - 0.5 * W * pdot) / rw;
        double right_speed = (v + 0.5 * W * pdot) / rw;

        std_msgs::msg::Float64 left_speed_msg;
        std_msgs::msg::Float64 right_speed_msg;

        left_speed_msg.data = left_speed;
        right_speed_msg.data = right_speed;

        pub_left_->publish(left_speed_msg);
        pub_right_->publish(right_speed_msg);
      }

  };

}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<diff_drive_example::DiffDriveExample>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}
