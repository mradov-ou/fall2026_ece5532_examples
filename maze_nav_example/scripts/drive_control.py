#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64
from geometry_msgs.msg import Twist

class RoundbotDriveControl(Node):
    WHEEL_RADIUS = 0.1
    WHEEL_SEPARATION = 0.5

    def __init__(self):
        super().__init__('drive_control')
        self.sub_cmd_vel = self.create_subscription(topic='cmd_vel', msg_type=Twist, callback=self.recv_cmd_vel, qos_profile=1)
        self.pub_left = self.create_publisher(topic='/roundbot/left_speed_cmd', msg_type=Float64, qos_profile=1)
        self.pub_right = self.create_publisher(topic='/roundbot/right_speed_cmd', msg_type=Float64, qos_profile=1)

        self.timer = self.create_timer(timer_period_sec=0.02, callback=self.timer_callback)
        self.cmd_vel = Twist()
        self.cmd_stamp_ns = 0

    def timer_callback(self):
        left_speed_msg = Float64()
        right_speed_msg = Float64()
        if 1e-9 * (self.get_clock().now().nanoseconds - self.cmd_stamp_ns) > 1.0:
            left_speed_msg.data = 0.0
            right_speed_msg.data = 0.0
        else:
            left_speed_msg.data = (self.cmd_vel.linear.x - 0.5 * RoundbotDriveControl.WHEEL_SEPARATION * self.cmd_vel.angular.z) / RoundbotDriveControl.WHEEL_RADIUS
            right_speed_msg.data = (self.cmd_vel.linear.x + 0.5 * RoundbotDriveControl.WHEEL_SEPARATION * self.cmd_vel.angular.z) / RoundbotDriveControl.WHEEL_RADIUS

        self.pub_left.publish(left_speed_msg)
        self.pub_right.publish(right_speed_msg)

    def recv_cmd_vel(self, msg: Twist):
        self.cmd_vel = msg
        self.cmd_stamp_ns = self.get_clock().now().nanoseconds


if __name__ == '__main__':
    rclpy.init()
    node_instance = RoundbotDriveControl()
    rclpy.spin(node_instance)
