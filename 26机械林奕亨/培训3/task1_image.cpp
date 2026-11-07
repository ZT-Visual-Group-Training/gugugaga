#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // 1. 读取图片并转 HSV
    Mat img = imread("armor.jpg");
    if (img.empty()) { cout << "图片读取失败" << endl; return -1; }
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    // 2. 分离 H, S, V 三个通道
    vector<Mat> channels;
    split(hsv, channels);

    // 3. 弹出三个窗口展示
    imshow("H Channel", channels[0]);
    imshow("S Channel", channels[1]);
    imshow("V Channel", channels[2]);

    // 4. 提取红色二值掩膜
    // 红色在 HSV 中分布在两端，所以需要两个区间合并
    Mat mask1, mask2, red_mask;
    inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1);
    inRange(hsv, Scalar(160, 100, 100), Scalar(180, 255, 255), mask2);
    bitwise_or(mask1, mask2, red_mask); // 合并两个掩膜

    // 5. 保存掩膜图片
    imwrite("red_mask.png", red_mask);
    cout << "red_mask.png 保存成功！" << endl;
    imshow("Red Mask", red_mask);

    waitKey(0);
    return 0;
}
