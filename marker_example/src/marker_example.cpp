#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

namespace marker_example {

  class MarkerExample : public rclcpp::Node {
    public:
      MarkerExample()
      : rclcpp::Node("marker_example")
      {
        // Timer to publish markers periodically
        timer_ = rclcpp::create_timer(this, get_clock(), std::chrono::milliseconds(50), std::bind(&MarkerExample::timer_callback, this));

        // Advertise a MarkerArray topic
        pub_markers_ = create_publisher<visualization_msgs::msg::MarkerArray>("marker_array", 1);

        // Create cube marker in frame 'map'
        cube_marker_.header.frame_id = "map";
        cube_marker_.action = visualization_msgs::msg::Marker::ADD;
        cube_marker_.type = visualization_msgs::msg::Marker::CUBE;
        cube_marker_.id = 0;

        cube_marker_.pose.position.x = 5.0;
        cube_marker_.pose.position.y = 6.0;
        cube_marker_.pose.position.z = 0.0;
        cube_marker_.pose.orientation.w = 1.0;

        cube_marker_.scale.x = 1.0;
        cube_marker_.scale.y = 1.0;
        cube_marker_.scale.z = 1.0;

        cube_marker_.color.a = 1.0;
        cube_marker_.color.r = 1.0;
        cube_marker_.color.g = 1.0;
        cube_marker_.color.b = 0.0;

        // Create arrow marker frame 'frame1'
        arrow_marker_.header.frame_id = "frame1";
        arrow_marker_.action = visualization_msgs::msg::Marker::ADD;
        arrow_marker_.type = visualization_msgs::msg::Marker::ARROW;
        arrow_marker_.id = 1;

        arrow_marker_.pose.position.x = -5.0;
        arrow_marker_.pose.position.y = 6.0;
        arrow_marker_.pose.position.z = 0.0;
        arrow_marker_.pose.orientation.w = 1.0;

        arrow_marker_.scale.x = 2.0;
        arrow_marker_.scale.y = 0.1;
        arrow_marker_.scale.z = 0.1;

        arrow_marker_.color.a = 1.0;
        arrow_marker_.color.r = 1.0;
        arrow_marker_.color.g = 0.0;
        arrow_marker_.color.b = 1.0;

      }

    private:
      rclcpp::TimerBase::SharedPtr timer_;
      rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr pub_markers_;

      visualization_msgs::msg::Marker cube_marker_;
      visualization_msgs::msg::Marker arrow_marker_;

      void timer_callback() {
        /* Publish marker array message:
         - Create MarkerArray message instance
         - Populate array with constant cube and arrow markers
         - Update timestamp in each marker's header
         - Publish
        */

        rclcpp::Time current_time = get_clock()->now();
        cube_marker_.header.stamp = current_time;
        arrow_marker_.header.stamp = current_time;

        visualization_msgs::msg::MarkerArray marker_array_msg;
        marker_array_msg.markers.push_back(cube_marker_);
        marker_array_msg.markers.push_back(arrow_marker_);
        pub_markers_->publish(marker_array_msg);
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
