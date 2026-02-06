import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    param_file = os.path.join(get_package_share_directory('ros_param_example'), 'config', 'param_values.yaml')

    param_example_node = Node(
        package='ros_param_example',
        executable='param_example',
        name='param_example',
        parameters=[param_file]
    )

    return LaunchDescription([param_example_node])
