#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
    const string imagePath = argc > 1 ? argv[1] : "assets/sample.png";
    const cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        cerr << "图片读取失败: " << imagePath << '\n';
        return 1;
    }

    cout << "图片尺寸: " << image.cols << " x " << image.rows << endl;
    cv::imshow("Training2 OpenCV", image);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
