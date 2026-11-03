/**
 * 培训3 · 任务2：视频里识别灯条和装甲板
 *
 * 流程：
 *   1. 读视频（默认 test_clip.mp4，可用命令行传路径）
 *   2. 转 HSV，用两段红色区间提取红色（暗光下灰度阈值会失效）
 *   3. 找轮廓 -> minAreaRect 框住灯条，按面积/长宽比/倾角过滤
 *   4. 灯条两两配对成装甲板
 *   5. 画：灯条外接矩形（黄）、灯条四个角点（红点）、
 *          装甲板四个角点 P1~P4、装甲板四边形（绿）
 *
 * 编译：
 *   cmake -S . -B build && cmake --build build -j
 *
 * 运行：
 *   ./build/detect_demo                 # 处理视频并保存，不弹窗
 *   ./build/detect_demo --show          # 弹窗实时看
 *   ./build/detect_demo out.mp4 --show  # 指定输出视频
 */

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------- 阈值参数
// 红色跨 0 度，两段区间取并集
const cv::Scalar kRedLower1(0, 100, 60);
const cv::Scalar kRedUpper1(10, 255, 255);
const cv::Scalar kRedLower2(160, 100, 60);
const cv::Scalar kRedUpper2(179, 255, 255);

// 灯条筛选：细长、竖直、面积不过小
const double kMinArea = 30.0;        // 轮廓最小面积
const double kMinAspect = 1.8;       // 长边 / 短边 最小比值
const double kMaxAngle = 45.0;       // 与竖直方向最大夹角（度）

// 灯条配对成装甲板
const double kMaxAngleDiff = 12.0;   // 两条灯条角度差上限
const double kMaxHeightRatio = 1.8;  // 两条灯条长度比上限
const double kMinGapRatio = 1.0;     // 两灯条中心距 / 灯条高 的下限
                                     // 原始是 1.2，但装甲板倾斜时两灯条在画面上的
                                     // 水平间距会被压缩（实测低到 1.13），1.2 会漏配
const double kMaxGapRatio = 5.0;     // 上限

struct LightBar {
    cv::Point2f center;
    double angle = 0.0;      // 归一化到 [-45, 45]
    double length = 0.0;     // 长边
    double width = 0.0;      // 短边（灯条实际宽度）
    std::vector<cv::Point2f> corners;   // minAreaRect 的四个角
    cv::Rect2f box;                     // 摆正的外接矩形（尺寸 = 灯条实际宽高）
};

// ---------------------------------------------------------------- 工具函数
/** 4 个点按 左上 / 右上 / 右下 / 左下 排序（对倾斜矩形同样有效） */
std::vector<cv::Point2f> orderCorners(const std::vector<cv::Point2f> &pts) {
    std::vector<cv::Point2f> p = pts;

    // x + y 最小 = 左上，最大 = 右下
    std::sort(p.begin(), p.end(), [](const cv::Point2f &a, const cv::Point2f &b) {
        return (a.x + a.y) < (b.x + b.y);
    });
    cv::Point2f tl = p[0], br = p[3];

    // y - x 最小 = 右上，最大 = 左下
    std::sort(p.begin(), p.end(), [](const cv::Point2f &a, const cv::Point2f &b) {
        return (a.y - a.x) < (b.y - b.x);
    });
    cv::Point2f tr = p[0], bl = p[3];

    return {tl, tr, br, bl};
}

/** 提取红色掩膜并做形态学去噪 */
cv::Mat redMaskOf(const cv::Mat &frame) {
    cv::Mat hsv, m1, m2, mask;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, kRedLower1, kRedUpper1, m1);
    cv::inRange(hsv, kRedLower2, kRedUpper2, m2);
    cv::bitwise_or(m1, m2, mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
    return mask;
}

/** 找候选灯条：轮廓 -> minAreaRect -> 按形状过滤 */
std::vector<LightBar> findLightBars(const cv::Mat &frame) {
    cv::Mat mask = redMaskOf(frame);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<LightBar> bars;
    for (const auto &cnt : contours) {
        if (cv::contourArea(cnt) < kMinArea) continue;

        cv::RotatedRect rect = cv::minAreaRect(cnt);

        // 统一成长边为 length，角度归一化
        float w = rect.size.width;
        float h = rect.size.height;
        double angle = rect.angle;
        if (w > h) {
            std::swap(w, h);
            angle += 90.0;
        }
        if (w <= 0 || h / w < kMinAspect) continue;

        double normAngle = std::fmod(angle + 45.0, 90.0) - 45.0;
        if (std::fabs(normAngle) > kMaxAngle) continue;

        LightBar bar;
        bar.center = rect.center;
        bar.angle = normAngle;
        bar.length = h;
        bar.width = w;
        cv::Point2f pts[4];
        rect.points(pts);
        bar.corners.assign(pts, pts + 4);

        // 画框：用 minAreaRect 量出的真实宽高，摆正（边水平/垂直），
        // 再每边外扩 kBoxPadding 像素，让框把灯条连同边缘光晕一起圈住。
        // 不用 boundingRect —— 那个会把整根斜灯条套进去，框会宽出一大截。
        const float kBoxPadding = 10.0f;
        bar.box = cv::Rect2f(rect.center.x - w / 2.0f - kBoxPadding,
                             rect.center.y - h / 2.0f - kBoxPadding, w + 2 * kBoxPadding,
                             h + 2 * kBoxPadding);
        bars.push_back(bar);
    }
    return bars;
}

/** 两两配对，返回 (左灯条, 右灯条) */
std::vector<std::pair<LightBar, LightBar>> pairBars(const std::vector<LightBar> &bars) {
    std::vector<std::pair<LightBar, LightBar>> pairs;
    for (size_t i = 0; i < bars.size(); ++i) {
        for (size_t j = i + 1; j < bars.size(); ++j) {
            const LightBar &a = bars[i];
            const LightBar &b = bars[j];

            if (std::fabs(a.angle - b.angle) > kMaxAngleDiff) continue;

            double la = a.length, lb = b.length;
            double maxL = std::max(la, lb), minL = std::max(std::min(la, lb), 1e-6);
            if (maxL / minL > kMaxHeightRatio) continue;

            double dx = std::fabs(a.center.x - b.center.x);
            double dy = std::fabs(a.center.y - b.center.y);
            double avgH = (la + lb) / 2.0;
            if (dx < 1e-6) continue;

            double gap = dx / avgH;
            if (gap < kMinGapRatio || gap > kMaxGapRatio) continue;
            if (dy > avgH * 0.6) continue;

            if (a.center.x < b.center.x) pairs.emplace_back(a, b);
            else pairs.emplace_back(b, a);
        }
    }
    return pairs;
}

/** 画灯条：摆正的黄色框（尺寸 = 灯条实际宽高）+ 四个角点 */
void drawBar(cv::Mat &frame, const LightBar &bar, const std::string &tag) {
    // 黄色框：边水平/垂直，尺寸取 minAreaRect 量出的真实宽高
    cv::Rect boxInt(cvRound(bar.box.x), cvRound(bar.box.y), cvRound(bar.box.width),
                    cvRound(bar.box.height));
    cv::rectangle(frame, boxInt, cv::Scalar(0, 255, 255), 2);

    // 四个角点就是正框的四个角：左上、右上、右下、左下
    std::vector<cv::Point2f> quad = {
        cv::Point2f(static_cast<float>(boxInt.x), static_cast<float>(boxInt.y)),
        cv::Point2f(static_cast<float>(boxInt.x + boxInt.width), static_cast<float>(boxInt.y)),
        cv::Point2f(static_cast<float>(boxInt.x + boxInt.width),
                    static_cast<float>(boxInt.y + boxInt.height)),
        cv::Point2f(static_cast<float>(boxInt.x), static_cast<float>(boxInt.y + boxInt.height))};

    cv::Point2f boxCenter(static_cast<float>(boxInt.x + boxInt.width / 2),
                          static_cast<float>(boxInt.y + boxInt.height / 2));

    for (size_t i = 0; i < quad.size(); ++i) {
        cv::circle(frame, quad[i], 8, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
        cv::circle(frame, quad[i], 5, cv::Scalar(0, 0, 255), -1, cv::LINE_AA);

        // 标签往框内推，避免和装甲板的 P1~P4（在框外角上）重叠
        cv::Point2f d = quad[i] - boxCenter;
        double len = std::sqrt(static_cast<double>(d.x) * d.x + static_cast<double>(d.y) * d.y);
        cv::Point2f off = (len > 1e-6)
                              ? cv::Point2f(static_cast<float>(-d.x / len * 18.0),
                                            static_cast<float>(-d.y / len * 18.0) + 5.0f)
                              : cv::Point2f(0.0f, 5.0f);
        cv::putText(frame, tag + std::to_string(i + 1), quad[i] + off,
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    }
}

/** 画装甲板：绿色四边形 + P1~P4 角点标注（用两个灯条正矩形的外侧四角连起来） */
void drawArmor(cv::Mat &frame, const LightBar &left, const LightBar &right) {
    // 左灯条取左边两角，右灯条取右边两角，围住整个装甲板
    cv::Point2f lTL(static_cast<float>(left.box.x), static_cast<float>(left.box.y));
    cv::Point2f lBL(static_cast<float>(left.box.x),
                    static_cast<float>(left.box.y + left.box.height));
    cv::Point2f rTR(static_cast<float>(right.box.x + right.box.width),
                    static_cast<float>(right.box.y));
    cv::Point2f rBR(static_cast<float>(right.box.x + right.box.width),
                    static_cast<float>(right.box.y + right.box.height));

    // 装甲板四点：左上 -> 右上 -> 右下 -> 左下
    std::vector<cv::Point2f> armor = {lTL, rTR, rBR, lBL};

    std::vector<cv::Point> poly;
    for (const auto &p : armor) poly.emplace_back(cvRound(p.x), cvRound(p.y));
    cv::polylines(frame, poly, true, cv::Scalar(0, 255, 0), 2);      // 绿

    for (size_t i = 0; i < armor.size(); ++i) {
        cv::circle(frame, armor[i], 5, cv::Scalar(255, 0, 255), -1); // 品红

        // 标签往四边形外侧偏移，避免和灯条角点重叠
        cv::Point2f c((armor[0].x + armor[1].x + armor[2].x + armor[3].x) / 4.0f,
                      (armor[0].y + armor[1].y + armor[2].y + armor[3].y) / 4.0f);
        cv::Point2f d = armor[i] - c;
        double len = std::sqrt(static_cast<double>(d.x) * d.x + static_cast<double>(d.y) * d.y);
        cv::Point2f off = (len > 1e-6)
                              ? cv::Point2f(static_cast<float>(d.x / len * 20.0),
                                            static_cast<float>(d.y / len * 20.0))
                              : cv::Point2f(0, -20);
        cv::putText(frame, "P" + std::to_string(i + 1),
                    armor[i] + off, cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    }
}

int main(int argc, char **argv) {
    // ------------------------------------------------------ 命令行参数
    std::string videoPath = "test_clip.mp4";
    std::string savePath = "detect_result.mp4";
    bool show = false;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--show") {
            show = true;
        } else if (a == "-h" || a == "--help") {
            std::cout << "用法: " << argv[0] << " [输入视频] [输出视频] [--show]\n"
                      << "  --show   弹出窗口实时显示处理过程（空格暂停，q/ESC 退出）\n";
            return 0;
        } else if (!a.empty() && a[0] != '-') {
            if (videoPath == "test_clip.mp4") videoPath = a;
            else savePath = a;
        }
    }

    // ------------------------------------------------------ 打开视频
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "[错误] 打不开视频：" << videoPath << "\n"
                  << "       请确认视频在工程目录里，并且当前工作目录是该工程目录。\n";
        return -1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 30.0;
    int w = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int total = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    std::cout << "[信息] 视频 " << w << "x" << h << " @ " << fps << "fps，共 " << total << " 帧\n";

    // ------------------------------------------------------ 输出视频
    cv::VideoWriter writer(savePath, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
                           cv::Size(w, h));
    if (!writer.isOpened()) {
        std::cerr << "[警告] 打不开输出视频写入器，结果不会保存：" << savePath << "\n";
    }

    cv::Point2f frameCenter(static_cast<float>(w) / 2.0f, static_cast<float>(h) / 2.0f);
    int frameIdx = 0;
    int framesWithArmor = 0;

    // ------------------------------------------------------ 逐帧处理
    cv::Mat frame;
    while (cap.read(frame)) {
        ++frameIdx;

        auto bars = findLightBars(frame);
        auto pairs = pairBars(bars);

        // 只画配对成功的灯条。未配对的候选（比如键盘上的红色键帽）一律不画，
        // 否则画面上会冒出和装甲板无关的小黄框。
        if (!pairs.empty()) {
            // 画面里有多块装甲板时，取离画面中心最近的一对
            size_t best = 0;
            double bestDist = 1e18;
            for (size_t i = 0; i < pairs.size(); ++i) {
                cv::Point2f mid = (pairs[i].first.center + pairs[i].second.center) * 0.5f;
                double d = cv::norm(mid - frameCenter);
                if (d < bestDist) {
                    bestDist = d;
                    best = i;
                }
            }
            drawBar(frame, pairs[best].first, "L");
            drawBar(frame, pairs[best].second, "R");
            drawArmor(frame, pairs[best].first, pairs[best].second);
            ++framesWithArmor;
        }

        // 左上角状态信息：cand = 检测到的候选灯条，armor = 配对成功的装甲板
        cv::putText(frame,
                    "frame " + std::to_string(frameIdx) + "/" + std::to_string(total) +
                        "   cand=" + std::to_string(bars.size()) +
                        "   armor=" + std::to_string(pairs.size()),
                    cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2,
                    cv::LINE_AA);

        if (writer.isOpened()) writer.write(frame);

        if (show) {
            cv::imshow("detect_demo", frame);
            int key = cv::waitKey(1) & 0xFF;
            if (key == 27 || key == 'q') break;
            if (key == ' ') cv::waitKey(0);
        }
    }

    cap.release();
    if (writer.isOpened()) writer.release();
    cv::destroyAllWindows();

    std::cout << "[统计] 共 " << frameIdx << " 帧，其中 " << framesWithArmor << " 帧识别到装甲板"
              << "（" << (frameIdx > 0 ? 100.0 * framesWithArmor / frameIdx : 0.0) << "%）\n";
    std::cout << "[成功] 结果视频已保存：" << savePath << "\n";
    return 0;
}
