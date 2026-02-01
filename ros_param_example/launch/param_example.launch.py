from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # TODO: set parameter values for the node
    param_example_node = Node(
        package='ros_param_example',
        executable='param_example',
        name='param_example',
    )

    return LaunchDescription([param_example_node])
