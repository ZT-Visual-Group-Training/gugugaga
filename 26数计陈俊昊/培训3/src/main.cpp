// main.cpp —— 入口：任务1（静态图）+ 任务2（视频）
#include "armor.hpp"

#include <iostream>
#include <string>
#include <cstdlib>

// 没有图形界面时（后台跑）自动跳过所有 imshow
static bool hasGui()
{
    const char* d = std::getenv("DISPLAY");
    return d && *d;
}

// ---------- 任务1：装甲板图片 ----------
static void TaskImage(const std::string& path)
{
    cv::Mat img = cv::imread(path);
    if (img.empty())
    {
        std::cout << "读图失败: " << path << std::endl;
        return;
    }

    cv::Mat mask = Armor::makeMask(img);

    // 拆 H/S/V 三个通道，方便检查阈值
    cv::Mat hsv, ch[3];
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
    cv::split(hsv, ch);

    cv::Mat out = Armor::processFrame(img);

    if (hasGui())
    {
        cv::imshow("H", ch[0]);
        cv::imshow("S", ch[1]);
        cv::imshow("V", ch[2]);
        cv::imshow("mask", mask);
        cv::imshow("result", out);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    bool ok = cv::imwrite("red_mask.png", mask);
    std::cout << "red_mask.png " << (ok ? "保存成功" : "保存失败") << std::endl;
}

// ---------- 任务2：视频 ----------
static void TaskVideo(const std::string& videoPath, const std::string& outPath)
{
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        std::cout << "视频打不开: " << videoPath << std::endl;
        return;
    }

    int w = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int h = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(cv::CAP_PROP_FPS);
    std::cout << "视频: " << w << "x" << h << "  " << fps << "fps  "
              << cap.get(cv::CAP_PROP_FRAME_COUNT) << "帧" << std::endl;

    // 输出尺寸/帧率必须和输入完全一致，否则写不出文件且不报错
    cv::VideoWriter vw(outPath, cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                       fps, cv::Size(w, h));
    if (!vw.isOpened())
    {
        std::cout << "VideoWriter 打开失败: " << outPath << std::endl;
        return;
    }

    cv::Mat frame;
    int n = 0;
    bool gui = hasGui();

    while (cap.read(frame))
    {
        n++;
        cv::Mat out = Armor::processFrame(frame);
        vw.write(out);

        if (gui)
        {
            // 虚拟机里 imshow 是软件渲染，1280 宽直接显示会一顿一顿，
            // 缩到一半再显示流畅很多（只影响窗口，不影响 result.mp4）
            cv::Mat small;
            cv::resize(out, small, cv::Size(640, 360));
            cv::imshow("result", small);

            // 30ms ≈ 33fps，接近原视频的 30fps。
            // 之前用 1ms 会跑得飞快，看着像快进。
            if (cv::waitKey(30) == 27) break;
        }
    }

    vw.release();
    cap.release();
    if (gui) cv::destroyAllWindows();

    std::cout << "共 " << n << " 帧，输出: " << outPath << std::endl;
}

int main(int argc, char** argv)
{
    std::string imgPath   = "assets/img2.png";
    std::string videoPath = "assets/114514.mp4";
    std::string outPath   = "result.mp4";

    if (argc >= 2) videoPath = argv[1];
    if (argc >= 3) outPath   = argv[2];

    TaskImage(imgPath);
    TaskVideo(videoPath, outPath);

    return 0;
}
