cmake_minimum_required(VERSION 3.5)
project(diff_drive_example)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(${PROJECT_NAME}
  src/diff_drive_example.cpp
)

install(
  PROGRAMS
    scripts/diff_drive_example.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_auto_package()