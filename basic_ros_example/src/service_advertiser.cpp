#include <rclcpp/rclcpp.hpp>
#include <basic_ros_example/srv/adder.hpp>

namespace basic_ros_example
{
  class ServiceAdvertiser : public rclcpp::Node {
    public:
      ServiceAdvertiser()
      : rclcpp::Node("service_advertiser")
      {
        service_advertiser_ = create_service<basic_ros_example::srv::Adder>("adder_service",
          std::bind(&ServiceAdvertiser::service_cb, this, std::placeholders::_1, std::placeholders::_2));
      }

    private:
      rclcpp::Service<basic_ros_example::srv::Adder>::SharedPtr service_advertiser_;

      void service_cb(const basic_ros_example::srv::Adder::Request::SharedPtr req,
                           basic_ros_example::srv::Adder::Response::SharedPtr res)
      {
        res->result = req->val1 + req->val2;
      }

  };

}


int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<basic_ros_example::ServiceAdvertiser>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}