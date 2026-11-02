#include <opencv2/opencv.hpp>
using namespace cv;  

int main()
{
    Mat img = imread("test.png");
    if (img.empty()) {
        std::cerr << "¶ÁÈ¡Ê§°Ü: test.png\n";
        return -1;
    }
	imshow("Image", img);
	waitKey(0);
    return 0;
}
