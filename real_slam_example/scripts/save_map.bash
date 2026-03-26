#!/bin/bash
PACKAGE_PATH=$(ros2 pkg prefix --share real_slam_example)
ros2 service call /slam_toolbox/serialize_map slam_toolbox/srv/SerializePoseGraph "{filename: '${PACKAGE_PATH}/map'}"