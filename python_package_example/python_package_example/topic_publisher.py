#! /usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class TopicPublisher(Node):
    def __init__(self):
        super().__init__('topic_publisher')
        self.sub_string = self.create_subscription(topic='topic_in', msg_type=String, callback=self.recv_string, qos_profile=1)
        self.pub_string = self.create_publisher(topic='topic_out', msg_type=String, qos_profile=1)

    def recv_string(self, msg):
        new_string_msg = String()
        new_string_msg.data = msg.data + '_123'
        self.pub_string.publish(new_string_msg)


def main(args=None):
    rclpy.init()
    node_instance = TopicPublisher()
    rclpy.spin(node_instance)

if __name__ == '__main__':
    main()
