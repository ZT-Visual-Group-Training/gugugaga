#include <opencv2/opencv.hpp>   
#include <iostream>
using namespace std;
int main() {
    // 读取当前目录下的 test.jpg
    cv::Mat image = cv::imread("test.jpg");

    // 检查图片是否读取成功
    if (image.empty()) {
       cerr << "错误：无法读取图片，请检查 test.jpg 是否在运行目录下。" << endl;
        return -1;
    }

    // 创建一个窗口并显示图片
    cv::imshow("显示图片", image);

    // 等待用户按任意键，然后关闭窗口
   // waitKey(0) 中的 0 表示无限等待
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}