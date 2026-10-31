#include <opencv2/opencv.hpp>   
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    string imagePath = "test.jpg";
    // 如果用户在命令行传了路径，就用用户传的
    if (argc > 1) {
        imagePath = argv[1];
    }

    cv::Mat image = cv::imread(imagePath);

    if (image.empty()) {
        cerr << "错误：无法读取图片，请检查路径：" << imagePath << endl;
        return -1;
    }

    cv::imshow("Image Window", image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}