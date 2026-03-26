cmake_minimum_required(VERSION 3.5)
project(maze_nav_example)

find_package(ament_cmake_auto REQUIRED)

install(
  PROGRAMS
    scripts/drive_control.py
    scripts/save_map.bash
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY
  config
    launch
    rviz
    worlds
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
