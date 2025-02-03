#include <rclcpp/rclcpp.hpp>
//#include <image_transport/image_transport.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <fstream>

class ObjectDetectionPublisher : public rclcpp::Node {
public:
    ObjectDetectionPublisher() : Node("object_detection_publisher") {
        publisher_ = this->create_publisher<sensor_msgs::msg::CompressedImage>(
            "object_detection/image/compressed", rclcpp::QoS(10).best_effort());

        timer_ = this->create_wall_timer(std::chrono::milliseconds(30), 
            std::bind(&ObjectDetectionPublisher::publish_frame, this));

        cap_.open(0);
        if (!cap_.isOpened()) {
            RCLCPP_ERROR(this->get_logger(), "Cannot open webcam!");
        }

        // โหลด YOLO โมเดล
        net_ = cv::dnn::readNet("/home/eleclabs/yolo/yolov4.weights",
                                "/home/eleclabs/yolo/yolov4.cfg",
                                "Darknet");
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        load_class_names("/home/eleclabs/yolo/coco.names");
    }

private:
    void publish_frame() {
        cv::Mat frame;
        cap_ >> frame;
        if (frame.empty()) return;

        cv::resize(frame, frame, cv::Size(640, 480));

        // YOLO Object Detection
        detect_objects(frame);

        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);

        auto msg = sensor_msgs::msg::CompressedImage();
        msg.format = "jpeg";
        msg.data = buffer;
        msg.header.stamp = this->get_clock()->now();

        publisher_->publish(msg);
    }

    void detect_objects(cv::Mat &frame) {
        cv::Mat blob;
        cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(416, 416), true, false);
        net_.setInput(blob);
        
        std::vector<cv::Mat> outputs;
        net_.forward(outputs, get_output_layers());

        for (auto &output : outputs) {
            float *data = (float *)output.data;
            for (int i = 0; i < output.rows; i++, data += output.cols) {
                float confidence = data[4];
                if (confidence > 0.5) {
                    int class_id = std::max_element(data + 5, data + output.cols) - (data + 5);
                    cv::Rect box(data[0] * frame.cols, data[1] * frame.rows,
                                 data[2] * frame.cols, data[3] * frame.rows);
                    cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
                    cv::putText(frame, class_names_[class_id], box.tl(), cv::FONT_HERSHEY_SIMPLEX, 
                                0.5, cv::Scalar(0, 0, 255), 2);
                }
            }
        }
    }

    std::vector<std::string> get_output_layers() {
        static std::vector<std::string> layers = net_.getUnconnectedOutLayersNames();
        return layers;
    }

    void load_class_names(const std::string &path) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            class_names_.push_back(line);
        }
    }

    rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    cv::VideoCapture cap_;
    cv::dnn::Net net_;
    std::vector<std::string> class_names_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ObjectDetectionPublisher>());
    rclcpp::shutdown();
    return 0;
}
