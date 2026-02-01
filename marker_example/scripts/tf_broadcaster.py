#! /usr/bin/env python3
import math
import rclpy
from rclpy.node import Node
from rcl_interfaces.msg import SetParametersResult

import tf2_ros
from geometry_msgs.msg import TransformStamped


class TfBroadcaster(Node):
    def __init__(self):
        super().__init__('tf_broadcaster')
        # Timer to update TF frame periodically
        self.timer = self.create_timer(timer_period_sec=0.02, callback=self.timer_callback)

        # TODO: Initialize a TF broadcaster instance

        # TODO: Create stamped transform message property and set the parent and child frame IDs

        # TODO: Declare parameters for 2-D position and angle, and bind an update callback

    def param_change_cb(self, params):
        # TODO: Update values in stamped transform with the latest parameter values
        return SetParametersResult(successful=True)

    def timer_callback(self):
        # TODO: Update the timestamp in the stamped transform header and update the TF frame using the broadcaster
        pass


if __name__ == '__main__':
    rclpy.init()
    node_instance = TfBroadcaster()
    rclpy.spin(node_instance)
