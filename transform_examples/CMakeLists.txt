cmake_minimum_required(VERSION 3.5)
project(transform_examples)
set(CMAKE_CXX_STANDARD 20)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(example1_eigen
  src/example1_eigen.cpp
)

ament_auto_add_executable(example2_eigen
  src/example2_eigen.cpp
)

ament_auto_add_executable(example1_tf
  src/example1_tf.cpp
)

ament_auto_add_executable(example2_tf
  src/example2_tf.cpp
)

install(
  PROGRAMS
    scripts/example1_numpy.py
    scripts/example1_transforms3d.py
    scripts/example2_numpy.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_auto_package()