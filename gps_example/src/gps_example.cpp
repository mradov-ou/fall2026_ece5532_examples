#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <nav_msgs/msg/path.hpp>
#include "enu.hpp"

namespace gps_example {
  class GpsExample : public rclcpp::Node {
    public:
      GpsExample()
      : rclcpp::Node("gps_example")
      {
        double ref_lat = declare_parameter<double>("ref_lat", INFINITY);
        double ref_lon = declare_parameter<double>("ref_lon", INFINITY);
        ref_alt_ = declare_parameter<double>("ref_alt", INFINITY);
        if (!std::isfinite(ref_lat) || !std::isfinite(ref_lon) || !std::isfinite(ref_alt_)) {
          RCLCPP_ERROR(get_logger(), "Reference coordinate parameters not set!");
          rclcpp::shutdown();
          return;
        }

        // Subscribe to GPS position and advertise an ENU path
        sub_fix_ = create_subscription<sensor_msgs::msg::NavSatFix>("fix", 1, std::bind(&GpsExample::recv_fix, this, std::placeholders::_1));
        pub_path_ = create_publisher<nav_msgs::msg::Path>("path", 1);
        gps_path_msg_.header.frame_id = "world";

        // Calculate ECEF position and ECEF -> ENU rotation matrix at the reference coordinates
        ref_ecef_ = llh_to_ecef(ref_lat, ref_lon, ref_alt_);
        enu_rot_mat_ = llh_to_rot_mat(ref_lat, ref_lon);
      }

    private:
      rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr sub_fix_;
      rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr pub_path_;
      nav_msgs::msg::Path gps_path_msg_;
      Eigen::Vector3d ref_ecef_;
      Eigen::Matrix3d enu_rot_mat_;
      double ref_alt_;

      void recv_fix(const sensor_msgs::msg::NavSatFix::ConstSharedPtr msg) {
        // Convert incoming latitude/longitude/altitude data into ENU
        double altitude;
        if (!std::isfinite(msg->altitude)) {
          altitude = ref_alt_;
        } else {
          altitude = msg->altitude;
        }
        Eigen::Vector3d current_ecef = llh_to_ecef(msg->latitude, msg->longitude, altitude);
        Eigen::Vector3d current_enu = enu_rot_mat_ * (current_ecef - ref_ecef_);

        // Append position to the end of the path message and then publish it out
        geometry_msgs::msg::PoseStamped new_path_point;
        new_path_point.pose.position.x = current_enu.x();
        new_path_point.pose.position.y = current_enu.y();
        new_path_point.pose.position.z = current_enu.z();
        gps_path_msg_.poses.push_back(new_path_point);

        gps_path_msg_.header.stamp = get_clock()->now();
        pub_path_->publish(gps_path_msg_);
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<gps_example::GpsExample>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}
