import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy

class CameraSubscriber(Node):
    def __init__(self):
        super().__init__('camera_subscriber')

        # ตั้งค่า QoS ให้ตรงกับ Publisher
        qos_profile = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )

        self.subscription = self.create_subscription(
            Image,
            'camera_image',
            self.listener_callback,
            qos_profile
        )
        self.bridge = CvBridge()

    def listener_callback(self, msg):
        # แปลง ROS Image message เป็น OpenCV image
        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        cv2.imshow('Real-Time Camera Image', frame)

        # กด 'q' เพื่อออก
        if cv2.waitKey(1) & 0xFF == ord('q'):
            rclpy.shutdown()

def main(args=None):
    rclpy.init(args=args)
    node = CameraSubscriber()
    rclpy.spin(node)
    cv2.destroyAllWindows()
