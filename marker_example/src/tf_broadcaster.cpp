#include <rclcpp/rclcpp.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <rcl_interfaces/msg/set_parameters_result.hpp>

namespace marker_example {

  class TfBroadcaster : public rclcpp::Node {
    public:
      TfBroadcaster()
      : rclcpp::Node("tf_broadcaster")
      {
        // Timer to update TF frame periodically
        timer = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(20), std::bind(&TfBroadcaster::timer_callback, this));

        // TODO: Initialize a TF broadcaster instance

        // TODO: Create stamped transform message property and set the parent and child frame IDs

        // TODO: Declare parameters for 2-D position and angle, and bind an update callback
      }

    private:

      rclcpp::TimerBase::SharedPtr timer;

      void timer_callback() {
        // TODO: Update the timestamp in the stamped transform header and update the TF frame using the broadcaster
      }

      rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb;
      rcl_interfaces::msg::SetParametersResult param_update(const std::vector<rclcpp::Parameter>& parameters) {
        // TODO: Update values in stamped transform with the latest parameter values

        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<marker_example::TfBroadcaster>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}
