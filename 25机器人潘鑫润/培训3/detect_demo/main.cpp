#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
using namespace cv;

// ================= 函数声明 =================
Mat preProcessing(Mat img);
vector<RotatedRect> findLightBars(Mat red_mask);
void drawArmorBox(Mat& img, RotatedRect leftBar, RotatedRect rightBar);
float getDistance(RotatedRect a, RotatedRect b);

int main()
{
    // ================= 任务 2：读取视频并识别装甲板 =================
    string videoPath = "../test_clip.mp4"; 
    VideoCapture cap(videoPath);

    if (!cap.isOpened())
    {
        cout << "视频打开失败，请检查路径是否正确" << endl;
        return -1;
    }

    Mat img;
    while (true)
    {
        cap.read(img);
        if (img.empty()) break; // 视频读完退出

        Mat red_mask = preProcessing(img); 
        vector<RotatedRect> lightBars = findLightBars(red_mask);

        // 两两配对寻找最合理的一对灯条
        if (lightBars.size() >= 2)
        {
            RotatedRect bestLeft, bestRight;
            float minDistance = 1e9; 

            for (int i = 0; i < lightBars.size(); i++) {
                for (int j = i + 1; j < lightBars.size(); j++) {
                    
                    RotatedRect leftBar = (lightBars[i].center.x < lightBars[j].center.x) ? lightBars[i] : lightBars[j];
                    RotatedRect rightBar = (lightBars[i].center.x < lightBars[j].center.x) ? lightBars[j] : lightBars[i];

                    float leftH = max(leftBar.size.width, leftBar.size.height);
                    float rightH = max(rightBar.size.width, rightBar.size.height);
                    float avgHeight = (leftH + rightH) / 2.0f;
                    float dist = getDistance(leftBar, rightBar);

                    if (dist > avgHeight * 5.0f) continue; 
                    if (dist < avgHeight * 1.0f) continue;
                    if (abs(leftH - rightH) > avgHeight * 0.4f) continue;
                    if (abs(leftBar.center.y - rightBar.center.y) > avgHeight * 0.4f) continue;

                    float angleL = leftBar.angle;
                    if (angleL > 45.0f) angleL -= 90.0f;
                    else if (angleL < -45.0f) angleL += 90.0f;

                    float angleR = rightBar.angle;
                    if (angleR > 45.0f) angleR -= 90.0f;
                    else if (angleR < -45.0f) angleR += 90.0f;
                    if (abs(angleL - angleR) > 15.0f) continue;

                    float maxL = max(leftBar.size.width, leftBar.size.height);
                    float minL = min(leftBar.size.width, leftBar.size.height);
                    if (maxL / minL < 1.8f) continue;
                    float maxR = max(rightBar.size.width, rightBar.size.height);
                    float minR = min(rightBar.size.width, rightBar.size.height);
                    if (maxR / minR < 1.8f) continue;

                    if (dist < minDistance) {
                        minDistance = dist;
                        bestLeft = leftBar;
                        bestRight = rightBar;
                    }
                }
            }

            if (minDistance < 1e8) {
                drawArmorBox(img, bestLeft, bestRight);
            }
        }

        imshow("Detection", img);
        waitKey(30);
    }

    cap.release();
    destroyAllWindows();
    return 0;
}

// ================= 函数实现 =================

Mat preProcessing(Mat img)
{
    Mat imgHSV, mask1, mask2, red_mask, kernel;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    inRange(imgHSV, Scalar(0, 70, 70), Scalar(15, 255, 255), mask1);
    inRange(imgHSV, Scalar(165, 70, 70), Scalar(180, 255, 255), mask2);
    bitwise_or(mask1, mask2, red_mask);
    kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(red_mask, red_mask, MORPH_CLOSE, kernel);
    morphologyEx(red_mask, red_mask, MORPH_OPEN, kernel);
    return red_mask;
}

vector<RotatedRect> findLightBars(Mat red_mask)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(red_mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<RotatedRect> validBars;
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) < 20) continue;
        RotatedRect rect = minAreaRect(contours[i]);
        float max_len = max(rect.size.width, rect.size.height);
        float min_len = min(rect.size.width, rect.size.height);
        float aspect_ratio = max_len / min_len;
        if (aspect_ratio > 1.8 && aspect_ratio < 12.0) {
            validBars.push_back(rect);
        }
    }
    return validBars;
}

float getDistance(RotatedRect a, RotatedRect b) {
    return norm(a.center - b.center);
}

// 核心画图：满足老师“两个细长小框 + 一个包住整体的大框 + 每个框的4个角点都标出”的要求
void drawArmorBox(Mat& img, RotatedRect leftBar, RotatedRect rightBar)
{
    Point2f leftPts[4], rightPts[4];
    leftBar.points(leftPts); 
    rightBar.points(rightPts);

    // 1. 画小框 (黄色)
    for (int j = 0; j < 4; j++) {
        line(img, leftPts[j], leftPts[(j + 1) % 4], Scalar(0, 255, 255), 2);
        line(img, rightPts[j], rightPts[(j + 1) % 4], Scalar(0, 255, 255), 2);
    }

    // 2. 在图像上显示灯条的四个角点 (每个灯条的4个点都画出来，黄色圆点)
    for(int j = 0; j < 4; j++) {
        circle(img, leftPts[j], 3, Scalar(0, 255, 255), -1);
        circle(img, rightPts[j], 3, Scalar(0, 255, 255), -1);
    }

    // 3. 计算“包住整体的大框”的四个角点
    // 寻找左灯条最靠左的上下两点（大框的外侧）
    sort(leftPts, leftPts + 4, [](Point2f a, Point2f b) { return a.x < b.x; });
    Point2f armorLT = leftPts[0].y < leftPts[1].y ? leftPts[0] : leftPts[1]; // 整体左上
    Point2f armorLB = leftPts[0].y > leftPts[1].y ? leftPts[0] : leftPts[1]; // 整体左下

    // 寻找右灯条最靠右的上下两点（大框的外侧）
    sort(rightPts, rightPts + 4, [](Point2f a, Point2f b) { return a.x > b.x; });
    Point2f armorRT = rightPts[0].y < rightPts[1].y ? rightPts[0] : rightPts[1]; // 整体右上
    Point2f armorRB = rightPts[0].y > rightPts[1].y ? rightPts[0] : rightPts[1]; // 整体右下

    // 4. 连接大框的四个角点，把整个装甲板框出来 (绿色)
    Scalar armorColor(0, 255, 0); 
    line(img, armorLT, armorRT, armorColor, 1); // 上边
    line(img, armorRT, armorRB, armorColor, 1); // 右边
    line(img, armorRB, armorLB, armorColor, 1); // 下边
    line(img, armorLB, armorLT, armorColor, 1); // 左边

    // 5. 标出大框的4个角点 (黄色实心圆)
    circle(img, armorLT, 4, armorColor, -1);
    circle(img, armorRT, 4, armorColor, -1);
    circle(img, armorRB, 4, armorColor, -1);
    circle(img, armorLB, 4, armorColor, -1);

       // ================= 新增：添加 P1-P4 文字标记 =================
    // 设置文字参数：字体、缩放比例、颜色(绿色)、线宽
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    
    // P1 标记 (左上角)：文字向右上偏移，避免遮挡圆点
    putText(img, "P1", Point(armorLT.x + 5, armorLT.y - 5), fontFace, fontScale, armorColor, thickness);
    
    // P2 标记 (右上角)
    putText(img, "P2", Point(armorRT.x + 5, armorRT.y - 5), fontFace, fontScale, armorColor, thickness);
    
    // P3 标记 (右下角)
    putText(img, "P3", Point(armorRB.x + 5, armorRB.y + 15), fontFace, fontScale, armorColor, thickness);
    
    // P4 标记 (左下角)
    putText(img, "P4", Point(armorLB.x - 25, armorLB.y + 15), fontFace, fontScale, armorColor, thickness);

}