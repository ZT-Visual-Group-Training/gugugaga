#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
    // 命令行第一个参数是图片路径；不传时默认找当前目录下的 test.jpg
    const std::string imagePath = (argc > 1) ? argv[1] : "test.jpg";

    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "[错误] 读取图片失败: " << imagePath << '\n';
        std::cerr << "用法: " << argv[0] << " <图片路径>\n";
        std::cerr << "例如: " << argv[0] << " ..\\..\\data\\test.jpg\n";
        return 1;
    }

    std::cout << "读取成功: " << imagePath << '\n';
    std::cout << "图片尺寸: " << image.cols << " x " << image.rows << '\n';
    std::cout << "按任意键关闭窗口...\n";

    cv::imshow("OpenCV Demo", image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
