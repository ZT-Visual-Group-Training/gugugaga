#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat img = cv::imread("images/apple.jpg");
    if (img.empty()) {
        std::cout << "failed to load image" << std::endl;
        return -1;
    }
    std::cout << "image loaded: " << img.cols << " x " << img.rows << std::endl;
    cv::imshow("opencv_demo", img);
    cv::waitKey(0);
    return 0;
}
