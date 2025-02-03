cmake_minimum_required(VERSION 3.8)
project(object_detection)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(object_detection_publisher src/object_detection_publisher.cpp)
target_link_libraries(object_detection_publisher ${OpenCV_LIBRARIES})
ament_target_dependencies(object_detection_publisher rclcpp sensor_msgs cv_bridge OpenCV)

add_executable(object_detection_subscriber src/object_detection_subscriber.cpp)
target_link_libraries(object_detection_subscriber ${OpenCV_LIBRARIES})
ament_target_dependencies(object_detection_subscriber rclcpp sensor_msgs cv_bridge OpenCV)

install(TARGETS
  object_detection_publisher
  object_detection_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
