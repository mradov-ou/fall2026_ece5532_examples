cmake_minimum_required(VERSION 3.5)
project(gnss_ekf_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(gnss_ekf_example src/gnss_ekf_example.cpp)

install(
  DIRECTORY
    launch
    rviz
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()