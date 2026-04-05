#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp>

namespace opencv_example {

  class HoughTransformNode : public rclcpp::Node {
    public:
      HoughTransformNode()
      : rclcpp::Node("hough_transform_node")
      {
        blue_thres_ = declare_parameter<int>("blue_thres", 130);
        erode_size_ = declare_parameter<int>("erode_size", 1);
        dilate_size_ = declare_parameter<int>("dilate_size", 1);
        hough_rho_res_ = declare_parameter<int>("hough_rho_res", 10);
        hough_theta_res_ = declare_parameter<double>("hough_theta_res", 0.05);
        hough_threshold_ = declare_parameter<int>("hough_threshold", 10);
        hough_min_length_ = declare_parameter<int>("hough_min_length", 20);
        hough_max_gap_ = declare_parameter<int>("hough_max_gap", 50);
        param_cb_ = add_on_set_parameters_callback(std::bind(&HoughTransformNode::param_update, this, std::placeholders::_1));

        sub_image_ = create_subscription<sensor_msgs::msg::Image>("image", 1, std::bind(&HoughTransformNode::recv_image, this, std::placeholders::_1));

        cv::namedWindow("Raw Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Blue Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Thres Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Erode Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Dilate Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Canny Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Lines Image", cv::WINDOW_AUTOSIZE);
      }

    private:
      // ROS parameters
      int blue_thres_;
      int erode_size_;
      int dilate_size_;
      int hough_rho_res_;
      double hough_theta_res_;
      int hough_threshold_;
      int hough_min_length_;
      int hough_max_gap_;

      rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_image_;
      void recv_image(const sensor_msgs::msg::Image::ConstSharedPtr msg) {
        // Convert raw image from ROS image message into a cv::Mat
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, msg->encoding);
        cv::Mat raw_img = cv_ptr->image;

        cv::imshow("Raw Image", raw_img);
        cv::waitKey(1);

        // Split RGB image into its three separate channels
        std::vector<cv::Mat> split_images;
        cv::split(raw_img, split_images);

        // Extract the blue channel into its own grayscale image
        cv::Mat blue_image = split_images[0];

        cv::imshow("Blue Image", blue_image);
        cv::waitKey(1);

        // Apply binary threshold to create a binary image where white pixels correspond to high blue values
        cv::Mat thres_img;
        cv::threshold(blue_image, thres_img, blue_thres_, 255, cv::THRESH_BINARY);

        cv::imshow("Thres Image", thres_img);
        cv::waitKey(1);

        // Apply erosion to clean up noise
        cv::Mat erode_img;
        cv::erode(thres_img, erode_img, cv::Mat::ones(erode_size_, erode_size_, CV_8U));

        cv::imshow("Erode Image", erode_img);
        cv::waitKey(1);

        // Apply dilation to expand regions that passed the erosion filter
        cv::Mat dilate_img;
        cv::dilate(erode_img, dilate_img, cv::Mat::ones(dilate_size_, dilate_size_, CV_8U));

        cv::imshow("Dilate Image", dilate_img);
        cv::waitKey(1);

        // Apply Canny edge detection to reduce the number of points that are passed to Hough Transform
        cv::Mat canny_img;
        cv::Canny(dilate_img, canny_img, 1, 2);
        cv::imshow("Canny Image", canny_img);

        // Run Probabilistic Hough Transform algorithm to detect line segments
        std::vector<cv::Vec4i> line_segments;
        cv::HoughLinesP(canny_img, line_segments, hough_rho_res_, hough_theta_res_,
                        hough_threshold_, hough_min_length_, hough_max_gap_);

        // Draw detected Hough lines onto the raw image for visualization
        for (int i=0; i<line_segments.size(); i++){
          cv::line(raw_img, cv::Point(line_segments[i][0], line_segments[i][1]),
            cv::Point(line_segments[i][2], line_segments[i][3]), cv::Scalar(0, 0, 255));
        }

        cv::imshow("Lines Image", raw_img);
        cv::waitKey(1);
      }

      rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_;
      rcl_interfaces::msg::SetParametersResult param_update(const std::vector<rclcpp::Parameter>& parameters) {
        for (const rclcpp::Parameter& p : parameters) {
          if (p.get_name() == "blue_thres") {
            blue_thres_ = p.as_int();
          } else if (p.get_name() == "erode_size") {
            erode_size_ = p.as_int();
          } else if (p.get_name() == "dilate_size") {
            dilate_size_ = p.as_int();
          } else if (p.get_name() == "hough_rho_res") {
            hough_rho_res_ = p.as_int();
          } else if (p.get_name() == "hough_theta_res") {
            hough_theta_res_ = p.as_double();
          } else if (p.get_name() == "hough_threshold") {
            hough_threshold_ = p.as_int();
          } else if (p.get_name() == "hough_min_length") {
            hough_min_length_ = p.as_int();
          } else if (p.get_name() == "hough_max_gap") {
            hough_max_gap_ = p.as_int();
          }
        }
        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
      }
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node_instance = std::make_shared<opencv_example::HoughTransformNode>();
  rclcpp::spin(node_instance);
  rclcpp::shutdown();
  return 0;
}