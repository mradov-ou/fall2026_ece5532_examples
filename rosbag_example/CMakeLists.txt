cmake_minimum_required(VERSION 3.5)
project(rosbag_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(data_generator_node
  src/data_generator_node.cpp
)

ament_auto_add_executable(time_analysis_node
  src/time_analysis_node.cpp
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()
