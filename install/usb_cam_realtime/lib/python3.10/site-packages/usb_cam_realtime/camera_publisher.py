import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy

class CameraPublisher(Node):
    def __init__(self):
        super().__init__('camera_publisher')

        # ตั้งค่า QoS เพื่อให้ลดความหน่วง
        qos_profile = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )

        self.publisher_ = self.create_publisher(Image, 'camera_image', qos_profile)
        self.timer = self.create_timer(0.03, self.timer_callback)  # Publish at ~30 FPS
        self.bridge = CvBridge()
        self.cap = cv2.VideoCapture(0)  # เปิดกล้อง USB (device index 0)

        if not self.cap.isOpened():
            self.get_logger().error('Failed to open camera')
            rclpy.shutdown()

        # ลดขนาดภาพเพื่อลดปริมาณข้อมูล
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    def timer_callback(self):
        ret, frame = self.cap.read()
        if ret:
            img_msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
            self.publisher_.publish(img_msg)
        else:
            self.get_logger().error('Failed to capture frame')

def main(args=None):
    rclpy.init(args=args)
    node = CameraPublisher()
    rclpy.spin(node)
    node.cap.release()
    rclpy.shutdown()
