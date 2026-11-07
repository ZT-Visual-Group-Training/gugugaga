#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("video.mp4");
    if (!cap.isOpened()) { cout << "视频打开失败" << endl; return -1; }

    Mat frame;
   
    while (cap.read(frame)) {
     	VideoWriter writer("result.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), 30, frame.size());
        Mat hsv, mask1, mask2, binary;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        
        // 1. 提取红色
        inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1);
        inRange(hsv, Scalar(160, 100, 100), Scalar(180, 255, 255), mask2);
        bitwise_or(mask1, mask2, binary);

        // 2. 找轮廓
        vector<vector<Point>> contours;
        findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Rect> valid_rects; // 存放找到的灯条矩形

        for (size_t i = 0; i < contours.size(); i++) {
            Rect rect = boundingRect(contours[i]); // 使用正矩形
            float width = rect.width;
            float height = rect.height;
            if (width == 0 || height == 0) continue;

            // 3. 过滤：灯条通常高大于宽（长宽比 > 1.5），且面积适中
            float ratio = height / (float)width; 
            float area = width * height;

            if (ratio > 1.5 && area > 50) {
                valid_rects.push_back(rect);
            }
        }

        // 4. 如果找到了两个灯条（通常是左边一个，右边一个）
        if (valid_rects.size() == 2) {
            // 区分左右灯条
            Rect left_rect, right_rect;
            if (valid_rects[0].x < valid_rects[1].x) {
                left_rect = valid_rects[0];
                right_rect = valid_rects[1];
            } else {
                left_rect = valid_rects[1];
                right_rect = valid_rects[0];
            }

            // 5. 提取最外围的四个角点
            Point2f P1(left_rect.x, left_rect.y);                                   // 左上
            Point2f P2(right_rect.x + right_rect.width, right_rect.y);              // 右上
            Point2f P3(right_rect.x + right_rect.width, right_rect.y + right_rect.height); // 右下
            Point2f P4(left_rect.x, left_rect.y + left_rect.height);                // 左下

            // 画四个角点（黄绿色实心圆）
            Scalar point_color(0, 255, 0);
            circle(frame, P1, 5, point_color, -1);
            circle(frame, P2, 5, point_color, -1);
            circle(frame, P3, 5, point_color, -1);
            circle(frame, P4, 5, point_color, -1);

            // 6. 连接四个角点（黄绿色线）
            line(frame, P1, P2, point_color, 2);
            line(frame, P2, P3, point_color, 2);
            line(frame, P3, P4, point_color, 2);
            line(frame, P4, P1, point_color, 2);

            // 画灯条本身的黄色外接矩形
            Scalar rect_color(0, 255, 255);
            rectangle(frame, left_rect, rect_color, 2);
            rectangle(frame, right_rect, rect_color, 2);
        }

        imshow("Armor Detection", frame);
        writer.write(frame);
        if (waitKey(30) == 'q') break; 
    }
    return 0;
}
