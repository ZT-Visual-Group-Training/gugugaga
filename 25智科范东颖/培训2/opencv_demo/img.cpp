#include <opencv2/opencv.hpp>
using namespace cv;  

int main()
{
    Mat img = imread("test.png");
	imshow("Image", img);
	waitKey(0);
    return 0;
}
