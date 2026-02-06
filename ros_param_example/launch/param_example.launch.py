from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    param_example_node = Node(
        package='ros_param_example',
        executable='param_example',
        name='param_example',
        parameters=[{
            'float_param': 10.0,
            'bool_param': True,
            'string_param': 'foo'
        }]
    )

    return LaunchDescription([param_example_node])
