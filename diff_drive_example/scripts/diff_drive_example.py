#! /usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from std_msgs.msg import Float64


class DiffDriveExample(Node):
    def __init__(self):
        super().__init__('diff_drive_example')
        self.sub_twist = self.create_subscription(topic='cmd_vel', msg_type=Twist, callback=self.recv_twist, qos_profile=1)
        self.pub_left = self.create_publisher(topic='left_speed', msg_type=Float64, qos_profile=1)
        self.pub_right = self.create_publisher(topic='right_speed', msg_type=Float64, qos_profile=1)

    def recv_twist(self, msg: Twist):
        rw = 0.3
        W = 1.2

        v = msg.linear.x
        pdot = msg.angular.z

        left_speed = (v - 0.5 * W * pdot) / rw
        right_speed = (v + 0.5 * W * pdot) / rw

        left_speed_msg = Float64(data=left_speed)
        right_speed_msg = Float64(data=right_speed)

        self.pub_left.publish(left_speed_msg)
        self.pub_right.publish(right_speed_msg)


if __name__ == '__main__':
    rclpy.init()
    node_instance = DiffDriveExample()
    rclpy.spin(node_instance)
