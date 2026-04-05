#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from rcl_interfaces.msg import SetParametersResult
import cv2
import cv_bridge
import numpy as np


class HoughTransformNode(Node):
    def __init__(self):
        super().__init__('hough_transform_node')

        self.blue_thres_ = self.declare_parameter('blue_thres', 130).value
        self.erode_size_ = self.declare_parameter('erode_size', 1).value
        self.dilate_size_ = self.declare_parameter('dilate_size', 1).value
        self.hough_rho_res_ = self.declare_parameter('hough_rho_res', 10).value
        self.hough_theta_res_ = self.declare_parameter('hough_theta_res', 0.05).value
        self.hough_threshold_ = self.declare_parameter('hough_threshold', 10).value
        self.hough_min_length_ = self.declare_parameter('hough_min_length', 20).value
        self.hough_max_gap_ = self.declare_parameter('hough_max_gap', 50).value
        self.add_on_set_parameters_callback(self.param_update)

        self.bridge_ = cv_bridge.CvBridge()
        self.sub_image_ = self.create_subscription(Image, 'image', self.recv_image, 1)

        cv2.namedWindow('Raw Image', cv2.WINDOW_AUTOSIZE)
        cv2.namedWindow('Blue Image', cv2.WINDOW_AUTOSIZE)
        cv2.namedWindow('Thres Image', cv2.WINDOW_AUTOSIZE)
        cv2.namedWindow('Erode Image', cv2.WINDOW_AUTOSIZE)
        cv2.namedWindow('Dilate Image', cv2.WINDOW_AUTOSIZE)
        cv2.namedWindow('Canny Image', cv2.WINDOW_AUTOSIZE)
        cv2.namedWindow('Lines Image', cv2.WINDOW_AUTOSIZE)

    def recv_image(self, msg):
        # Convert raw image from ROS image message into a numpy array
        raw_img = self.bridge_.imgmsg_to_cv2(msg, msg.encoding)

        cv2.imshow('Raw Image', raw_img)
        cv2.waitKey(1)

        # Split RGB image into its three separate channels
        split_images = cv2.split(raw_img)

        # Extract the blue channel into its own grayscale image
        blue_image = split_images[0]

        cv2.imshow('Blue Image', blue_image)
        cv2.waitKey(1)

        # Apply binary threshold to create a binary image where white pixels correspond to high blue values
        _, thres_img = cv2.threshold(blue_image, self.blue_thres_, 255, cv2.THRESH_BINARY)

        cv2.imshow('Thres Image', thres_img)
        cv2.waitKey(1)

        # Apply erosion to clean up noise
        erode_kernel = np.ones(shape=(self.erode_size_, self.erode_size_), dtype=np.uint8)
        erode_img = cv2.erode(thres_img, erode_kernel)

        cv2.imshow('Erode Image', erode_img)
        cv2.waitKey(1)

        # Apply dilation to expand regions that passed the erosion filter
        dilate_kernel = np.ones(shape=(self.dilate_size_, self.dilate_size_), dtype=np.uint8)
        dilate_img = cv2.dilate(erode_img, dilate_kernel)

        cv2.imshow('Dilate Image', dilate_img)
        cv2.waitKey(1)

        # Apply Canny edge detection to reduce the number of points passed to Hough Transform
        canny_img = cv2.Canny(dilate_img, 1, 2)
        cv2.imshow('Canny Image', canny_img)

        # Run Probabilistic Hough Transform algorithm to detect line segments
        line_segments = cv2.HoughLinesP(image=canny_img,
                                        rho=self.hough_rho_res_,
                                        theta=self.hough_theta_res_,
                                        threshold=self.hough_threshold_,
                                        minLineLength=self.hough_min_length_,
                                        maxLineGap=self.hough_max_gap_)

        # Draw detected Hough lines onto the raw image for visualization
        if line_segments is not None:
            for seg in line_segments:
                x1, y1, x2, y2 = seg[0]
                cv2.line(raw_img, (x1, y1), (x2, y2), (0, 0, 255))

        cv2.imshow('Lines Image', raw_img)
        cv2.waitKey(1)

    def param_update(self, parameters):
        for p in parameters:
            if p.name == 'blue_thres':
                self.blue_thres_ = p.value
            elif p.name == 'erode_size':
                self.erode_size_ = p.value
            elif p.name == 'dilate_size':
                self.dilate_size_ = p.value
            elif p.name == 'hough_rho_res':
                self.hough_rho_res_ = p.value
            elif p.name == 'hough_theta_res':
                self.hough_theta_res_ = p.value
            elif p.name == 'hough_threshold':
                self.hough_threshold_ = p.value
            elif p.name == 'hough_min_length':
                self.hough_min_length_ = p.value
            elif p.name == 'hough_max_gap':
                self.hough_max_gap_ = p.value

        result = SetParametersResult()
        result.successful = True
        return result


if __name__ == '__main__':
    rclpy.init()
    node_instance = HoughTransformNode()
    rclpy.spin(node_instance)
