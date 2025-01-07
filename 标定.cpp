#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

// 函数，用于从给定路径中加载指定扩展名的所有图像文件
void loadImagesFromFolder(const std::string& folderPath, const std::string& extension, std::vector<cv::Mat>& images) {
    std::vector<std::string> fileNames;
    cv::glob(folderPath + "/*" + extension, fileNames);  // 查找所有指定扩展名的文件

    for (const auto& file : fileNames) {
        cv::Mat image = cv::imread(file);
        if (!image.empty()) {
            images.push_back(image);  // 如果图像成功加载，则存储
        }
        else {
            std::cerr << "Failed to load image: " << file << std::endl;
        }
    }
}

int main() {
    // Square size in millimeters
    float squareSize = 3.0f;

    std::vector<cv::Mat> imagesA, imagesB;

    // 加载 CameraA 目录中的 .bmp 图像
    loadImagesFromFolder("E:\\一些代码Matlab\\折射校准\\20241201\\calibration_images\\CameraA", ".bmp", imagesA);

    // 加载 CameraB 目录中的 .bmp 图像
    loadImagesFromFolder("E:\\一些代码Matlab\\折射校准\\20241201\\calibration_images\\CameraC", ".bmp", imagesB);

    std::vector<std::vector<cv::Point2f>> imagePointsA, imagePointsB;
    cv::Size boardSize(12, 11);  // Checkerboard size, adjust based on your checkerboard

    for (size_t i = 0; i < imagesA.size(); i++) {
        cv::Mat grayA, grayB;
        cv::cvtColor(imagesA[i], grayA, cv::COLOR_BGR2GRAY);
        cv::cvtColor(imagesB[i], grayB, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> cornersA, cornersB;

        bool foundA = cv::findChessboardCorners(imagesA[i], boardSize, cornersA);
        bool foundB = cv::findChessboardCorners(imagesB[i], boardSize, cornersB);

        if (foundA && foundB) {
            cv::cornerSubPix(grayA, cornersA, cv::Size(11, 11), cv::Size(-1, -1),
                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
            cv::cornerSubPix(grayB, cornersB, cv::Size(11, 11), cv::Size(-1, -1),
                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
            imagePointsA.push_back(cornersA);
            imagePointsB.push_back(cornersB);
        }
        else {
            std::cerr << "Chessboard corners not found in image " << i + 1 << std::endl;
        }
    }

    // Generate world points (3D coordinates of checkerboard points)
    std::vector<std::vector<cv::Point3f>> objectPoints(1);
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            objectPoints[0].push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
        }
    }
    objectPoints.resize(imagePointsA.size(), objectPoints[0]);

    // Camera calibration for both cameras
    cv::Mat cameraMatrixA, distCoeffsA, cameraMatrixB, distCoeffsB;
    std::vector<cv::Mat> rvecsA, tvecsA, rvecsB, tvecsB;
    cv::Mat points4D;

    // 进行相机标定
    double rms1 = cv::calibrateCamera(objectPoints, imagePointsA, imagesA[0].size(), cameraMatrixA, distCoeffsA, rvecsA, tvecsA);
    double rms2 = cv::calibrateCamera(objectPoints, imagePointsB, imagesB[0].size(), cameraMatrixB, distCoeffsB, rvecsB, tvecsB);

    // 打印相机内参
    std::cout << "重投影误差A： " << rms1 << std::endl;
    std::cout << "Camera Matrix A: \n" << cameraMatrixA << std::endl;
    std::cout << "Distortion Coefficients A: \n" << distCoeffsA << std::endl;

    std::cout << "重投影误差B： " << rms2 << std::endl;
    std::cout << "Camera Matrix B: \n" << cameraMatrixB << std::endl;
    std::cout << "Distortion Coefficients B: \n" << distCoeffsB << std::endl;

    for (size_t i = 0; i < imagesA.size(); i++) {
        cv::Mat R_A, R_B;
        // Convert rotation vectors to rotation matrices
        cv::Rodrigues(rvecsA[i], R_A);
        cv::Rodrigues(rvecsB[i], R_B);

        // 打印外参（旋转矩阵和平移向量）
        std::cout << "Rotation Matrix A for image " << i + 1 << ":\n" << R_A << std::endl;
        std::cout << "Translation Vector A for image " << i + 1 << ":\n" << tvecsA[i] << std::endl;

        std::cout << "Rotation Matrix B for image " << i + 1 << ":\n" << R_B << std::endl;
        std::cout << "Translation Vector B for image " << i + 1 << ":\n" << tvecsB[i] << std::endl;

        // Construct projection matrix for Camera A: P = K * [R | T]
        cv::Mat projMatrixA = cv::Mat::zeros(3, 4, CV_64F);
        cv::Mat RT_A;
        cv::hconcat(R_A, tvecsA[i], RT_A);  // Combine R and T into a 3x4 matrix
        projMatrixA = cameraMatrixA * RT_A;

        // Construct projection matrix for Camera B: P = K * [R | T]
        cv::Mat projMatrixB = cv::Mat::zeros(3, 4, CV_64F);
        cv::Mat RT_B;
        cv::hconcat(R_B, tvecsB[i], RT_B);  // Combine R and T into a 3x4 matrix
        projMatrixB = cameraMatrixB * RT_B;

        // Triangulate points
        cv::triangulatePoints(projMatrixA, projMatrixB, imagePointsA[i], imagePointsB[i], points4D);
    }

    // Convert homogeneous coordinates to 3D
    std::vector<cv::Point3f> points3D;
    for (int i = 0; i < points4D.cols; i++) {
        cv::Point3f p;
        p.x = points4D.at<float>(0, i) / points4D.at<float>(3, i);
        p.y = points4D.at<float>(1, i) / points4D.at<float>(3, i);
        p.z = points4D.at<float>(2, i) / points4D.at<float>(3, i);
        points3D.push_back(p);
    }

    // 计算重投影误差
    double totalReprojectionError = 0.0;
    int totalPoints = 0;

    for (size_t i = 0; i < imagesA.size(); i++) {
        std::vector<cv::Point2f> projectedPointsA, projectedPointsB;

        // 将3D点投影到图像平面
        cv::projectPoints(points3D, rvecsA[i], tvecsA[i], cameraMatrixA, distCoeffsA, projectedPointsA);
        cv::projectPoints(points3D, rvecsB[i], tvecsB[i], cameraMatrixB, distCoeffsB, projectedPointsB);

        // 计算重投影误差
        for (size_t j = 0; j < imagePointsA[i].size(); j++) {
            double errorA = cv::norm(imagePointsA[i][j] - projectedPointsA[j]);
            double errorB = cv::norm(imagePointsB[i][j] - projectedPointsB[j]);
            totalReprojectionError += (errorA + errorB)/12/11;
            totalPoints += 2;  // 每个图像点对应两个误差（Camera A 和 Camera B）
        }
    }

    double meanReprojectionError = totalReprojectionError / totalPoints;

    // 计算误差的方差
    double variance = 0.0;
    for (size_t i = 0; i < imagesA.size(); i++) {
        std::vector<cv::Point2f> projectedPointsA, projectedPointsB;

        // 将3D点投影到图像平面
        cv::projectPoints(points3D, rvecsA[i], tvecsA[i], cameraMatrixA, distCoeffsA, projectedPointsA);
        cv::projectPoints(points3D, rvecsB[i], tvecsB[i], cameraMatrixB, distCoeffsB, projectedPointsB);

        // 计算误差并求和
        for (size_t j = 0; j < imagePointsA[i].size(); j++) {
            double errorA = cv::norm(imagePointsA[i][j] - projectedPointsA[j]);
            double errorB = cv::norm(imagePointsB[i][j] - projectedPointsB[j]);
            double error = (errorA + errorB) / 2;  // 平均误差
            variance += (error - meanReprojectionError) * (error - meanReprojectionError);
        }
    }

    variance /= totalPoints;  // 计算方差

    std::cout << "Mean Reprojection Error: " << meanReprojectionError << std::endl;
    std::cout << "Variance of Reprojection Error: " << variance << std::endl;

    return 0;
}
