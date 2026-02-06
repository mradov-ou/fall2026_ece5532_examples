#! /usr/bin/env python3
import rclpy
from rclpy.node import Node
from rcl_interfaces.msg import SetParametersResult, ParameterDescriptor, ParameterType, FloatingPointRange

class ParamExample(Node):
    def __init__(self):
        super().__init__('param_example')
        self.bool_param_value = self.declare_parameter('bool_param', False).value
        self.string_param_value = self.declare_parameter('string_param', 'value').value

        param_desc = ParameterDescriptor(
            type=ParameterType.PARAMETER_DOUBLE,
            floating_point_range=[FloatingPointRange(from_value=-50.0, to_value=50.0)],
            description='This is a float parameter'
        )
        self.float_param_value = self.declare_parameter('float_param', 0.0, param_desc).value

        # Bind parameter update callback function here
        self.add_on_set_parameters_callback(self.param_change_cb)

        # Timer to print out latest parameter values periodically
        self.timer = self.create_timer(timer_period_sec=1.0, callback=self.timer_callback)

    def timer_callback(self):
        self.get_logger().info(f'\nFloat value: {self.float_param_value}\nBool value: {self.bool_param_value}\nString value: {self.string_param_value}')

    def param_change_cb(self, params):
        for p in params:
            if p.name == 'bool_param':
                self.bool_param_value = p.value
            elif p.name == 'string_param':
                self.string_param_value = p.value
            elif p.name == 'float_param':
                self.float_param_value = p.value

        return SetParametersResult(successful=True)



if __name__ == '__main__':
    rclpy.init()
    node_instance = ParamExample()
    rclpy.spin(node_instance)
