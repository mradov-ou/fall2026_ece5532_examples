cmake_minimum_required(VERSION 3.5)
project(ament_package_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(basic_ros_example REQUIRED)

# Topic publisher / subscriber node
add_executable(topic_publisher src/topic_publisher.cpp)
ament_target_dependencies(topic_publisher
  rclcpp
  std_msgs
)

# Service advertiser node
add_executable(service_advertiser src/service_advertiser.cpp)
ament_target_dependencies(service_advertiser
  rclcpp
  basic_ros_example
)

# Install C++ executable targets
install(
  TARGETS
    topic_publisher
    service_advertiser
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
