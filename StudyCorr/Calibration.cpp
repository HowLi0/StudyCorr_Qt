#include "Calibration.h"

    ChessCalibration::ChessCalibration(int rows, int cols, float squareSize, QStringList chessPath)
    {
        this->boardSize = cv::Size(rows,cols);
        this->squareSize = squareSize;
        // 将 QStringList 转换为 std::vector<std::string>
        for (const QString& path : chessPath) {
            this->chessPath.push_back(path.toStdString());
        }
    }

    ChessCalibration::ChessCalibration(int rows, int cols, float squareSize, QStringList chessPathLeft, QStringList chessPathRight)
    {
        this->boardSize = cv::Size(rows, cols);
        this->squareSize = squareSize;
        // 将 QStringList 转换为 std::vector<std::string>
        for (const QString& path : chessPathLeft) {
            this->chessPathLeft.push_back(path.toUtf8().constData());
           // std::cout << path.toUtf8().constData() << std::endl;
        }
        for (const QString& path : chessPathRight) {
            this->chessPathRight.push_back(path.toUtf8().constData());
        }
    }

    ChessCalibration::~ChessCalibration()
    {
    }

    bool ChessCalibration::prefareMonocularCalibration()
    {
        if (chessPath.size()==0)
        {
            std::cerr << "There is nothing for calibration." << std::endl;
            return false;
        }

        for (int i = 0; i < boardSize.height; ++i)
        {
            for (int j = 0; j < boardSize.width; ++j)
            { 
                this->objectPoints.emplace_back(j * squareSize, i * squareSize, 0.00f);
            }
        }
        this->objectPointsVec = std::vector<std::vector<cv::Point3f>> (chessPath.size(), objectPoints);
        for (int i = 0; i < chessPath.size(); ++i)
        {
            cv::Mat img1 = cv::imread(chessPath[i]);
            cv::Mat gray1;
            cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);

            std::vector<cv::Point2f> corners1;
            bool found = cv::findChessboardCorners(gray1, boardSize, corners1);

            if (found)
            {
                cv::cornerSubPix(gray1, corners1, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));
                this->imagePointsVec1.push_back(corners1);
                drawCornersAndAxes(img1, corners1, boardSize, found);
                this->img1_frames.push_back(img1);
            }
            else
            {
                if (!found)
                {
                    std::cerr << "Chessboard corners not found in image: " << chessPath[i] << std::endl;
                }
            }
        }
        return true;
    }

    void ChessCalibration::startMonocularCompute()
    {
        if (imagePointsVec1.empty())
        {
            std::cerr << "No valid chessboard corners were found in the provided images." << std::endl;
            exit(1);
        }

        this->cameraMatrix1 = cv::Mat::eye(3, 3, CV_64F);

        // Camera calibration for both cameras
        double rms = cv::calibrateCamera(objectPointsVec, imagePointsVec1, cv::Size(img1_frames[0].cols, img1_frames[0].rows), cameraMatrix1, distCoeffs1, rvecs1, tvecs1);
        std::cout << "RMS error of camera： " << rms << std::endl;

        // 输出相机内参、畸变系数、外参
        std::cout << "Camera Matrix：" << std::endl << cameraMatrix1 << std::endl;
        std::cout << "Distortion Coefficients：" << std::endl << distCoeffs1 << std::endl;
        for (int i = 0; i < chessPath.size(); i++)
        {
            // 将旋转向量转换为旋转矩阵
            cv::Mat R ,T;
            cv::Rodrigues(rvecs1[i], R);
            T = tvecs1[i];
            std::cout << "Rotation Matrix: " << std::endl << R << std::endl;
            std::cout << "Translation Vector: " << std::endl << tvecs1 [i]<< std::endl;
            cv::Mat projMatrix=computeProjMatrix(R, T, cameraMatrix1);
            this->projMatrixVec1.push_back(projMatrix);
        }
        std::cout << "compute is over"<< std::endl;
    }

    cv::Mat ChessCalibration::computeProjMatrix(const cv::Mat R, const cv::Mat T, const cv::Mat cameraMatrix) const
    {
        // Construct projection matrix for Camera : P = K * [R | T]
        cv::Mat projMatrix = cv::Mat::zeros(3, 4, CV_64F);
        cv::Mat RT;
        cv::hconcat(R, T, RT);  // Combine R and T into a 3x4 matrix
        projMatrix = cameraMatrix * RT;
        return projMatrix;
    }


    bool ChessCalibration::prefareStereoCalibration()
    {
        if (chessPathLeft.size() != chessPathRight.size()) {
            std::cerr << "The number of images from Camera1 and Camera2 should be the same." << std::endl;
            return false;
        }

        for (int i = 0; i < boardSize.height; ++i) {
            for (int j = 0; j < boardSize.width; ++j) {
                this->objectPoints.emplace_back(j * squareSize, i * squareSize, 0.00f);
            }
        }

        this->objectPointsVec = std::vector<std::vector<cv::Point3f>>(chessPathLeft.size(), objectPoints);

        for (int i = 0; i < chessPathLeft.size(); ++i) {
            cv::Mat img1 = cv::imread(chessPathLeft[i]);
            cv::Mat img2 = cv::imread(chessPathRight[i]);

            if (img1.empty() || img2.empty()) {
                std::cerr << "Failed to load images: " << chessPathLeft[i] << ", " << chessPathRight[i] << std::endl;
                return false;
            }

            cv::Mat gray1, gray2;
            cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
            cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

            std::vector<cv::Point2f> corners1, corners2;
            bool found1 = cv::findChessboardCorners(gray1, boardSize, corners1,
                cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
            bool found2 = cv::findChessboardCorners(gray2, boardSize, corners2,
                cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

            if (found1 && found2) {
                cv::cornerSubPix(gray1, corners1, cv::Size(11, 11), cv::Size(-1, -1),
                    cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));
                cv::cornerSubPix(gray2, corners2, cv::Size(11, 11), cv::Size(-1, -1),
                    cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));

                this->imagePointsVec1.push_back(corners1);
                this->imagePointsVec2.push_back(corners2);

                drawCornersAndAxes(img1, corners1, boardSize, found1);
                drawCornersAndAxes(img2, corners2, boardSize, found2);

                this->img1_frames.push_back(img1);
                this->img2_frames.push_back(img2);
            }
        }
        return true;
    }

    void ChessCalibration::startStereoCalibration()
    {
        if (imagePointsVec1.empty() || imagePointsVec2.empty()) {
            std::cerr << "No valid chessboard corners were found in the provided images." << std::endl;
            return;
        }

        double rms1 = cv::calibrateCamera(objectPointsVec, imagePointsVec1, img1_frames[0].size(), cameraMatrix1, distCoeffs1, rvecs1, tvecs1);
        double rms2 = cv::calibrateCamera(objectPointsVec, imagePointsVec2, img2_frames[0].size(), cameraMatrix2, distCoeffs2, rvecs2, tvecs2);

        cv::Mat R, T, E, F;
        double  rms= cv::stereoCalibrate(objectPointsVec, imagePointsVec1, imagePointsVec2,
            cameraMatrix1, distCoeffs1,
            cameraMatrix2, distCoeffs2,
            img1_frames[0].size(), R, T, E, F,
            cv::CALIB_FIX_INTRINSIC);

        std::cout << "RMS error of camera1: " << rms1 << std::endl;
        std::cout << "RMS error of camera2: " << rms2 << std::endl;
        std::cout << "Stereo calibration RMS error:" << rms << std::endl;

        for (int i = 0; i < chessPathLeft.size(); i++)
        {
            // 将旋转向量转换为旋转矩阵
            cv::Mat R1, R2,T1,T2;
            cv::Rodrigues(rvecs1[i], R1);
            cv::Rodrigues(rvecs2[i], R2);
            T1 = tvecs1[i];
            T2 = tvecs2[i];
            cv::Mat projMatrix1 = computeProjMatrix(R1, T1, cameraMatrix1);
            cv::Mat projMatrix2 = computeProjMatrix(R2, T2, cameraMatrix2);
            this->projMatrixVec1.push_back(projMatrix1);
            this->projMatrixVec2.push_back(projMatrix2);
        }
    }


    void ChessCalibration::drawCornersAndAxes(cv::Mat& img, const std::vector<cv::Point2f>& corners, cv::Size boardSize, bool found)
    {
        if (found) {
            // 在角点处画空心圆
            for (const auto& corner : corners) {
                cv::circle(img, corner, 15, cv::Scalar(0, 0, 255), 2); // 红色空心圆圈，半径为15，线宽为2
            }

            // 计算第一个角点的位置，作为原点
            cv::Point2f origin = corners[0];

            // 计算X轴和Y轴的方向
            cv::Point2f xAxis = corners[3] - corners[0];
            cv::Point2f yAxis = corners[3 * boardSize.width] - corners[0];

            // 绘制X轴（蓝色）并添加箭头和标签
            cv::arrowedLine(img, origin, origin + xAxis, cv::Scalar(255, 0, 0), 4);
            cv::putText(img, "X", origin + xAxis * 1.1, cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255, 0, 0), 4);

            // 绘制Y轴（绿色）并添加箭头和标签
            cv::arrowedLine(img, origin, origin + yAxis, cv::Scalar(0, 255, 0), 8);
            cv::putText(img, "Y", origin + yAxis * 1.1, cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 0), 4);

            // 绘制原点（白色空心圆）
            cv::circle(img, origin, 15, cv::Scalar(255, 255, 255), 2); // 白色空心圆圈，半径为15，线宽为2
        }
    }
