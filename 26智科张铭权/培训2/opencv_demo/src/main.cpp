#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
    Mat img = imread("/media/sf_win_share/test1.jpg");

    if(img.empty())
    {
        cout << "图片读取失败" << endl;
        return -1;
    }

    imshow("image window", img);
    waitKey(0);

    return 0;
}
