#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// ==================== 确保输出目录存在 ====================
static void ensure_dir(const std::string &dir)
{
    cv::utils::fs::createDirectory(dir);              // 已存在则不报错
}

// ==================== 批量：单通道 → 三通道 ====================
static std::vector<cv::Mat> to_bgr_all(const std::vector<cv::Mat> &img)   // 输入叫 img
{
    std::vector<cv::Mat> out;
    out.reserve(img.size());
    for (const auto &m : img)                         // 遍历输入 img（原来错写成 out）
    {
        if (m.channels() == 1)                        // 单通道才转
        {
            cv::Mat bgr;
            cv::cvtColor(m, bgr, cv::COLOR_GRAY2BGR);
            out.push_back(bgr);
        }
        else                                          // 已是三通道直接放
        {
            out.push_back(m);                         // push_back，不是 push_backward
        }
    }
    return out;
}

// ==================== 给单张图片加标题 ====================
static cv::Mat with_title(const cv::Mat &img, const std::string &title)
{
    cv::Mat color;
    if (img.channels() == 1)
        cv::cvtColor(img, color, cv::COLOR_GRAY2BGR);
    else
        color = img.clone();

    cv::Mat canvas(color.rows + 40, color.cols, CV_8UC3, cv::Scalar(32, 32, 32)); // colr → color
    color.copyTo(canvas(cv::Rect(0, 40, color.cols, color.rows)));
    cv::putText(canvas, title, cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    return canvas;
}

// ==================== 多图三列铺平 ====================
static cv::Mat flatten(const std::vector<std::pair<std::string, cv::Mat>> &imgs)
{
    const int cols = 3;
    const int w = 430;
    const int n = static_cast<int>(imgs.size());
    const int rows = (n + cols - 1) / cols;

    std::vector<cv::Mat> tiles;
    for (const auto &[title, img] : imgs)
    {
        if (img.empty())
        {
            std::cout << "Image is empty: " << title << std::endl;
            continue;
        }
        int h = static_cast<int>(img.rows * 1.0 / img.cols * w);
        h = std::max(h, 1);
        cv::Mat resized;
        cv::resize(img, resized, cv::Size(w, h));
        tiles.push_back(with_title(resized, title));
    }

    if (tiles.empty())
        return cv::Mat();

    int tileH = 0;
    for (const auto &t : tiles) tileH = std::max(tileH, t.rows);
    int tileW = tiles[0].cols;

    std::vector<cv::Mat> row_imgs;
    for (int r = 0; r < rows; ++r)
    {
        std::vector<cv::Mat> row;
        int start = r * cols;
        int end   = std::min(start + cols, static_cast<int>(tiles.size()));
        for (int i = start; i < end; ++i)
        {
            cv::Mat t = tiles[i];
            if (t.rows < tileH)
            {
                cv::Mat padded(tileH, t.cols, CV_8UC3, cv::Scalar(32, 32, 32));
                t.copyTo(padded(cv::Rect(0, 0, t.cols, t.rows)));
                t = padded;
            }
            row.push_back(t);
        }
        while (static_cast<int>(row.size()) < cols)
            row.push_back(cv::Mat(tileH, tileW, CV_8UC3, cv::Scalar(32, 32, 32)));

        cv::Mat row_img;
        cv::hconcat(row, row_img);
        row_imgs.push_back(row_img);
    }

    cv::Mat montage;
    cv::vconcat(row_imgs, montage);
    return montage;
}

// ==================== 主函数 ====================
int main()
{
    const std::string outDir = "output";
    ensure_dir(outDir);

    std::string imagepath = "../armor.jpg";           // 双引号
    cv::Mat src = cv::imread(imagepath);
    if (src.empty())
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    // ---------- 1. HSV 分离 ----------
    cv::Mat imageHSV;
    cv::cvtColor(src, imageHSV, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> channels;
    cv::split(imageHSV, channels);                    // H, S, V 单通道
    cv::Mat H_channels = channels[0];
    cv::Mat S_channels = channels[1];
    cv::Mat V_channels = channels[2];

    // ---------- 2. 红色掩膜 + 开运算 ----------
    cv::Mat mask1, mask2, mask;
    cv::inRange(imageHSV, cv::Scalar(0, 100, 70),   cv::Scalar(10, 255, 255), mask1);  // cv::inRange
    cv::inRange(imageHSV, cv::Scalar(170, 100, 70), cv::Scalar(180, 255, 255), mask2);
    cv::bitwise_or(mask1, mask2, mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat mask_open;
    cv::morphologyEx(mask, mask_open, cv::MORPH_OPEN, kernel);

    // ---------- 3. 叠加图 ----------
    cv::Mat overlay = src.clone();
    overlay.setTo(cv::Scalar(0, 255, 0), mask_open);

    // ---------- 4. 保存到 output/ ----------
    std::vector<std::pair<std::string, cv::Mat>> toSave = {
        {"original.png",   src},
        {"H_channel.png",  H_channels},
        {"S_channel.png",  S_channels},
        {"V_channel.png",  V_channels},
        {"mask_raw.png",   mask},
        {"mask_open.png",  mask_open},                // 补上开运算掩膜
        {"overlay.png",    overlay},                  // 双引号
    };
    for (const auto &[name, mat] : toSave)
        cv::imwrite(outDir + "/" + name, mat);

    // ---------- 5. 批量转三通道（含 mask_open）----------
    std::vector<cv::Mat> singleCH = {H_channels, S_channels, V_channels, mask_open}; // 加 mask_open
    std::vector<cv::Mat> threeCH  = to_bgr_all(singleCH);   // 统一命名 threeCH

    // ---------- 6. 拼贴 6 张（2 行 × 3 列）----------
    std::vector<std::pair<std::string, cv::Mat>> imgs = {
        {"Original",  src},
        {"H channel", threeCH[0]},                    // 用 threeCH（大写）
        {"S channel", threeCH[1]},
        {"V channel", threeCH[2]},
        {"Mask",      threeCH[3]},                    // threeCH[3] = mask_open 的三通道版
        {"Overlay",   overlay},
    };
    cv::Mat montage = flatten(imgs);
    if (!montage.empty())
    {
        cv::imwrite(outDir + "/montage.png", montage);
        cv::imshow("montage", montage);
    }

    cv::waitKey(0);
    return 0;
}