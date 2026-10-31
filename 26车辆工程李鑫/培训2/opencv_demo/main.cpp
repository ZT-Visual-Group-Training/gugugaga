#include <iostream>
#include <opencv2/opencv.hpp>

int main() {

    cv::Mat img = cv::imread("test.jpg");
    
    if (img.empty()) {
        std::cerr << "错误: 无法读取图片！请确保当前目录下有 test.jpg" << std::endl;
        return -1;
    }
    
    std::cout << "成功读取图片，尺寸: " << img.cols << " x " << img.rows << std::endl;
    
    cv::imshow("My Image", img);
    cv::waitKey(0); 
    return 0;
}
