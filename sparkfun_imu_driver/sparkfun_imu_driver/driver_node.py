#! /usr/bin/env python3
import math
import sys
import threading
import rclpy
from rclpy.node import Node
import serial
from sensor_msgs.msg import Imu

def main(args=None):
    rclpy.init()
    node_instance = Node(node_name='driver_node')
    device = node_instance.declare_parameter(name='serial_device', value='/dev/sparkfun_imu').value
    baud_rate = node_instance.declare_parameter(name='baud_rate', value=500000).value

    thread = threading.Thread(target=rclpy.spin, args=(node_instance, ), daemon=True)
    thread.start()

    rate = node_instance.create_rate(frequency=1.0, clock=node_instance.get_clock())
    serial_port = None
    while rclpy.ok():
        try:
            serial_port = serial.Serial(port=device, baudrate=baud_rate, timeout=1)
            node_instance.get_logger().info('Successfully connected to IMU!')
            break
        except serial.serialutil.SerialException as e:
            node_instance.get_logger().warn(f'Failed to connect to IMU: {e}')
            rate.sleep()

    if not rclpy.ok():
        sys.exit(1)

    pub_imu = node_instance.create_publisher(msg_type=Imu, topic='imu', qos_profile=1)

    while rclpy.ok():
        if not serial_port.is_open:
            node_instance.get_logger().error('Serial port closed... Shutting down')
            sys.exit(0)

        try:
            line = serial_port.readline().decode()
        except UnicodeDecodeError as e:
            continue
        except serial.SerialException as e:
            node_instance.get_logger().error('Lost connection to IMU...')
            serial_port.close()
            continue


        ascii_fields = [s.strip() for s in line.split(',')]
        if len(ascii_fields) != 7:
            continue

        accel_scale_factor = 0.01
        gyro_scale_factor = math.pi / 180 # dps --> rad/s

        try:
            imu_msg = Imu()
            imu_msg.header.stamp = node_instance.get_clock().now().to_msg()
            imu_msg.header.frame_id = 'map'

            # Negate x and z components to rotate sensor's reference frame by 180 degrees about y
            imu_msg.linear_acceleration.x = -accel_scale_factor * float(ascii_fields[0])
            imu_msg.linear_acceleration.y = accel_scale_factor * float(ascii_fields[1])
            imu_msg.linear_acceleration.z = -accel_scale_factor * float(ascii_fields[2])
            imu_msg.angular_velocity.x = -gyro_scale_factor * float(ascii_fields[3])
            imu_msg.angular_velocity.y = gyro_scale_factor * float(ascii_fields[4])
            imu_msg.angular_velocity.z = -gyro_scale_factor * float(ascii_fields[5])
            pub_imu.publish(imu_msg)
        except ValueError as e:
            pass


    rclpy.spin_once(node=node_instance)
    rclpy.shutdown()
    thread.join()

if __name__ == '__main__':
    main()
