#include <iostream>
#include <opencv2/opencv.hpp>
int main(){
cv::Mat img  = cv::imread("test.png");
if (img.empty()){
    std::cout << "未识别到图片" << std::endl;
    return 1;
}
cv::imshow("img",img);
cv::waitKey(0);
cv::destroyAllWindows();
}