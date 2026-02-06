#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

int main(int argc, char** argv) {

  tf2::Vector3 translation(14, 14, 0);
  tf2::Quaternion q;
  q.setRPY(0, 0, 120 * M_PI / 180.0);

  tf2::Transform transform;
  transform.setOrigin(translation);
  transform.setRotation(q);

  tf2::Vector3 global_coordinates(5, 27, 0);

  tf2::Vector3 vehicle_coordinates = transform.inverse() * global_coordinates;

  printf("Target position in vehicle coordinates: (%f, %f, %f)\n",
      vehicle_coordinates.x(), vehicle_coordinates.y(), vehicle_coordinates.z());

  return 0;
}