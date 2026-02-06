#! /usr/bin/env python3
import rclpy
from rclpy.node import Node
from visualization_msgs.msg import MarkerArray, Marker


class MarkerExample(Node):
    def __init__(self):
        super().__init__('marker_example')

        # Timer to publish markers periodically
        self.timer = self.create_timer(timer_period_sec=0.05, callback=self.timer_callback)

        # Advertise a MarkerArray topic
        self.pub_markers = self.create_publisher(topic='marker_array', msg_type=MarkerArray, qos_profile=1)

        # Create cube marker in frame 'map'
        self.cube_marker = Marker()
        self.cube_marker.header.frame_id = "map"
        self.cube_marker.action = Marker.ADD
        self.cube_marker.type = Marker.CUBE
        self.cube_marker.id = 0

        self.cube_marker.pose.position.x = 5.0
        self.cube_marker.pose.position.y = 6.0
        self.cube_marker.pose.position.z = 0.0
        self.cube_marker.pose.orientation.w = 1.0

        self.cube_marker.scale.x = 1.0
        self.cube_marker.scale.y = 1.0
        self.cube_marker.scale.z = 1.0

        self.cube_marker.color.a = 1.0
        self.cube_marker.color.r = 1.0
        self.cube_marker.color.g = 1.0
        self.cube_marker.color.b = 0.0

        # Create arrow marker frame 'frame1'
        self.arrow_marker = Marker()
        self.arrow_marker.header.frame_id = "frame1"
        self.arrow_marker.action = Marker.ADD
        self.arrow_marker.type = Marker.ARROW
        self.arrow_marker.id = 1

        self.arrow_marker.pose.position.x = -5.0
        self.arrow_marker.pose.position.y = 6.0
        self.arrow_marker.pose.position.z = 0.0
        self.arrow_marker.pose.orientation.w = 1.0

        self.arrow_marker.scale.x = 2.0
        self.arrow_marker.scale.y = 0.1
        self.arrow_marker.scale.z = 0.1

        self.arrow_marker.color.a = 1.0
        self.arrow_marker.color.r = 1.0
        self.arrow_marker.color.g = 0.0
        self.arrow_marker.color.b = 1.0


    def timer_callback(self):
        """ TODO: Publish marker array message:
         - Create MarkerArray message instance
         - Populate array with constant cube and arrow markers
         - Update timestamp in each marker's header
         - Publish
        """
        current_time = self.get_clock().now()
        self.cube_marker.header.stamp = current_time.to_msg()
        self.arrow_marker.header.stamp = current_time.to_msg()

        marker_array_msg = MarkerArray()
        marker_array_msg.markers.append(self.cube_marker)
        marker_array_msg.markers.append(self.arrow_marker)
        self.pub_markers.publish(marker_array_msg)


if __name__ == '__main__':
    rclpy.init()
    node_instance = MarkerExample()
    rclpy.spin(node_instance)
