#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
using namespace std;
using namespace cv;
int main()
{
    Mat img = imread("images/test.jpg");
    if(img.empty()){
        cout << "图片读取失败！" << endl;
        return 1;
    }
    cout << "图片读取成功，尺寸：" << img.cols << " x " << img.rows << endl;
    imshow("opencv_demo", img);
    waitKey(0);
    return 0;
}
