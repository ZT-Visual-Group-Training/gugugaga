// opencv_demo - 培训2 作业2
// 26食质张锴锐
// 功能：使用 OpenCV 读取并显示一张图片
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
int main(int argc, char** argv)
{
    // 图片路径：优先用命令行参数，否则用默认的 zhuzhuxia.jpg
    std::string path = (argc > 1) ? argv[1] : "zhuzhuxia.jpg";

    // 读取图片（imread 返回 Mat 对象；路径错误时返回空 Mat）
    cv::Mat image = cv::imread(path);
    if (image.empty())
    {
        std::cerr << "错误：无法读取图片 -> " << path << std::endl;
        return -1;
    }

    // 打印图片信息，确认读取成功
    std::cout << "成功读取图片：" << path
              << "（宽 " << image.cols << " x 高 " << image.rows
              << "，通道数 " << image.channels() << "）" << std::endl;

    // 创建可缩放的显示窗口，显示图片
    cv::namedWindow("zhuzhuxia", cv::WINDOW_NORMAL);//windowsname+shuxing
    cv::imshow("zhuzhuxia", image); 

    std::cout << "按任意键关闭窗口..." << std::endl;
    cv::waitKey(0);          // 等待按键（0 表示无限等待）
    cv::destroyAllWindows(); // 关闭窗口

    return 0;
}
