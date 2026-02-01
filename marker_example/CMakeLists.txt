cmake_minimum_required(VERSION 3.5)
project(marker_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(marker_example_node
  src/marker_example.cpp
)

ament_auto_add_executable(tf_broadcaster
  src/tf_broadcaster.cpp
)

install(
  PROGRAMS
    scripts/marker_example_node.py
    scripts/tf_broadcaster.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_auto_package()