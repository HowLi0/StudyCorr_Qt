#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 主函数
int main(int argc, char** argv) {
    // 读取输入图像
    Mat image = imread("../2.bmp", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "无法加载图像!" << endl;
        return -1;
    }

    // 1. 对输入图像进行预处理（平滑）
    Mat smoothed;
    GaussianBlur(image, smoothed, Size(5, 5), 0);

    // 2. 使用Canny边缘检测提取边缘
    Mat edges;
    double lowThreshold = 50;  // 低阈值
    double highThreshold = 150;  // 高阈值
    Canny(smoothed, edges, lowThreshold, highThreshold);

    // 3. 创建标记矩阵，初始化为背景（值为0）
    Mat markers = Mat::zeros(image.size(), CV_32SC1);

    // 4. 手动选择前景和背景区域
    // 选择前景区域
    cout << "请手动选择前景区域..." << endl;
    Rect foreground = selectROI("Select Foreground", image, false, false);
    if (foreground.width == 0 || foreground.height == 0) {
        cout << "未选择有效的前景区域!" << endl;
        return -1;
    }
    rectangle(markers, foreground, Scalar(1), -1);  // 在标记矩阵中标记前景

    // 选择背景区域
    cout << "请手动选择背景区域..." << endl;
    Rect background = selectROI("Select Background", image, false, false);
    if (background.width == 0 || background.height == 0) {
        cout << "未选择有效的背景区域!" << endl;
        return -1;
    }
    rectangle(markers, background, Scalar(2), -1);  // 在标记矩阵中标记背景

    // 5. 将灰度图像转换为三通道图像
    Mat colorImage;
    cvtColor(image, colorImage, COLOR_GRAY2BGR);  // 转换为3通道的BGR图像

    // 6. 应用分水岭算法
    watershed(colorImage, markers);

    // 7. 将分水岭标记转换为可显示的图像
    Mat markersNormalized;
    markers.convertTo(markersNormalized, CV_8UC1);
    normalize(markersNormalized, markersNormalized, 0, 255, NORM_MINMAX);

    // 8. 绘制分水岭分割的结果
    Mat watershedResult;
    cvtColor(image, watershedResult, COLOR_GRAY2BGR);
    watershedResult.setTo(Scalar(0, 0, 255), markersNormalized == 1); // 前景用红色标记
    watershedResult.setTo(Scalar(255, 0, 0), markersNormalized == 2); // 背景用蓝色标记

    // 9. 显示分水岭分割的结果
    namedWindow("Watershed Segmentation", WINDOW_NORMAL);
    imshow("Watershed Segmentation", watershedResult);

    // 10. 提取ROI区域（通过前景区域）
    Mat roiMask = markersNormalized == 1;  // 获取前景区域
    Mat roiResult;
    image.copyTo(roiResult, roiMask);  // 将ROI区域从原图复制到新图

    // 11. 显示ROI区域
    namedWindow("ROI Region", WINDOW_NORMAL);
    imshow("ROI Region", roiResult);

    // 保存结果
    imwrite("../watershed_roi_result.jpg", roiResult);

    waitKey(0);
    return 0;
}
