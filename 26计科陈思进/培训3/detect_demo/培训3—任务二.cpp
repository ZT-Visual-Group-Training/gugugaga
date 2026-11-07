#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/geometry/2d.hpp> 
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

struct CornerPair { Point2f top, bottom; };

CornerPair outer_corners(const RotatedRect& lightbar, bool take_max_x) {
    Point2f p[4];
    lightbar.points(p);
    vector<Point2f> pts(p, p + 4);
    sort(pts.begin(), pts.end(), [](const Point2f& a, const Point2f& b) {
        return a.y < b.y;
        });
    auto pick = [take_max_x](const Point2f& a, const Point2f& b) {
        return (take_max_x ? a.x > b.x : a.x < b.x) ? a : b;
        };
    return { pick(pts[0], pts[1]), pick(pts[2], pts[3]) };
}

void draw_armor(Mat& img, const RotatedRect& left, const RotatedRect& right) {
    CornerPair l = outer_corners(left, false);
    CornerPair r = outer_corners(right, true);
    vector<Point> quad = { l.top, l.bottom, r.top, r.bottom };

    const vector<size_t> kQuadOrder = { 0, 2, 3, 1 };
    vector<Point> ordered(4);
    for (size_t i = 0; i < 4; ++i) ordered[i] = quad[kQuadOrder[i]];

    polylines(img, vector<vector<Point>>{ordered}, true, Scalar(0, 255, 0), 2, LINE_AA);

    rectangle(img, left.boundingRect(), Scalar(0, 255, 255), 2);
    rectangle(img, right.boundingRect(), Scalar(0, 255, 255), 2);

    const char* labels[4] = { "P1", "P2", "P3", "P4" };
    for (int i = 0; i < 4; ++i) {
        Point p = ordered[i];
        line(img, { p.x - 8, p.y }, { p.x + 8, p.y }, Scalar(0, 255, 0), 1, LINE_AA);
        line(img, { p.x, p.y - 8 }, { p.x, p.y + 8 }, Scalar(0, 255, 0), 1, LINE_AA);
        circle(img, p, 3, Scalar(0, 255, 0), -1, LINE_AA);
        circle(img, p, 8, Scalar(0, 255, 0), 1, LINE_AA);

        bool up = (i < 2);
        putText(img, labels[i], { p.x + 11, up ? p.y - 12 : p.y + 22 },
            FONT_HERSHEY_SIMPLEX, 0.45, Scalar(0, 255, 0), 1, LINE_AA);
    }
}


bool isLightBar(const RotatedRect& rect, double contourArea) {
    if (contourArea < 30) return false; 

    float w = rect.size.width;
    float h = rect.size.height;
    float max_len = max(w, h);
    float min_len = min(w, h);

    
    if (max_len / min_len < 1.5) return false;
    
    if (max_len < 15.0) return false;

    return true;
}


bool isArmorPair(const RotatedRect& a, const RotatedRect& b) {
    float len_a = max(a.size.width, a.size.height);
    float len_b = max(b.size.width, b.size.height);

   
    if (max(len_a, len_b) / min(len_a, len_b) > 1.5) return false;

    
    float angle_a = (a.size.width > a.size.height) ? a.angle + 90 : a.angle;
    float angle_b = (b.size.width > b.size.height) ? b.angle + 90 : b.angle;
    if (abs(angle_a - angle_b) > 25.0) return false;

    
    float dist = norm(a.center - b.center);
    float avg_len = (len_a + len_b) / 2.0;
    float dist_ratio = dist / avg_len;
    if (dist_ratio < 1.0 || dist_ratio > 5.0) return false;

  
    if (abs(a.center.y - b.center.y) > avg_len * 0.7) return false;

    return true;
}

int main() {
    string videoPath = "C:/Users/Administrator/Desktop/video.mp4";
    VideoCapture cap(videoPath);
    if (!cap.isOpened()) return -1;

    string outPath = "outputs/train3_demo.mp4";
    fs::create_directories("outputs");
    int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS) > 1.0 ? cap.get(CAP_PROP_FPS) : 30.0;
    VideoWriter writer(outPath, VideoWriter::fourcc('m', 'p', '4', 'v'), fps, Size(width, height));

    Mat frame;
    namedWindow("Armor Detection", WINDOW_NORMAL);

    while (cap.read(frame)) {
        if (frame.empty()) break;

        Mat hsv, mask1, mask2, redMask;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(0, 50, 40), Scalar(10, 255, 255), mask1);
        inRange(hsv, Scalar(170, 50, 40), Scalar(180, 255, 255), mask2);
        bitwise_or(mask1, mask2, redMask);

        
        Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(redMask, redMask, MORPH_CLOSE, kernel);

        vector<vector<Point>> contours;
        findContours(redMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<RotatedRect> lightBars;
        for (const auto& contour : contours) {
            RotatedRect rect = minAreaRect(contour);
            if (isLightBar(rect, contourArea(contour))) {
                lightBars.push_back(rect);
            }
        }

        double currentMaxArea = 0;
        RotatedRect bestLeft, bestRight;
        bool foundPair = false;

        
        for (size_t i = 0; i < lightBars.size(); ++i) {
            for (size_t j = i + 1; j < lightBars.size(); ++j) {
                if (!isArmorPair(lightBars[i], lightBars[j])) continue;

                double area = lightBars[i].size.area() + lightBars[j].size.area();
                if (area > currentMaxArea) {
                    currentMaxArea = area;
                    
                    if (lightBars[i].center.x < lightBars[j].center.x) {
                        bestLeft = lightBars[i];
                        bestRight = lightBars[j];
                    }
                    else {
                        bestLeft = lightBars[j];
                        bestRight = lightBars[i];
                    }
                    foundPair = true;
                }
            }
        }

        
        if (foundPair) {
            draw_armor(frame, bestLeft, bestRight);
        }

        imshow("Armor Detection", frame);
        writer.write(frame);

        int key = waitKey(30);
        if (key == 27) break;
        if (key == 32) {
            while (true) {
                int pauseKey = waitKey(0);
                if (pauseKey == 32 || pauseKey == 27) break;
            }
        }
    }

    writer.release();
    cap.release();
    destroyAllWindows();
    return 0;
}