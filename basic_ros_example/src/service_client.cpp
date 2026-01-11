#include <rclcpp/rclcpp.hpp>
#include <basic_ros_example/srv/adder.hpp>

namespace basic_ros_example {

  class ServiceClient : public rclcpp::Node {
    public:
      ServiceClient()
      : rclcpp::Node("service_client")
      {
        adder_service_ = create_client<basic_ros_example::srv::Adder>("adder_service");
      }

      bool service_available() {
        return adder_service_->wait_for_service(std::chrono::milliseconds(2000));
      }

      void call_service(double val1, double val2) {
        auto req = std::make_shared<basic_ros_example::srv::Adder_Request>();
        req->val1 = val1;
        req->val2 = val2;
        auto response = adder_service_->async_send_request(req);

        if (rclcpp::spin_until_future_complete(shared_from_this(), response) == rclcpp::FutureReturnCode::SUCCESS) {
          RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Service succeeded! Result: " << response.get()->result);
        } else {
          RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Service call failed!");
        }
      }

    private:
      rclcpp::Client<basic_ros_example::srv::Adder>::SharedPtr adder_service_;

  };

}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<basic_ros_example::ServiceClient>();

  if (!node_instance->service_available()) {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Service is not available");
    rclcpp::shutdown();
    return 1;
  }

  node_instance->call_service(4.5, 1.0);

  rclcpp::shutdown();
  return 0;
}
