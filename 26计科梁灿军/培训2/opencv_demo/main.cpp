#include <iostream>

#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    // 没给参数就读 test.jpg
    std::string path = "test.jpg";
    if (argc > 1)
    {
        path = argv[1];
    }

    cv::Mat img = cv::imread(path);
    if (img.empty())
    {
        std::cout << "图片读取失败: " << path << std::endl;
        return -1;
    }

    std::cout << "宽: " << img.cols << "  高: " << img.rows
              << "  通道数: " << img.channels() << std::endl;
    std::cout << "OpenCV 版本: " << CV_VERSION << std::endl;

    cv::imshow("test", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
