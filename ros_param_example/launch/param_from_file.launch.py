import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # TODO: Load parameters for the node from a YAML file
    param_example_node = Node(
        package='ros_param_example',
        executable='param_example',
        name='param_example',
    )

    return LaunchDescription([param_example_node])
