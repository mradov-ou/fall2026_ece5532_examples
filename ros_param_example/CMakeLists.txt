cmake_minimum_required(VERSION 3.5)
project(ros_param_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(param_example
  src/param_example.cpp
)

install(
  PROGRAMS
    scripts/param_example.py
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY
    config
    launch
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()
