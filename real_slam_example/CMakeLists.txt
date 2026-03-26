cmake_minimum_required(VERSION 3.5)
project(real_slam_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake REQUIRED)

install(
    DIRECTORY
        config
        launch
        rviz
    DESTINATION share/${PROJECT_NAME}
)

install(
    PROGRAMS
        scripts/save_map.bash
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
