#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp> 
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;


Mat imread_chinese(const string& path, int flags = IMREAD_COLOR) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) return Mat();
    vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return imdecode(buffer, flags);
}

int main() {
    
    string imgPath = "C:/Users/Administrator/Desktop/图片1.jpg";
    Mat img = imread_chinese(imgPath);

    if (img.empty()) {
        cout << "无法读取图片，请检查路径是否正确: " << imgPath << endl;
        return -1;
    }

    
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    
    vector<Mat> hsvChannels;
    split(hsv, hsvChannels); 

    
    imshow("H Channel", hsvChannels[0]);
    imshow("S Channel", hsvChannels[1]);
    imshow("V Channel", hsvChannels[2]);
    imshow("Original", img);

   
    Mat mask1, mask2, redMask;
   
    inRange(hsv, Scalar(0, 70, 50), Scalar(10, 255, 255), mask1);
    inRange(hsv, Scalar(170, 70, 50), Scalar(180, 255, 255), mask2);
    bitwise_or(mask1, mask2, redMask);

    
    imshow("Red Mask", redMask);
    imwrite("red_mask.png", redMask);
    cout << "red_mask.png 已保存到当前工程目录！" << endl;

    ;waitKey(0);
    return 0;
}