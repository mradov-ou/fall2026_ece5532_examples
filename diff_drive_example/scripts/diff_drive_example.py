#! /usr/bin/env python3
import rclpy
from rclpy.node import Node


class DiffDriveExample(Node):
    def __init__(self):
        super().__init__('diff_drive_example')


if __name__ == '__main__':
    rclpy.init()
    node_instance = DiffDriveExample()
    rclpy.spin(node_instance)
