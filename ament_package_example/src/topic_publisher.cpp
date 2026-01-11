#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

namespace ament_package_example {

  class TopicPublisher : public rclcpp::Node {
    public:
      TopicPublisher()
      : rclcpp::Node("topic_publisher")
      {
        sub_string_ = create_subscription<std_msgs::msg::String>("topic_in", 1, std::bind(&TopicPublisher::recv_string, this, std::placeholders::_1));
        pub_string_ = create_publisher<std_msgs::msg::String>("topic_out", 1);
      }

    private:
      rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_string_;
      rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_string_;

      void recv_string(const std_msgs::msg::String::ConstSharedPtr msg) {
        std_msgs::msg::String new_string;
        new_string.data = msg->data + "_123";
        pub_string_->publish(new_string);
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<ament_package_example::TopicPublisher>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}