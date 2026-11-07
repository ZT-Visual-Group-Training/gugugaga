
#include <opencv2/opencv.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

using namespace cv;
using namespace std;


static const int H1_LO = 0,   H1_HI = 10;   
static const int H2_LO = 160, H2_HI = 180; 
static const int S_LO  = 120;                
static const int V_LO  = 70;           


static void makeDir(const string& dir) {
#ifdef _WIN32
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0755);
#endif
}

static string joinPath(const string& dir, const string& name) {
    if (dir.empty()) return name;
    const char last = dir[dir.size() - 1];
    if (last == '/' || last == '\\') return dir + name;
    return dir + "/" + name;
}

static void putTextOutlined(Mat& img, const string& text, Point org,
                            double scale, const Scalar& color, int thickness = 1) {
    putText(img, text, org, FONT_HERSHEY_SIMPLEX, scale, Scalar(0, 0, 0),
            thickness + 2, LINE_AA);
    putText(img, text, org, FONT_HERSHEY_SIMPLEX, scale, color,
            thickness, LINE_AA);
}


static Mat makeChannelPanel(const Mat& gray, const string& title,
                            const string& subtitle) {
    Mat vis;
    cvtColor(gray, vis, COLOR_GRAY2BGR);

    const int top = 58;
    Mat canvas;
    copyMakeBorder(vis, canvas, top, 0, 0, 0, BORDER_CONSTANT, Scalar(28, 28, 28));

    putText(canvas, title, Point(14, 24), FONT_HERSHEY_SIMPLEX, 0.62,
            Scalar(0, 255, 255), 1, LINE_AA);
    putText(canvas, subtitle, Point(14, 48), FONT_HERSHEY_SIMPLEX, 0.45,
            Scalar(200, 200, 200), 1, LINE_AA);
    return canvas;
}

static bool task1Image(const string& imagePath, const string& outDir, bool gui) {
    cout << "\n========== 任务 1：HSV 通道分离 + 红色掩膜 ==========\n";

    Mat img = imread(imagePath);
    if (img.empty()) {
        cerr << "[错误] 无法读取图片: " << imagePath << "\n"
             << "       请确认图片和 exe 在同一目录，或把图片路径作为参数传入。\n";
        return false;
    }

    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    vector<Mat> channels;
    split(hsv, channels);   // channels[0]=H, [1]=S, [2]=V

    imwrite(joinPath(outDir, "H_channel.png"), channels[0]);
    imwrite(joinPath(outDir, "S_channel.png"), channels[1]);
    imwrite(joinPath(outDir, "V_channel.png"), channels[2]);

    // 红色掩膜：两段 H 分别 inRange，再合并
    Mat mask1, mask2, redMask;
    inRange(hsv, Scalar(H1_LO, S_LO, V_LO), Scalar(H1_HI, 255, 255), mask1);
    inRange(hsv, Scalar(H2_LO, S_LO, V_LO), Scalar(H2_HI, 255, 255), mask2);
    bitwise_or(mask1, mask2, redMask);

    // 去噪：先开运算去掉小亮点，再闭运算填补灯条内部的小孔
    const Mat kOpen  = getStructuringElement(MORPH_RECT, Size(3, 3));
    const Mat kClose = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(redMask, redMask, MORPH_OPEN,  kOpen);
    morphologyEx(redMask, redMask, MORPH_CLOSE, kClose);

    imwrite(joinPath(outDir, "red_mask.png"), redMask);

    // 掩膜叠加图，方便检查阈值效果
    Mat overlay = img.clone();
    Mat red(img.size(), CV_8UC3, Scalar(0, 0, 255));
    red.copyTo(overlay, redMask);
    addWeighted(img, 0.60, overlay, 0.40, 0, overlay);
    char thresholdText[192];
    snprintf(thresholdText, sizeof(thresholdText),
             "red mask: H[%d,%d] U [%d,%d]  S[%d,255]  V[%d,255]",
             H1_LO, H1_HI, H2_LO, H2_HI, S_LO, V_LO);
    putTextOutlined(overlay, thresholdText, Point(14, 30), 0.55,
                    Scalar(0, 255, 255), 1);
    imwrite(joinPath(outDir, "red_mask_overlay.png"), overlay);

    // 三通道拼成一张“照片”，H 通道仅显示时归一化
    Mat hVis;
    double hMin = 0, hMax = 0;
    minMaxLoc(channels[0], &hMin, &hMax);
    if (hMax > hMin) {
        channels[0].convertTo(hVis, CV_8U, 255.0 / (hMax - hMin), -hMin * 255.0 / (hMax - hMin));
    } else {
        hVis = Mat::zeros(channels[0].size(), CV_8U);
    }

    char hInfo[96], sInfo[96], vInfo[96], maskInfo[96];
    snprintf(hInfo, sizeof(hInfo), "raw H range: %.0f ~ %.0f (normalized for display)", hMin, hMax);
    snprintf(sInfo, sizeof(sInfo), "S range: 0 ~ 255");
    snprintf(vInfo, sizeof(vInfo), "V range: 0 ~ 255");
    snprintf(maskInfo, sizeof(maskInfo), "red pixels: %d", countNonZero(redMask));

    vector<Mat> panels;
    panels.push_back(makeChannelPanel(hVis, "H channel", hInfo));
    panels.push_back(makeChannelPanel(channels[1], "S channel", sInfo));
    panels.push_back(makeChannelPanel(channels[2], "V channel", vInfo));
    Mat panel;
    hconcat(panels, panel);
    imwrite(joinPath(outDir, "hsv_channels.png"), panel);

    cout << "  图片尺寸: " << img.cols << " x " << img.rows << "\n"
         << "  红色阈值: H [" << H1_LO << "," << H1_HI << "] U ["
         << H2_LO << "," << H2_HI << "], S [" << S_LO << ",255], V ["
         << V_LO << ",255]\n"
         << "  红色像素: " << countNonZero(redMask) << " (" << maskInfo << ")\n"
         << "  已保存: H/S/V_channel.png, hsv_channels.png, red_mask.png, red_mask_overlay.png\n";

    if (gui) {
        imshow("1 - H channel (normalized)", hVis);
        imshow("2 - S channel", channels[1]);
        imshow("3 - V channel", channels[2]);
        imshow("4 - Red mask (inRange)", redMask);
        cout << "  按任意键继续任务 2 ...\n";
        waitKey(0);
        destroyAllWindows();
    }
    return true;
}

// ---------------- 任务 2 ----------------
struct LightBar {
    Rect box;           // 正的（轴对齐）外接矩形，用于画黄色框
    RotatedRect rect;   // 最小外接矩形，用于筛选和取外侧角点
    Point2f pts[4];     // rect 的四个顶点
    float w = 0.f;      // 短边
    float h = 0.f;      // 长边
    float cx = 0.f;
    float cy = 0.f;
    double area = 0.0;
};

struct ArmorDetection {
    bool found = false;
    LightBar left;
    LightBar right;
    Point2f corners[4];   // P1 左上、P2 右上、P3 右下、P4 左下
    Point2f center;       // 装甲板中心
    double score = 0.0;
};

static float longAxisAngle(const LightBar& b) {
    const Point2f e1 = b.pts[1] - b.pts[0];
    const Point2f e2 = b.pts[2] - b.pts[1];
    const Point2f v = (norm(e1) > norm(e2)) ? e1 : e2;
    float a = (float)(atan2(v.y, v.x) * 180.0 / CV_PI);
    if (a < 0.f) a += 180.f;
    if (a >= 180.f) a -= 180.f;
    return a;
}

static void yRange(const LightBar& b, float& top, float& bottom) {
    top = (float)b.box.y;
    bottom = (float)(b.box.y + b.box.height);
}

static vector<LightBar> findLightBars(const Mat& mask, const Size& frameSize) {
    vector<vector<Point> > contours;
    Mat tmp = mask.clone();
    findContours(tmp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<LightBar> bars;
    for (size_t i = 0; i < contours.size(); ++i) {
        const double area = contourArea(contours[i]);
        if (area < 120.0) continue;                       // 太小，可能是噪点

        RotatedRect rr = minAreaRect(contours[i]);
        float w = rr.size.width;
        float h = rr.size.height;
        if (w > h) swap(w, h);

        if (w < 6.f || h < 28.f) continue;                // 太细/太短
        if (w > frameSize.width  * 0.20f) continue;       // 太宽，不是灯条
        if (h > frameSize.height * 0.85f) continue;       // 太高，不是灯条

        const float ratio = h / (std::max)(1.0f, w);      // 长宽比
        if (ratio < 1.7f || ratio > 12.0f) continue;

        const double fill = area / (double)(w * h);       // 填充率
        if (fill < 0.35) continue;                         // 太稀疏，像反光/噪声

        LightBar b;
        b.rect = rr;
        rr.points(b.pts);
        b.box = boundingRect(contours[i]);
        b.w = w;
        b.h = h;
        b.cx = b.box.x + b.box.width * 0.5f;
        b.cy = b.box.y + b.box.height * 0.5f;
        b.area = area;
        bars.push_back(b);
    }
    return bars;
}

struct PairChoice {
    int left = -1;
    int right = -1;
    double score = -1.0;
};

// 在候选灯条中选“左右一对”：高度接近、中心高度接近、有足够间距和重叠
static bool selectPair(const vector<LightBar>& bars, PairChoice& best) {
    bool found = false;
    for (size_t i = 0; i < bars.size(); ++i) {
        for (size_t j = i + 1; j < bars.size(); ++j) {
            const LightBar& a = bars[i];
            const LightBar& b = bars[j];

            const int li = (a.cx <= b.cx) ? (int)i : (int)j;
            const int ri = (a.cx <= b.cx) ? (int)j : (int)i;
            const LightBar& L = bars[li];
            const LightBar& R = bars[ri];

            const float hMax = (std::max)(L.h, R.h);
            const float hMin = (std::min)(L.h, R.h);
            if (hMax / hMin > 1.8f) continue;             // 高度差太大

            const float dy = fabs(L.cy - R.cy);
            if (dy > 0.45f * hMax) continue;              // 上下错位太多

            const float dx = R.cx - L.cx;
            if (dx < 0.8f * hMax || dx > 4.5f * hMax) continue;  // 间距不合理

            float lTop, lBottom, rTop, rBottom;
            yRange(L, lTop, lBottom);
            yRange(R, rTop, rBottom);
            const float overlap = (std::min)(lBottom, rBottom) -
                                  (std::max)(lTop, rTop);
            if (overlap < 0.55f * hMin) continue;          // 竖直方向重叠太少

            float dAng = fabs(longAxisAngle(L) - longAxisAngle(R));
            if (dAng > 90.f) dAng = 180.f - dAng;
            if (dAng > 25.f) continue;                    // 倾斜方向差太多

            double score = L.area + R.area;
            score *= (1.0 - 0.50 * dy / hMax);
            score *= (1.0 - 0.40 * fabs(hMax / hMin - 1.0));
            score *= (1.0 - 0.30 * dAng / 25.0);

            if (score > best.score) {
                best.left = li;
                best.right = ri;
                best.score = score;
                found = true;
            }
        }
    }
    return found;
}

static void outerEdgeCorners(const LightBar& bar, const Point2f& u, bool leftSide,
                             Point2f& top, Point2f& bottom) {
    const Point2f* p = bar.pts;

    // minAreaRect 的四条边两长两短，先找出两条“长边”
    const float l01 = (float)norm(p[1] - p[0]);
    const float l12 = (float)norm(p[2] - p[1]);
    int a, b, c, d;
    if (l01 >= l12) {
        a = 0; b = 1; c = 2; d = 3;   // 长边: (0,1) 和 (2,3)
    } else {
        a = 1; b = 2; c = 3; d = 0;   // 长边: (1,2) 和 (3,0)
    }

    const Point2f m1 = (p[a] + p[b]) * 0.5f;
    const Point2f m2 = (p[c] + p[d]) * 0.5f;
    const float proj1 = m1.x * u.x + m1.y * u.y;
    const float proj2 = m2.x * u.x + m2.y * u.y;

    // 左灯条选更靠左的长边，右灯条选更靠右的长边
    const bool useFirst = leftSide ? (proj1 < proj2) : (proj1 > proj2);
    Point2f e0 = useFirst ? p[a] : p[c];
    Point2f e1 = useFirst ? p[b] : p[d];
    if (e0.y > e1.y) swap(e0, e1);
    top = e0;
    bottom = e1;
}

static ArmorDetection detectArmor(const Mat& frame, const Mat& mask) {
    ArmorDetection det;
    const vector<LightBar> bars = findLightBars(mask, frame.size());

    PairChoice pair;
    if (!selectPair(bars, pair)) return det;

    det.left = bars[pair.left];
    det.right = bars[pair.right];
    det.score = pair.score;

    // 从左灯条指向右灯条的单位向量，用来判断“外侧”
    const Point2f cL(det.left.cx, det.left.cy);
    const Point2f cR(det.right.cx, det.right.cy);
    const Point2f d = cR - cL;
    const float n = (float)norm(d);
    if (n < 1.0f) return det;
    const Point2f u = d / n;

    Point2f lTop, lBottom, rTop, rBottom;
    outerEdgeCorners(det.left,  u, true,  lTop, lBottom);
    outerEdgeCorners(det.right, u, false, rTop, rBottom);

    // 装甲板四个角点：左灯条外侧上/下 + 右灯条外侧上/下
    det.corners[0] = lTop;       // P1 左上
    det.corners[1] = rTop;       // P2 右上
    det.corners[2] = rBottom;    // P3 右下
    det.corners[3] = lBottom;    // P4 左下
    det.center = (det.corners[0] + det.corners[1] + det.corners[2] + det.corners[3]) * 0.25f;
    det.found = true;
    return det;
}

static void putCornerLabel(Mat& frame, const char* text, Point p, int dx, int dy) {
    Point org(p.x + dx, p.y + dy);
    org.x = (std::max)(4, (std::min)(frame.cols - 42, org.x));
    org.y = (std::max)(16, (std::min)(frame.rows - 8, org.y));
    putText(frame, text, org, FONT_HERSHEY_SIMPLEX, 0.5,
            Scalar(0, 255, 0), 1, LINE_AA);
}

static void drawDetection(Mat& frame, const ArmorDetection& det) {
    if (!det.found) {
        putTextOutlined(frame, "No armor detected", Point(24, frame.rows - 26),
                        0.65, Scalar(0, 0, 255), 1);
        return;
    }

    // 1) 左右灯条：黄色轴对齐外接矩形
    rectangle(frame, det.left.box, Scalar(0, 255, 255), 2, LINE_AA);
    rectangle(frame, det.right.box, Scalar(0, 255, 255), 2, LINE_AA);

    // 2) 四个角点连成装甲板外接四边形：绿色
    vector<Point> quad;
    for (int k = 0; k < 4; ++k)
        quad.push_back(Point(cvRound(det.corners[k].x), cvRound(det.corners[k].y)));
    polylines(frame, quad, true, Scalar(0, 255, 0), 2, LINE_AA);

    // 3) 四个角点：绿色圆点 + P1~P4 标签
    const char* names[4] = { "P1", "P2", "P3", "P4" };
    const int dx[4] = { -34, 8, 8, -34 };
    const int dy[4] = { -8, -8, 20, 20 };
    for (int k = 0; k < 4; ++k) {
        circle(frame, quad[k], 6, Scalar(0, 255, 0), 2, LINE_AA);
        circle(frame, quad[k], 2, Scalar(0, 255, 0), -1, LINE_AA);
        putCornerLabel(frame, names[k], quad[k], dx[k], dy[k]);
    }

    // 4) 装甲板中心十字
    drawMarker(frame, Point(cvRound(det.center.x), cvRound(det.center.y)),
               Scalar(0, 255, 0), MARKER_CROSS, 18, 1, LINE_AA);
}

static bool openVideo(const string& path, VideoCapture& cap) {
    cap.open(path);
    if (cap.isOpened()) return true;
    cap.open(path, CAP_FFMPEG);
    if (cap.isOpened()) return true;
    cap.open(path, CAP_DSHOW);
    return cap.isOpened();
}

static bool task2Video(const string& videoPath, const string& outDir, bool gui) {
    cout << "\n========== 任务 2：视频灯条 + 四角点检测 ==========\n";

    VideoCapture cap;
    if (!openVideo(videoPath, cap)) {
        cerr << "[错误] 无法打开视频: " << videoPath << "\n";
        return false;
    }

    const double fps = cap.get(CAP_PROP_FPS) > 1.0 ? cap.get(CAP_PROP_FPS) : 30.0;
    const int total = (int)cap.get(CAP_PROP_FRAME_COUNT);
    const int frameW = (int)cap.get(CAP_PROP_FRAME_WIDTH);
    const int frameH = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
    cout << "  视频: " << frameW << " x " << frameH << ", " << fps << " fps, "
         << total << " 帧\n";

    VideoWriter writer(joinPath(outDir, "armor_detected.mp4"),
                       VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
                       Size(frameW, frameH));
    if (!writer.isOpened()) {
        cerr << "  [警告] 无法创建输出视频，将只保存关键帧图片。\n";
    }

    set<int> keyFrames;
    if (total > 10) {
        keyFrames.insert(0);
        keyFrames.insert(total * 30 / 100);
        keyFrames.insert(total * 50 / 100);
        keyFrames.insert(total * 70 / 100);
        keyFrames.insert(total * 99 / 100);
    }

    Mat frame;
    Mat bestFrame;
    double bestScore = -1.0;
    int index = 0;
    int foundCount = 0;

    while (cap.read(frame)) {
        Mat hsv, m1, m2, mask;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(H1_LO, S_LO, V_LO), Scalar(H1_HI, 255, 255), m1);
        inRange(hsv, Scalar(H2_LO, S_LO, V_LO), Scalar(H2_HI, 255, 255), m2);
        bitwise_or(m1, m2, mask);

        const Mat kOpen  = getStructuringElement(MORPH_RECT, Size(3, 3));
        const Mat kClose = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(mask, mask, MORPH_OPEN,  kOpen);
        morphologyEx(mask, mask, MORPH_CLOSE, kClose);

        const ArmorDetection det = detectArmor(frame, mask);
        if (det.found) ++foundCount;

        drawDetection(frame, det);

        if (det.found && det.score > bestScore) {
            bestScore = det.score;
            bestFrame = frame.clone();
        }

        if (keyFrames.count(index)) {
            char name[64];
            snprintf(name, sizeof(name), "detect_%03d.jpg", index);
            imwrite(joinPath(outDir, name), frame);
        }

        if (writer.isOpened()) writer.write(frame);

        if (gui) {
            imshow("Task 2 - Armor Detection", frame);
            const int key = waitKey(1);
            if (key == 'q' || key == 'Q' || key == 27) break;
        }
        ++index;
    }

    cap.release();
    if (writer.isOpened()) writer.release();

    if (bestFrame.empty()) bestFrame = frame.clone();
    if (!bestFrame.empty()) {
        imwrite(joinPath(outDir, "detect_result.jpg"), bestFrame);
    }

    cout << "  处理帧数: " << index << ", 检测成功: " << foundCount;
    if (index > 0) cout << " (" << (100.0 * foundCount / index) << "%)";
    cout << "\n  已保存: armor_detected.mp4, detect_XXX.jpg, detect_result.jpg\n";

    if (gui) {
        cout << "  按任意键退出 ...\n";
        waitKey(0);
        destroyAllWindows();
    }
    return true;
}

// ---------------- main ----------------
int main(int argc, char** argv) {
    bool gui = true;
    vector<string> pos;
    for (int i = 1; i < argc; ++i) {
        const string a = argv[i];
        if (a == "--no-gui" || a == "--headless") {
            gui = false;
        } else {
            pos.push_back(a);
        }
    }

    const string imagePath = pos.size() > 0 ? pos[0] : "armor_img.jpg";
    const string videoPath = pos.size() > 1 ? pos[1] : "armor_video.mp4";
    const string outDir    = pos.size() > 2 ? pos[2] : "output";

    makeDir(outDir);

    cout << "OpenCV " << CV_VERSION << "\n"
         << "图像: " << imagePath << "\n"
         << "视频: " << videoPath << "\n"
         << "输出: " << outDir << "\n";

    if (!task1Image(imagePath, outDir, gui)) return 1;
    if (!task2Video(videoPath, outDir, gui)) return 1;

    cout << "\n全部完成。\n";
    return 0;
}
