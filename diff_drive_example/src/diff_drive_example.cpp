#include <rclcpp/rclcpp.hpp>

namespace diff_drive_example {

  class DiffDriveExample : public rclcpp::Node {
    public:
      DiffDriveExample()
      : rclcpp::Node("diff_drive_example")
      {
      }

  };

}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<diff_drive_example::DiffDriveExample>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}
