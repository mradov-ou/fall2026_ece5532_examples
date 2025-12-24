cmake_minimum_required(VERSION 3.5)
project(cpp_example)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(mult_example
  mult_example.cpp
)

ament_auto_add_executable(factorial_example
  factorial_example.cpp
  Factorial.cpp
)

ament_auto_package()