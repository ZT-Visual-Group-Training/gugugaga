// armor.hpp —— 装甲板灯条识别：对外声明
#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

namespace Armor
{
    // 红色阈值（H 165~180 覆盖 OpenCV 里红色的高值段）
    extern const cv::Scalar kLow;
    extern const cv::Scalar kHigh;

    // 一根灯条：外接矩形（筛选/配对用） + 轮廓本身（取角点用）
    struct Bar
    {
        cv::Rect rect;
        std::vector<cv::Point> contour;
    };

    // 从 BGR 图生成红色二值掩膜（已做开闭运算）
    cv::Mat makeMask(const cv::Mat& bgr);

    // 在掩膜里挑出"像灯条"的轮廓
    std::vector<Bar> findBars(const cv::Mat& mask);

    // 两根灯条是否像同一块装甲板上的左右灯条
    bool isPair(const cv::Rect& a, const cv::Rect& b);

    // 取轮廓的极值点：0=左上 1=右上 2=右下 3=左下
    cv::Point extreme(const std::vector<cv::Point>& c, int mode);

    // 在图上画黄框 + 绿框 + 四个角点
    void drawArmor(cv::Mat& img, const Bar& a, const Bar& b);

    // 一帧完整处理：BGR 进，画好框的 BGR 出
    cv::Mat processFrame(const cv::Mat& bgr);
}
