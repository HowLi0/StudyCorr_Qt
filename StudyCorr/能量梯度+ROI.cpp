#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 主函数
int main(int argc, char** argv) {
    // 读取输入图像
    Mat image = imread("../2.bmp", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "\u65e0\u6cd5\u52a0\u8f7d\u56fe\u50cf!" << endl;
        return -1;
    }

    // 1. 计算梯度能量
    Mat gradX, gradY, gradMag;
    Sobel(image, gradX, CV_64F, 1, 0, 3); // X方向梯度
    Sobel(image, gradY, CV_64F, 0, 1, 3); // Y方向梯度
    magnitude(gradX, gradY, gradMag);     // 计算梯度幅值

    // 归一化梯度幅值以便显示
    Mat gradMagNorm;
    normalize(gradMag, gradMagNorm, 0, 255, NORM_MINMAX, CV_8UC1);

    // 阈值化梯度幅值以提取高能量区域
    Mat binaryMask;
    double thresholdValue =60; // 根据图像调整阈值
    threshold(gradMagNorm, binaryMask, thresholdValue, 255, THRESH_BINARY);

    // 形态学操作以优化区域
    Mat kernel = getStructuringElement(MORPH_RECT, Size(8, 8));
    morphologyEx(binaryMask, binaryMask, MORPH_CLOSE, kernel); // 闭运算填补间隙

    // 查找高能量区域的轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binaryMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 创建 ROI 掩膜
    Mat roiMask = Mat::zeros(image.size(), CV_8UC1);
    drawContours(roiMask, contours, -1, Scalar(255), FILLED); // 绘制轮廓到掩膜

    // 显示 ROI 掩膜
    namedWindow("ROI Mask", WINDOW_NORMAL);
    imshow("ROI Mask", roiMask);

    // 2. 自动选取 ROI 内的点
    int stepSize = 5;  // 步长
    vector<Point> selectedPoints;

    for (int y = 0; y < image.rows; y += stepSize) {
        for (int x = 0; x < image.cols; x += stepSize) {
            if (roiMask.at<uchar>(y, x) == 255) {  // 如果在 ROI 内
                selectedPoints.emplace_back(x, y);
            }
        }
    }

    // 3. 可视化结果
    Mat visualization;
    cvtColor(image, visualization, COLOR_GRAY2BGR);

    // 绘制 ROI 区域轮廓
    drawContours(visualization, contours, -1, Scalar(0, 255, 0), 2);  // 绿色表示 ROI 区域

    // 在图像上绘制选取的点
    for (const auto& point : selectedPoints) {
        circle(visualization, point, 3, Scalar(0, 0, 255), FILLED);  // 红色表示选取点
    }

    // 显示结果
    namedWindow("Visualization with Selected Points", WINDOW_NORMAL);
    imshow("Visualization with Selected Points", visualization);

    // 保存结果
    imwrite("../gradient_based_roi.jpg", visualization);

    waitKey(0);
    return 0;
}