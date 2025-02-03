#include <rclcpp/rclcpp.hpp>
//#include <image_transport/image_transport.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <opencv2/opencv.hpp>

class WebcamPublisher : public rclcpp::Node {
public:
    WebcamPublisher() : Node("webcam_publisher") {
        publisher_ = this->create_publisher<sensor_msgs::msg::CompressedImage>(
            "webcam/image/compressed", rclcpp::QoS(10).best_effort());

        timer_ = this->create_wall_timer(std::chrono::milliseconds(30), 
            std::bind(&WebcamPublisher::publish_frame, this));

        cap_.open(0);
        if (!cap_.isOpened()) {
            RCLCPP_ERROR(this->get_logger(), "Cannot open webcam!");
        }
    }

private:
    void publish_frame() {
        cv::Mat frame;
        cap_ >> frame;
        if (frame.empty()) return;

        cv::resize(frame, frame, cv::Size(640, 480));

        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);

        auto msg = sensor_msgs::msg::CompressedImage();
        msg.format = "jpeg";
        msg.data = buffer;
        msg.header.stamp = this->get_clock()->now();

        publisher_->publish(msg);
    }

    rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    cv::VideoCapture cap_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<WebcamPublisher>());
    rclcpp::shutdown();
    return 0;
}
