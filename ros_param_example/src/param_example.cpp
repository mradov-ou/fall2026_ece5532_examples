#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>

namespace ros_param_example {

  class ParamExample : public rclcpp::Node {
    public:
      ParamExample()
      : rclcpp::Node("param_example")
      {
        bool_param_value_ = false;  // TODO: Declare boolean ROS parameter here
        string_param_value_ = "";   // TODO: Declare string ROS parameter here
        float_param_value_ = 0.0;   // TODO: Declare double ROS parameter here

        // TODO: Bind parameter update callback function here

        // Timer to print out latest parameter values periodically
        timer_ = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(1000), std::bind(&ParamExample::timer_callback, this));
      }

    private:
      rclcpp::TimerBase::SharedPtr timer_;

      double float_param_value_;
      bool bool_param_value_;
      std::string string_param_value_;

      void timer_callback() {
        RCLCPP_INFO_STREAM(get_logger(), std::endl << "Float value: " << float_param_value_ << std::endl << "Bool value: " << (bool_param_value_ ? "true" : "false") << std::endl << "String value: " << string_param_value_);
      }

  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<ros_param_example::ParamExample>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}
