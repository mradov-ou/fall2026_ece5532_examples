from setuptools import setup

package_name = 'sparkfun_imu_driver'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Micho Radovnikovich',
    maintainer_email='mtradovn@oakland.edu',
    description='Driver node for parsing the default serial output stream from an OpenLog Artemis IMU: https://www.sparkfun.com/products/16832',
    license='BSD',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'driver_node = sparkfun_imu_driver.driver_node:main',
        ],
    },
)