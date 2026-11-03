/**
 * 培训3 · 任务1：HSV 通道分离 + 红色掩膜
 *
 * 流程：
 *   1. 读取装甲板图片（默认 armor.jpg，可用命令行传路径）
 *   2. 转成 HSV
 *   3. 分离 H / S / V 三个通道，分别弹出三个窗口展示
 *   4. 用 inRange 提取红色装甲的二值掩膜
 *   5. 保存为 red_mask.png
 *
 * 编译：
 *   cmake -S . -B build && cmake --build build -j
 *
 * 运行：
 *   ./build/hsv_demo                # 处理并保存掩膜，不弹窗
 *   ./build/hsv_demo --show         # 弹窗看效果
 *   ./build/hsv_demo 别的图.jpg --show
 */

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------- 阈值参数
// 红色在 HSV 里跨 0 度（色相环首尾相连），所以要两段区间取并集。
// OpenCV 的 H 范围是 0~179（不是 0~360）。
const cv::Scalar kRedLower1(0, 100, 60);    // 0 度附近：红到橙红
const cv::Scalar kRedUpper1(10, 255, 255);
const cv::Scalar kRedLower2(160, 100, 60);  // 180 度附近：紫红到红
const cv::Scalar kRedUpper2(179, 255, 255);

// ---------------------------------------------------------------- 工具函数
/** H 通道最大只有 179，不拉伸的话整张图几乎全黑，看不出东西 */
cv::Mat normalizeChannel(const cv::Mat &ch) {
    cv::Mat out;
    cv::normalize(ch, out, 0, 255, cv::NORM_MINMAX);
    return out;
}

int main(int argc, char **argv) {
    // ------------------------------------------------------ 命令行参数
    std::string imagePath = "armor.jpg";   // 默认用工程目录里的图
    bool show = false;
    bool record = false;                   // 录屏模式：固定窗口大小和位置，方便录演示视频
    std::string outPath = "red_mask.png";

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--show") {
            show = true;
        } else if (a == "--record") {
            show = true;
            record = true;
        } else if (a == "-h" || a == "--help") {
            std::cout << "用法: " << argv[0] << " [图片路径] [--show | --record]\n"
                      << "  --show     弹出窗口显示 H / S / V 三个通道和红色掩膜\n"
                      << "  --record   同 --show，但把 5 个窗口缩排成 2 行，方便录演示视频\n";
            return 0;
        } else if (!a.empty() && a[0] != '-') {
            imagePath = a;
        }
    }

    // ------------------------------------------------------ 1. 读图
    cv::Mat img = cv::imread(imagePath);
    if (img.empty()) {
        std::cerr << "[错误] 读不到图片：" << imagePath << "\n"
                  << "       请确认图片在工程目录里，并且当前工作目录是该工程目录。\n";
        return -1;
    }
    std::cout << "[信息] 图片尺寸：" << img.cols << " x " << img.rows << "\n";

    // ------------------------------------------------------ 2. 转 HSV
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // ------------------------------------------------------ 3. 分离 H / S / V
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);   // channels[0]=H, [1]=S, [2]=V

    // 录屏模式：先把图缩小再显示，窗口大小就由图像决定，不依赖 resizeWindow
    auto showAt = [&](const std::string &name, const cv::Mat &imgToShow, int x, int y) {
        if (record) {
            cv::Mat small;
            cv::resize(imgToShow, small, cv::Size(400, 225), 0, 0, cv::INTER_AREA);
            cv::imshow(name, small);
            cv::moveWindow(name, x, y);
        } else {
            cv::imshow(name, imgToShow);
        }
    };

    if (show) {
        cv::Mat hView = normalizeChannel(channels[0]);
        showAt("H channel", hView, 0, 20);
        showAt("S channel", channels[1], 410, 20);
        showAt("V channel", channels[2], 820, 20);
    }

    // ------------------------------------------------------ 4. inRange 提红色
    cv::Mat mask1, mask2, redMask;
    cv::inRange(hsv, kRedLower1, kRedUpper1, mask1);
    cv::inRange(hsv, kRedLower2, kRedUpper2, mask2);
    cv::bitwise_or(mask1, mask2, redMask);

    // 形态学去噪：开运算去小噪点，闭运算补灯条内部空洞
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(redMask, redMask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(redMask, redMask, cv::MORPH_CLOSE, kernel);

    if (show) {
        showAt("red_mask", redMask, 0, 280);

        // 叠加预览：掩膜和原图取与，直观看阈值准不准
        cv::Mat overlay;
        cv::bitwise_and(img, img, overlay, redMask);
        showAt("red overlay", overlay, 410, 280);

        cv::waitKey(30);   // 让窗口管理器把布局落实
    }

    // ------------------------------------------------------ 5. 保存掩膜
    bool ok = cv::imwrite(outPath, redMask);
    if (ok) {
        std::cout << "[成功] 掩膜已保存：" << outPath << "\n";
    } else {
        std::cerr << "[失败] 掩膜保存失败：" << outPath << "\n";
        return -1;
    }

    // 统计前景像素，确认掩膜不是全黑
    int whitePixels = cv::countNonZero(redMask);
    double ratio = 100.0 * whitePixels / static_cast<double>(redMask.total());
    std::cout << "[统计] 红色像素 " << whitePixels << " 个，占比 " << ratio << "%\n";

    if (show) {
        std::cout << "[提示] 按任意键关闭所有窗口\n";
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
    return 0;
}
