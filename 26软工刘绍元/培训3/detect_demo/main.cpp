#include <opencv2/opencv.hpp>
#if __has_include(<opencv2/geometry/2d.hpp>)
#include <opencv2/geometry/2d.hpp>
#endif
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

struct Light {
    cv::Point2f center;
    std::array<cv::Point2f, 4> corners;
    double length;
    double angle;
};

struct Armor {
    int left;
    int right;
    double score;
};

std::vector<Light> findLights(const cv::Mat& image) {
    cv::Mat hsv, low, high, mask;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, cv::Scalar(0, 100, 60), cv::Scalar(12, 255, 255), low);
    cv::inRange(hsv, cv::Scalar(168, 100, 60), cv::Scalar(179, 255, 255), high);
    cv::bitwise_or(low, high, mask);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE,
                    cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<Light> lights;
    for (const auto& contour : contours) {
        if (cv::contourArea(contour) < 10) continue;
        cv::RotatedRect rect = cv::minAreaRect(contour);
        std::array<cv::Point2f, 4> corners;
        rect.points(corners.data());
        std::sort(corners.begin(), corners.end(), [](auto a, auto b) {
            return a.y < b.y;
        });
        if (corners[0].x > corners[1].x) std::swap(corners[0], corners[1]);
        if (corners[2].x > corners[3].x) std::swap(corners[2], corners[3]);
        cv::Point2f top = (corners[0] + corners[1]) * 0.5f;
        cv::Point2f bottom = (corners[2] + corners[3]) * 0.5f;
        double length = cv::norm(bottom - top);
        double width = cv::norm(corners[0] - corners[1]);
        if (width < 1 || length < 8) continue;
        double ratio = length / width;
        double angle = std::atan2(bottom.x - top.x, bottom.y - top.y) * 180 / CV_PI;
        if (ratio < 1.5 || ratio > 20 || std::abs(angle) > 45) continue;
        lights.push_back({rect.center, corners, length, angle});
    }
    double longest = 0;
    for (const auto& light : lights) longest = std::max(longest, light.length);
    lights.erase(std::remove_if(lights.begin(), lights.end(), [&](const Light& light) {
        return light.length < longest * 0.3;
    }), lights.end());
    std::sort(lights.begin(), lights.end(), [](const Light& a, const Light& b) {
        return a.center.x < b.center.x;
    });
    return lights;
}

std::vector<Armor> matchLights(const std::vector<Light>& lights) {
    std::vector<Armor> candidates;
    for (int i = 0; i < static_cast<int>(lights.size()); ++i) {
        for (int j = i + 1; j < static_cast<int>(lights.size()); ++j) {
            const auto& left = lights[i];
            const auto& right = lights[j];
            double length = (left.length + right.length) * 0.5;
            double sideRatio = std::max(left.length, right.length) /
                               std::min(left.length, right.length);
            cv::Point2f delta = right.center - left.center;
            double ratio = cv::norm(delta) / length;
            double roll = std::atan2(delta.y, delta.x) * 180 / CV_PI;
            double error = std::max(std::abs(left.angle + roll),
                                    std::abs(right.angle + roll));
            if (ratio < 1 || ratio > 5 || sideRatio > 1.6) continue;
            if (error > 28 || std::abs(left.angle - right.angle) > 22) continue;
            double score = (sideRatio - 1) * 3 + error / 28 + std::abs(ratio - 2.2) * 0.2;
            candidates.push_back({i, j, score});
        }
    }
    std::sort(candidates.begin(), candidates.end(), [](const Armor& a, const Armor& b) {
        return a.score < b.score;
    });
    std::vector<bool> used(lights.size(), false);
    std::vector<Armor> armors;
    for (const auto& armor : candidates) {
        if (used[armor.left] || used[armor.right]) continue;
        used[armor.left] = used[armor.right] = true;
        armors.push_back(armor);
    }
    return armors;
}

void drawResult(cv::Mat& image, const std::vector<Light>& lights,
                const std::vector<Armor>& armors) {
    for (const auto& light : lights) {
        const auto& p = light.corners;
        const std::array<int, 4> order = {0, 1, 3, 2};
        for (int k = 0; k < 4; ++k) {
            cv::line(image, p[order[k]], p[order[(k + 1) % 4]], cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
            cv::circle(image, p[k], 3, cv::Scalar(255, 255, 0), -1, cv::LINE_AA);
        }
    }
    for (const auto& armor : armors) {
        const auto& left = lights[armor.left].corners;
        const auto& right = lights[armor.right].corners;
        std::array<cv::Point2f, 4> corners = {left[0], right[1], right[3], left[2]};
        for (int k = 0; k < 4; ++k) {
            cv::line(image, corners[k], corners[(k + 1) % 4], cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
            cv::circle(image, corners[k], 5, cv::Scalar(0, 128, 255), -1, cv::LINE_AA);
            cv::Point label(cvRound(corners[k].x) + 7, cvRound(corners[k].y) - 7);
            cv::putText(image, "P" + std::to_string(k + 1), label,
                        cv::FONT_HERSHEY_SIMPLEX, 0.55, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        }
    }
}

int main(int argc, char** argv) {
    try {
        cv::CommandLineParser args(argc, argv,
            "{help h||}{@video|test_clip.mp4|}{@output|result.mp4|}{no-show||}{shots||}");
        if (args.has("help")) {
            std::cout << "detect_demo [video] [output.mp4] [--no-show] [--shots=directory]\n";
            return 0;
        }
        std::string input = args.get<std::string>(0);
        std::string output = args.get<std::string>(1);
        std::string shots = args.get<std::string>("shots");
        if (!args.check()) {
            args.printErrors();
            return 1;
        }
        if (std::filesystem::absolute(input).lexically_normal() ==
            std::filesystem::absolute(output).lexically_normal()) {
            std::cerr << "Input and output must be different files\n";
            return 1;
        }
        cv::VideoCapture capture(input);
        if (!capture.isOpened()) {
            std::cerr << "Cannot open video: " << input << '\n';
            return 1;
        }
        cv::Mat frame;
        if (!capture.read(frame)) {
            std::cerr << "Video has no readable frames\n";
            return 1;
        }
        double fps = capture.get(cv::CAP_PROP_FPS);
        if (!std::isfinite(fps) || fps <= 0) fps = 30;
        auto parent = std::filesystem::path(output).parent_path();
        if (!parent.empty()) std::filesystem::create_directories(parent);
        if (!shots.empty()) std::filesystem::create_directories(shots);
        cv::VideoWriter writer(output, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, frame.size());
        if (!writer.isOpened()) {
            std::cerr << "Cannot create video: " << output << '\n';
            return 1;
        }
        bool show = !args.has("no-show");
        if (show) {
            cv::namedWindow("Armor detection", cv::WINDOW_NORMAL);
            cv::resizeWindow("Armor detection", 960, 540);
            cv::moveWindow("Armor detection", 150, 100);
        }
        int frames = 0;
        int detected = 0;
        int totalArmors = 0;
        int totalLights = 0;
        const int delay = std::max(1, cvRound(1000 / fps));
        do {
            auto lights = findLights(frame);
            auto armors = matchLights(lights);
            totalLights += static_cast<int>(lights.size());
            totalArmors += static_cast<int>(armors.size());
            if (!armors.empty()) ++detected;
            drawResult(frame, lights, armors);
            writer.write(frame);
            if (!shots.empty() && frames % 120 == 0) {
                auto file = std::filesystem::path(shots) / ("frame_" + std::to_string(frames) + ".png");
                if (!cv::imwrite(file.string(), frame)) {
                    throw std::runtime_error("Cannot save screenshot");
                }
            }
            ++frames;
            if (show) {
                cv::imshow("Armor detection", frame);
                int key = cv::waitKey(delay);
                if (key == 27 || key == 'q') break;
                if (cv::getWindowProperty("Armor detection", cv::WND_PROP_VISIBLE) == 0) break;
            }
        } while (capture.read(frame));
        writer.release();
        capture.release();
        if (show) cv::destroyAllWindows();
        std::cout << "Frames: " << frames << "\nFrames with armor: " << detected
                  << "\nLightbars: " << totalLights << "\nArmors: " << totalArmors << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
