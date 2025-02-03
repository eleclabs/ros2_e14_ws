#include <rclcpp/rclcpp.hpp>
//#include <image_transport/image_transport.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <opencv2/opencv.hpp>

class WebcamSubscriber : public rclcpp::Node {
public:
    WebcamSubscriber() : Node("webcam_subscriber") {
        subscription_ = this->create_subscription<sensor_msgs::msg::CompressedImage>(
            "webcam/image/compressed", rclcpp::QoS(10).best_effort(),
            std::bind(&WebcamSubscriber::image_callback, this, std::placeholders::_1));
    }

private:
    void image_callback(const sensor_msgs::msg::CompressedImage::SharedPtr msg) {
        try {
            cv::Mat image = cv::imdecode(msg->data, cv::IMREAD_COLOR);
            cv::imshow("Webcam Stream", image);
            cv::waitKey(1);
        } catch (std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "Error: %s", e.what());
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr subscription_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<WebcamSubscriber>());
    rclcpp::shutdown();
    return 0;
}
