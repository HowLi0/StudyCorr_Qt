#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // 直接读取图像
    Mat image = imread("../3.bmp", IMREAD_GRAYSCALE); // 灰度模式读取
    if (image.empty()) {
        cout << "无法加载图像!" << endl;
        return -1;
    }

    // 二值化
    Mat binary;
    threshold(image, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // 查找轮廓
    vector<vector<Point>> contours;
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 创建一个空白图像，用于绘制自适应轮廓
    Mat contourMask = Mat::zeros(image.size(), CV_8UC1);

    // 遍历所有轮廓
    for (const auto& contour : contours) {
        // 对轮廓进行多边形逼近，简化形状
        vector<Point> approxContour;
        double epsilon = 0.0002 * arcLength(contour, true); // 设置逼近精度
        approxPolyDP(contour, approxContour, epsilon, true);

        // 填充轮廓区域
        drawContours(contourMask, vector<vector<Point>>{approxContour}, -1, Scalar(255), FILLED);
    }

    // 提取ROI，使用掩模与原图相乘
    Mat roi = image.clone();
    roi.setTo(0, contourMask == 0);  // 将掩模为0的部分设为0，即保留ROI部分

    // 转换为彩色图像以绘制红色轮廓
    Mat roiWithContours;
    cvtColor(roi, roiWithContours, COLOR_GRAY2BGR);

    // 在 ROI 上绘制红色轮廓
    for (const auto& contour : contours) {
        // 对轮廓进行多边形逼近，简化形状
        vector<Point> approxContour;
        double epsilon = 0.0002 * arcLength(contour, true); // 设置逼近精度
        approxPolyDP(contour, approxContour, epsilon, true);

        // 在 ROI 上绘制红色轮廓
        drawContours(roiWithContours, vector<vector<Point>>{approxContour}, -1, Scalar(0, 0, 255), 1); // 红色线条，宽度3
    }

    // 生成点集
    int stepSize = 15;   // 步长
    int windowSize = 40;  // DIC 窗口大小
    vector<Point> dicPoints;

    for (int y = 0; y < roi.rows; y += stepSize) {
        for (int x = 0; x < roi.cols; x += stepSize) {
            // 检查点是否在 ROI 掩模区域内
            if (contourMask.at<uchar>(y, x) == 255) {
                dicPoints.emplace_back(x, y);
            }
        }
    }

    // 可视化点集
    for (const auto& point : dicPoints) {
        circle(roiWithContours, point, 2, Scalar(0, 255, 0), FILLED); // 绘制绿色点
    }

    namedWindow("Original Image", WINDOW_NORMAL);
    imshow("Original Image", image);
    cv::resizeWindow("Original Image", 600, 400);  // 设置显示窗口的大小

    namedWindow("ROI", WINDOW_NORMAL);
    imshow("ROI", roi);
    cv::resizeWindow("ROI", 600, 400);  // 设置显示窗口的大小

    namedWindow("ROI with DIC Points", WINDOW_NORMAL);
    imshow("ROI with DIC Points", roiWithContours); // 带红色轮廓的结果图像
    cv::resizeWindow("ROI with DIC Points", 600, 400);  // 设置显示窗口的大小
    waitKey(0);
    return 0;
}
