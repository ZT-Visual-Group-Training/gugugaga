#include <opencv2/opencv.hpp>
#include <string>
using namespace cv;
using namespace std;

int main()
{
    
    Mat img = imread("../3.png");
    if(img.empty())
    {
        cout<<"该图片不存在"<<endl;
        return -1;
    }

    imshow("img",img);
    waitKey(0);

    return 0;
}