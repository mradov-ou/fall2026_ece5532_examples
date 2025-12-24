cmake_minimum_required(VERSION 3.5)
project(py_example)
find_package(ament_cmake REQUIRED)

install(
  PROGRAMS
    scripts/factorial.py
    scripts/mult_example.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
