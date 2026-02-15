cmake_minimum_required(VERSION 3.5)
project(dead_reckoning_odom)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(${PROJECT_NAME} src/dead_reckoning_node.cpp)

install(
  DIRECTORY
    launch
    rviz
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()