cmake_minimum_required(VERSION 3.5)
project(opencv_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
find_package(OpenCV REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(hough_transform_node src/hough_transform_node.cpp)
target_link_libraries(hough_transform_node
  ${OpenCV_LIBRARIES}
)

install(
  DIRECTORY
    config
    launch
  DESTINATION share/${PROJECT_NAME}
)

install(
  PROGRAMS
    scripts/hough_transform_node.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_auto_package()