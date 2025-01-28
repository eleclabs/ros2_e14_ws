from setuptools import find_packages, setup

package_name = 'usb_cam_realtime'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='eleclabs',
    maintainer_email='eleclabs@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'camera_publisher = usb_cam_realtime.camera_publisher:main',
            'camera_subscriber = usb_cam_realtime.camera_subscriber:main',
        ],
    },
)
