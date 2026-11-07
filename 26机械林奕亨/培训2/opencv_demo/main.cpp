#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }
    
    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        std::cout << "无法读取图片，请检查路径是否正确: " << argv[1] << std::endl;
        return -1;
    }
    
    cv::imshow("OpenCV Test", img);
    cv::waitKey(0);
    return 0;
}
