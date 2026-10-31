#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    const std::string image_path = argc > 1 ? argv[1] : "images/sample.ppm";
    const cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);

    if (image.empty())
    {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return 1;
    }

    std::cout << "Loaded image: " << image.cols << " x " << image.rows << std::endl;
    cv::imshow("OpenCV image", image);
    cv::waitKey(0);
    return 0;
}
