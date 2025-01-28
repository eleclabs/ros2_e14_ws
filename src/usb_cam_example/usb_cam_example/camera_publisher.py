import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2

class CameraPublisher(Node):
    def __init__(self):
        super().__init__('camera_publisher')
        self.publisher_ = self.create_publisher(Image, 'camera_image', 10)
        self.timer = self.create_timer(0.1, self.timer_callback)  # Publish at 10 Hz
        self.bridge = CvBridge()
        self.cap = cv2.VideoCapture(0)  # เปิดกล้อง USB (device index 0)

        if not self.cap.isOpened():
            self.get_logger().error('Failed to open camera')
            rclpy.shutdown()

    def timer_callback(self):
        ret, frame = self.cap.read()
        if ret:
            # แปลงภาพ OpenCV เป็น ROS Image message
            img_msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
            self.publisher_.publish(img_msg)
        else:
            self.get_logger().error('Failed to capture frame')

def main(args=None):
    rclpy.init(args=args)
    node = CameraPublisher()
    rclpy.spin(node)
    node.cap.release()  # ปิดกล้องเมื่อหยุดการทำงาน
    rclpy.shutdown()

if __name__ == '__main__':
    main()
