// armor.cpp —— 装甲板灯条识别：实现
#include "armor.hpp"

#include <algorithm>
#include <cmath>

namespace Armor
{
    // V 下限决定四个角点"咬"在哪：
    //   V 低（0）   → 轮廓包含暗红光晕，点落在红条外缘甚至更外
    //   V 高（170） → 轮廓只剩亮芯，点缩进红条内部
    // 视频里灯条边缘是渐变的，没有绝对"正确"的值，100 是折中：
    // 轮廓边界大致落在肉眼看到的红色边缘上。
    const cv::Scalar kLow(165, 100, 100);
    const cv::Scalar kHigh(180, 255, 255);

    // 筛选参数（都写在这儿，方便以后调）
    static const double kMinArea  = 1000;
    static const double kMaxArea  = 25000;
    static const double kMinRatio = 2.0;
    static const double kRatioH   = 1.5;
    static const double kDyRatio  = 0.4;

    cv::Mat makeMask(const cv::Mat& bgr)
    {
        cv::Mat hsv, mask;
        cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, kLow, kHigh, mask);

        cv::Mat k3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::Mat k5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

        // 先开（去零散小白点），再闭（补灯条上的断口）
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN,  k3);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, k5);

        return mask;
    }

    std::vector<Bar> findBars(const cv::Mat& mask)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<Bar> bars;
        for (const auto& c : contours)
        {
            cv::Rect r = cv::boundingRect(c);
            if (r.width <= 0) continue;

            double area  = cv::contourArea(c);
            double ratio = (double)r.height / r.width;

            // 三个相对判据：面积区间 + 细长比。别加宽度上限，
            // 灯条在某些角度会变宽，加死一定会漏帧。
            if (area > kMinArea && area < kMaxArea && ratio > kMinRatio)
            {
                Bar b;
                b.rect    = r;
                b.contour = c;
                bars.push_back(b);
            }
        }
        return bars;
    }

    bool isPair(const cv::Rect& a, const cv::Rect& b)
    {
        double h1 = a.height, h2 = b.height;
        double hmin = std::min(h1, h2);
        if (hmin <= 0) return false;

        bool heightOk = (std::max(h1, h2) / hmin) < kRatioH;
        bool topOk    = std::abs(a.y - b.y) < kDyRatio * hmin;
        return heightOk && topOk;
    }

    cv::Point extreme(const std::vector<cv::Point>& c, int mode)
    {
        cv::Point best = c[0];
        long long bestV = 0;

        for (size_t i = 0; i < c.size(); i++)
        {
            long long v;
            if (mode == 0 || mode == 2) v = (long long)c[i].x + c[i].y;
            else                        v = (long long)c[i].x - c[i].y;

            if (i == 0) { bestV = v; best = c[i]; continue; }

            bool better = (mode == 0 || mode == 3) ? (v < bestV) : (v > bestV);
            if (better) { bestV = v; best = c[i]; }
        }
        return best;
    }

    void drawArmor(cv::Mat& img, const Bar& a, const Bar& b)
    {
        const cv::Scalar YELLOW(0, 255, 255);
        const cv::Scalar GREEN(0, 255, 0);

        // 黄框：单根灯条的外接矩形（垫在底层）
        cv::rectangle(img, a.rect, YELLOW, 2);
        cv::rectangle(img, b.rect, YELLOW, 2);

        // 谁在左，用外接矩形的 x 判断
        const Bar& L = (a.rect.x < b.rect.x) ? a : b;
        const Bar& R = (a.rect.x < b.rect.x) ? b : a;

        // 四个角从轮廓上取，才贴灯条
        cv::Point p1 = extreme(L.contour, 0);   // 左上
        cv::Point p2 = extreme(R.contour, 1);   // 右上
        cv::Point p3 = extreme(R.contour, 2);   // 右下
        cv::Point p4 = extreme(L.contour, 3);   // 左下

        std::vector<cv::Point> pts = { p1, p2, p3, p4 };

        // ---- 时间平滑：新值 70% + 上一帧 30% ----
        // static：跨帧记住上一次的点，函数退出不销毁
        // 0.7 越大越跟手但越抖；0.7 越小越稳但会拖影（框比实物慢半拍）
        static std::vector<cv::Point> prevPts;
        const double wNew = 0.7, wOld = 0.3;

        if (prevPts.size() == pts.size())      // 第一帧 prevPts 为空，跳过
        {
            for (size_t k = 0; k < pts.size(); k++)
            {
                pts[k].x = (int)(wNew * pts[k].x + wOld * prevPts[k].x);
                pts[k].y = (int)(wNew * pts[k].y + wOld * prevPts[k].y);
            }
        }
        prevPts = pts;                         // 存给下一帧

        cv::polylines(img, pts, true, GREEN, 1);
        for (auto& p : pts)
            cv::circle(img, p, 5, GREEN, -1);
    }

    cv::Mat processFrame(const cv::Mat& bgr)
    {
        cv::Mat mask = makeMask(bgr);
        std::vector<Bar> bars = findBars(mask);

        cv::Mat out = bgr.clone();

        // 可能有多组候选，挑"高度差 + 顶边差"最小的那一对
        int bestI = -1, bestJ = -1;
        double bestScore = 1e18;

        for (size_t i = 0; i < bars.size(); i++)
        {
            for (size_t j = i + 1; j < bars.size(); j++)
            {
                if (!isPair(bars[i].rect, bars[j].rect)) continue;

                double hmin = std::min((double)bars[i].rect.height,
                                       (double)bars[j].rect.height);
                double score = std::abs(bars[i].rect.height - bars[j].rect.height)
                             + std::abs(bars[i].rect.y - bars[j].rect.y) / hmin * 100.0;

                if (score < bestScore)
                {
                    bestScore = score;
                    bestI = (int)i;
                    bestJ = (int)j;
                }
            }
        }

        if (bestI >= 0)
            drawArmor(out, bars[bestI], bars[bestJ]);

        return out;
    }
}
