#!/bin/bash
PACKAGE_PATH=$(ros2 pkg prefix --share maze_nav_example)
ros2 service call /slam_toolbox/serialize_map slam_toolbox/srv/SerializePoseGraph "{filename: '${PACKAGE_PATH}/map'}"