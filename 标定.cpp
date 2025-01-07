#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

// ���������ڴӸ���·���м���ָ����չ��������ͼ���ļ�
void loadImagesFromFolder(const std::string& folderPath, const std::string& extension, std::vector<cv::Mat>& images) {
    std::vector<std::string> fileNames;
    cv::glob(folderPath + "/*" + extension, fileNames);  // ��������ָ����չ�����ļ�

    for (const auto& file : fileNames) {
        cv::Mat image = cv::imread(file);
        if (!image.empty()) {
            images.push_back(image);  // ���ͼ��ɹ����أ���洢
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

    // ���� CameraA Ŀ¼�е� .bmp ͼ��
    loadImagesFromFolder("E:\\һЩ����Matlab\\����У׼\\20241201\\calibration_images\\CameraA", ".bmp", imagesA);

    // ���� CameraB Ŀ¼�е� .bmp ͼ��
    loadImagesFromFolder("E:\\һЩ����Matlab\\����У׼\\20241201\\calibration_images\\CameraC", ".bmp", imagesB);

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

    // ��������궨
    double rms1 = cv::calibrateCamera(objectPoints, imagePointsA, imagesA[0].size(), cameraMatrixA, distCoeffsA, rvecsA, tvecsA);
    double rms2 = cv::calibrateCamera(objectPoints, imagePointsB, imagesB[0].size(), cameraMatrixB, distCoeffsB, rvecsB, tvecsB);

    // ��ӡ����ڲ�
    std::cout << "��ͶӰ���A�� " << rms1 << std::endl;
    std::cout << "Camera Matrix A: \n" << cameraMatrixA << std::endl;
    std::cout << "Distortion Coefficients A: \n" << distCoeffsA << std::endl;

    std::cout << "��ͶӰ���B�� " << rms2 << std::endl;
    std::cout << "Camera Matrix B: \n" << cameraMatrixB << std::endl;
    std::cout << "Distortion Coefficients B: \n" << distCoeffsB << std::endl;

    for (size_t i = 0; i < imagesA.size(); i++) {
        cv::Mat R_A, R_B;
        // Convert rotation vectors to rotation matrices
        cv::Rodrigues(rvecsA[i], R_A);
        cv::Rodrigues(rvecsB[i], R_B);

        // ��ӡ��Σ���ת�����ƽ��������
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

    // ������ͶӰ���
    double totalReprojectionError = 0.0;
    int totalPoints = 0;

    for (size_t i = 0; i < imagesA.size(); i++) {
        std::vector<cv::Point2f> projectedPointsA, projectedPointsB;

        // ��3D��ͶӰ��ͼ��ƽ��
        cv::projectPoints(points3D, rvecsA[i], tvecsA[i], cameraMatrixA, distCoeffsA, projectedPointsA);
        cv::projectPoints(points3D, rvecsB[i], tvecsB[i], cameraMatrixB, distCoeffsB, projectedPointsB);

        // ������ͶӰ���
        for (size_t j = 0; j < imagePointsA[i].size(); j++) {
            double errorA = cv::norm(imagePointsA[i][j] - projectedPointsA[j]);
            double errorB = cv::norm(imagePointsB[i][j] - projectedPointsB[j]);
            totalReprojectionError += (errorA + errorB)/12/11;
            totalPoints += 2;  // ÿ��ͼ����Ӧ������Camera A �� Camera B��
        }
    }

    double meanReprojectionError = totalReprojectionError / totalPoints;

    // �������ķ���
    double variance = 0.0;
    for (size_t i = 0; i < imagesA.size(); i++) {
        std::vector<cv::Point2f> projectedPointsA, projectedPointsB;

        // ��3D��ͶӰ��ͼ��ƽ��
        cv::projectPoints(points3D, rvecsA[i], tvecsA[i], cameraMatrixA, distCoeffsA, projectedPointsA);
        cv::projectPoints(points3D, rvecsB[i], tvecsB[i], cameraMatrixB, distCoeffsB, projectedPointsB);

        // ���������
        for (size_t j = 0; j < imagePointsA[i].size(); j++) {
            double errorA = cv::norm(imagePointsA[i][j] - projectedPointsA[j]);
            double errorB = cv::norm(imagePointsB[i][j] - projectedPointsB[j]);
            double error = (errorA + errorB) / 2;  // ƽ�����
            variance += (error - meanReprojectionError) * (error - meanReprojectionError);
        }
    }

    variance /= totalPoints;  // ���㷽��

    std::cout << "Mean Reprojection Error: " << meanReprojectionError << std::endl;
    std::cout << "Variance of Reprojection Error: " << variance << std::endl;

    return 0;
}
