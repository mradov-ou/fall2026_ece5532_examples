#include <rclcpp/rclcpp.hpp>

namespace marker_example {

  class MarkerExample : public rclcpp::Node {
    public:
      MarkerExample()
      : rclcpp::Node("marker_example")
      {
        // Timer to publish markers periodically
        timer_ = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(50), std::bind(&MarkerExample::timer_callback, this));

        // TODO: Advertise a MarkerArray topic

        // TODO: Create cube marker in frame 'map'

        // TODO: Create arrow marker frame 'frame1'

      }

    private:
      rclcpp::TimerBase::SharedPtr timer_;

      void timer_callback() {
        /* TODO: Publish marker array message:
         - Create MarkerArray message instance
         - Populate array with constant cube and arrow markers
         - Update timestamp in each marker's header
         - Publish
        */
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<marker_example::MarkerExample>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}
