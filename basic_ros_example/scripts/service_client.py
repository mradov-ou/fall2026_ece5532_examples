#! /usr/bin/env python3
import sys
import rclpy
from rclpy.node import Node
from basic_ros_example.srv import Adder

class ServiceClient(Node):
    def __init__(self):
        super().__init__('service_client')
        self.service = self.create_client(srv_name='adder_service', srv_type=Adder)

    def service_available(self):
        return self.service.wait_for_service(timeout_sec=2.0)

    def call_service(self, val1, val2):
        req = Adder.Request()
        req.val1 = val1
        req.val2 = val2
        future = self.service.call_async(req)
        rclpy.spin_until_future_complete(self, future)
        return future.result()


if __name__ == '__main__':
    rclpy.init()
    node_instance = ServiceClient()

    if not node_instance.service_available():
        node_instance.get_logger().error('Service is not available')
        rclpy.shutdown()
        sys.exit(1)

    try:
        response = node_instance.call_service(val1=4.5, val2=1.0)
        node_instance.get_logger().info(f'Service succeeded! Result: {response.result}')
    except:
        node_instance.get_logger().error('Service call failed!')
        sys.exit(1)
