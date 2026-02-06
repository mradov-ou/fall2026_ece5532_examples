#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>

namespace ros_param_example {

  class ParamExample : public rclcpp::Node {
    public:
      ParamExample()
      : rclcpp::Node("param_example")
      {
        bool_param_value_ = declare_parameter<bool>("bool_param", false);
        string_param_value_ = declare_parameter<std::string>("string_param", "value");

        rcl_interfaces::msg::ParameterDescriptor param_desc;
        param_desc.type = rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
        param_desc.floating_point_range.resize(1);
        param_desc.floating_point_range[0].from_value = -50.0;
        param_desc.floating_point_range[0].to_value = 50.0;
        param_desc.description = "This is a float parameter";
        float_param_value_ = declare_parameter<double>("float_param", 0.0, param_desc);

        // Bind parameter update callback function here
        param_cb_ = add_on_set_parameters_callback(std::bind(&ParamExample::param_update, this, std::placeholders::_1));

        // Timer to print out latest parameter values periodically
        timer_ = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(1000), std::bind(&ParamExample::timer_callback, this));
      }

    private:
      rclcpp::TimerBase::SharedPtr timer_;
      rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_;

      rcl_interfaces::msg::SetParametersResult param_update(const std::vector<rclcpp::Parameter>& parameters) {

        for (const rclcpp::Parameter& p : parameters) {
          if (p.get_name() == "bool_param") {
            bool_param_value_ = p.as_bool();
          } else if (p.get_name() == "string_param") {
            string_param_value_ = p.as_string();
          } else if (p.get_name() == "float_param") {
            float_param_value_ = p.as_double();
          }
        }

        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
      }

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
