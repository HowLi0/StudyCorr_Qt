#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>

int main() {
    // 1. 加载图像和标定板的世界坐标
    std::vector<cv::Point3f> objectPoints;
    float squareSize = 3.0;  // 标定板的每个方块边长（单位：mm）

    // 假设标定板有12x11个方块（通常标定板的角点数为12x11）
    for (int i = 0; i < 11; ++i) {  // 11行
        for (int j = 0; j < 12; ++j) {  // 12列
            objectPoints.push_back(cv::Point3f(j * squareSize, i * squareSize, 0.0f));
        }
    }

    // 2. 加载图像并检测角点
    cv::Mat image = cv::imread("E:\\一些代码Matlab\\折射校准\\20241201\\calibration_images\\CameraA\\9.bmp");

    if (image.empty()) {
        std::cerr << "图像加载失败!" << std::endl;
        return -1;
    }

    // 转换为灰度图像
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    std::vector<cv::Point2f> imagePoints;
    bool found = cv::findChessboardCorners(grayImage, cv::Size(12, 11), imagePoints);

    if (!found) {
        std::cerr << "标定板角点未能检测到!" << std::endl;
        return -1;
    }

    // 3. 提高角点检测精度
    cv::cornerSubPix(grayImage, imagePoints, cv::Size(11, 11), cv::Size(-1, -1),
        cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.1));

    // 4. 将objectPoints和imagePoints转换为cv::Mat格式
    std::vector<std::vector<cv::Point3f>> objectPointsVec(1, objectPoints);
    std::vector<std::vector<cv::Point2f>> imagePointsVec(1, imagePoints);

    // 5. 相机内参和畸变系数初始化
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);  // 初始化相机内参矩阵
    cv::Mat distCoeffs = cv::Mat::zeros(1, 5, CV_64F);  // 初始化畸变系数

    // 6. 使用calibrateCamera来计算内参和外参
    cv::Mat rvecs, tvecs;
    double rms = cv::calibrateCamera(objectPointsVec, imagePointsVec, image.size(),
        cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "重投影误差： " << rms << std::endl;

    // 输出相机内参、畸变系数、外参
    std::cout << "相机内参矩阵：" << std::endl << cameraMatrix << std::endl;
    std::cout << "畸变系数：" << std::endl << distCoeffs << std::endl;

    // 打印外参（旋转向量和平移向量）
    std::cout << "外参（旋转向量）： " << std::endl << rvecs << std::endl;
    std::cout << "外参（平移向量）： " << std::endl << tvecs << std::endl;

    // 将旋转向量转换为旋转矩阵
    cv::Mat R;
    cv::Rodrigues(rvecs, R);
    std::cout << "旋转矩阵： " << std::endl << R << std::endl;

    return 0;
}
