#! /usr/bin/env python3
import rclpy
from rclpy.node import Node


class MarkerExample(Node):
    def __init__(self):
        super().__init__('marker_example')

        # Timer to publish markers periodically
        self.timer = self.create_timer(timer_period_sec=0.05, callback=self.timer_callback)

        # TODO: Advertise a MarkerArray topic

        # TODO: Create cube marker in frame 'map'

        # TODO: Create arrow marker frame 'frame1'

    def timer_callback(self):
        """ TODO: Publish marker array message:
         - Create MarkerArray message instance
         - Populate array with constant cube and arrow markers
         - Update timestamp in each marker's header
         - Publish
        """
        pass


if __name__ == '__main__':
    rclpy.init()
    node_instance = MarkerExample()
    rclpy.spin(node_instance)
