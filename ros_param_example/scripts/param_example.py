#! /usr/bin/env python3
import rclpy
from rclpy.node import Node

class ParamExample(Node):
    def __init__(self):
        super().__init__('param_example')
        self.bool_param_value = False # TODO: declare boolean ROS parameter here
        self.string_param_value = '' # TODO: declare string ROS parameter here
        self.float_param_value = 0.0 # TODO: declare float ROS parameter here

        # TODO: Bind parameter update callback function here

        # Timer to print out latest parameter values periodically
        self.timer = self.create_timer(timer_period_sec=1.0, callback=self.timer_callback)

    def timer_callback(self):
        self.get_logger().info(f'\nFloat value: {self.float_param_value}\nBool value: {self.bool_param_value}\nString value: {self.string_param_value}')


if __name__ == '__main__':
    rclpy.init()
    node_instance = ParamExample()
    rclpy.spin(node_instance)
