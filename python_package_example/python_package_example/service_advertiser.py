#! /usr/bin/env python3
import rclpy
from rclpy.node import Node
from basic_ros_example.srv import Adder


class ServiceAdvertiser(Node):
    def __init__(self):
        super().__init__('service_advertiser')
        self.srv = self.create_service(srv_name='adder_service', srv_type=Adder, callback=self.srv_callback)

    def srv_callback(self, req: Adder.Request, res: Adder.Response):
        res.result = req.val1 + req.val2
        return res


def main(args=None):
    rclpy.init()
    node_instance = ServiceAdvertiser()
    rclpy.spin(node_instance)

if __name__ == '__main__':
    main()