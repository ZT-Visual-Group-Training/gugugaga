#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    try {
        cv::CommandLineParser args(argc, argv,
            "{help h||}{@image|armor.png|}{@output|.|}{no-show||}");
        if (args.has("help")) {
            std::cout << "hsv_demo [image] [output_dir] [--no-show]\n";
            return 0;
        }
        std::string path = args.get<std::string>(0);
        std::string output = args.get<std::string>(1);
        if (!args.check()) {
            args.printErrors();
            return 1;
        }
        cv::Mat image = cv::imread(path);
        if (image.empty()) {
            std::cerr << "Cannot open image: " << path << '\n';
            return 1;
        }
        std::filesystem::create_directories(output);
        cv::Mat hsv, low, high, mask;
        cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);
        cv::inRange(hsv, cv::Scalar(0, 100, 60), cv::Scalar(12, 255, 255), low);
        cv::inRange(hsv, cv::Scalar(168, 100, 60), cv::Scalar(179, 255, 255), high);
        cv::bitwise_or(low, high, mask);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE,
                        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
        auto save = [&](const std::string& name, const cv::Mat& mat) {
            auto file = std::filesystem::path(output) / name;
            if (!cv::imwrite(file.string(), mat)) {
                throw std::runtime_error("Cannot save: " + file.string());
            }
        };
        save("red_mask.png", mask);
        const std::vector<std::string> names = {"H", "S", "V"};
        for (int i = 0; i < 3; ++i) {
            save(names[i] + ".png", channels[i]);
        }
        std::cout << "Saved red_mask.png and H/S/V channels\n";
        if (!args.has("no-show")) {
            for (int i = 0; i < 3; ++i) {
                cv::namedWindow(names[i], cv::WINDOW_NORMAL);
                cv::resizeWindow(names[i], 600, 300);
                cv::moveWindow(names[i], 20 + (i % 2) * 630, 30 + (i / 2) * 380);
                cv::imshow(names[i], channels[i]);
            }
            cv::namedWindow("Red mask", cv::WINDOW_NORMAL);
            cv::resizeWindow("Red mask", 600, 300);
            cv::moveWindow("Red mask", 650, 410);
            cv::imshow("Red mask", mask);
            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
