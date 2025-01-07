#include "Calibration.h"


    std::vector<cv::Mat> img1_frames;
    std::vector<cv::Mat> img2_frames;

    bool ChessCalibration::PerformStereoCalibration(const std::vector<std::string>& cameraPosition1Files, const std::vector<std::string>& cameraPosition2Files)
    {
        if (cameraPosition1Files.size() != cameraPosition2Files.size())
        {
            std::cerr << "The number of images from Camera1 and Camera2 should be the same." << std::endl;
            return false;
        }

        std::vector<std::vector<cv::Point2f>> imagePoints1, imagePoints2;
        cv::Size boardSize(11, 8); // Adjust to your chessboard size
        float squareSize = 3.0f; // Adjust to your chessboard square size

        std::vector<cv::Point3f> objectPoints;
        for (int i = 0; i < boardSize.height; ++i)
            for (int j = 0; j < boardSize.width; ++j)
                objectPoints.emplace_back(j * squareSize, i * squareSize, 0);

        std::vector<std::vector<cv::Point3f>> objectPointsVec(cameraPosition1Files.size(), objectPoints);

        for (size_t i = 0; i < cameraPosition1Files.size(); ++i)
        {
            cv::Mat img1 = cv::imread(cameraPosition1Files[i]);
            cv::Mat img2 = cv::imread(cameraPosition2Files[i]);

            if (img1.empty() || img2.empty())
            {
                std::cerr << "Failed to load images: " << cameraPosition1Files[i] << ", " << cameraPosition2Files[i] << std::endl;
                return false;
            }

            cv::Mat gray1, gray2;
            cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
            cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

            std::vector<cv::Point2f> corners1, corners2;
            bool found1 = cv::findChessboardCorners(gray1, boardSize, corners1);
            bool found2 = cv::findChessboardCorners(gray2, boardSize, corners2);

            if (found1 && found2)
            {
                cv::cornerSubPix(gray1, corners1, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));
                cv::cornerSubPix(gray2, corners2, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.01));
                imagePoints1.push_back(corners1);
                imagePoints2.push_back(corners2);

                drawCornersAndAxes(img1, corners1, boardSize, found1);
                drawCornersAndAxes(img2, corners2, boardSize, found2);

                // Store processed images
                img1_frames.push_back(img1);
                img2_frames.push_back(img2);
            }
            else
            {
                if (!found1)
                {
                    std::cerr << "Chessboard corners not found in image: " << cameraPosition1Files[i] << std::endl;
                }
                if (!found2)
                {
                    std::cerr << "Chessboard corners not found in image: " << cameraPosition2Files[i] << std::endl;
                }
            }
        }

        if (imagePoints1.empty() || imagePoints2.empty())
        {
            std::cerr << "No valid chessboard corners were found in the provided images." << std::endl;
            return false;
        }

        cameraMatrix1 = cv::Mat::eye(3, 3, CV_64F);
        cameraMatrix2 = cv::Mat::eye(3, 3, CV_64F);

        // Camera calibration for both cameras
        cv::Mat cameraMatrixA, distCoeffsA, cameraMatrixB, distCoeffsB;
        std::vector<cv::Mat> rvecsA, tvecsA, rvecsB, tvecsB;
        cv::calibrateCamera(objectPointsVec, imagePoints1, cv::Size(img1_frames[0].cols, img1_frames[0].rows), cameraMatrixA, distCoeffsA, rvecsA, tvecsA);
        cv::calibrateCamera(objectPointsVec, imagePoints2, cv::Size(img1_frames[0].cols, img1_frames[0].rows), cameraMatrixB, distCoeffsB, rvecsB, tvecsB);

        double rms = cv::stereoCalibrate(objectPointsVec, imagePoints1, imagePoints2,
            cameraMatrix1, distCoeffs1,
            cameraMatrix2, distCoeffs2,
            cv::Size(img1_frames[0].cols, img1_frames[0].rows), R, T, E, F,
            cv::CALIB_FIX_INTRINSIC,
            cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5));

        std::cout << "Stereo Calibration done with RMS error = " << rms << std::endl;

        // Evaluation of calibration result
        std::cout << "Evaluating stereo calibration results..." << std::endl;
        double totalError = 0.0;
        double err = 0.0;
        std::vector<cv::Point2f> imagePoints1Reproj, imagePoints2Reproj;
        std::cout << "Calibration error for each image:" << std::endl;

        for (size_t i = 0; i < objectPointsVec.size(); ++i)
        {
            // Reproject 3D points to both cameras
            cv::projectPoints(objectPointsVec[i], rvecsA[i], tvecsA[i], cameraMatrixA, distCoeffsA, imagePoints1Reproj);
            cv::projectPoints(objectPointsVec[i], rvecsB[i], tvecsB[i], cameraMatrixB, distCoeffsB, imagePoints2Reproj);

            // Calculate reprojection error for Camera 1
            cv::Mat imgPoints1Mat = cv::Mat(1, imagePoints1[i].size(), CV_32FC2);
            cv::Mat imagePoints1ReprojMat = cv::Mat(1, imagePoints1Reproj.size(), CV_32FC2);
            for (int j = 0; j < imagePoints1[i].size(); j++)
            {
                imagePoints1ReprojMat.at<cv::Vec2f>(0, j) = cv::Vec2f(imagePoints1Reproj[j].x, imagePoints1Reproj[j].y);
                imgPoints1Mat.at<cv::Vec2f>(0, j) = cv::Vec2f(imagePoints1[i][j].x, imagePoints1[i][j].y);
            }
            double error1 = cv::norm(imgPoints1Mat, imagePoints1ReprojMat, cv::NORM_L2);
            error1 /= imagePoints1[i].size();

            // Calculate reprojection error for Camera 2
            cv::Mat imgPoints2Mat = cv::Mat(1, imagePoints2[i].size(), CV_32FC2);
            cv::Mat imagePoints2ReprojMat = cv::Mat(1, imagePoints2Reproj.size(), CV_32FC2);
            for (int j = 0; j < imagePoints2[i].size(); j++)
            {
                imagePoints2ReprojMat.at<cv::Vec2f>(0, j) = cv::Vec2f(imagePoints2Reproj[j].x, imagePoints2Reproj[j].y);
                imgPoints2Mat.at<cv::Vec2f>(0, j) = cv::Vec2f(imagePoints2[i][j].x, imagePoints2[i][j].y);
            }
            double error2 = cv::norm(imgPoints2Mat, imagePoints2ReprojMat, cv::NORM_L2);
            error2 /= imagePoints2[i].size();

            // Compute average error for the image
            err = (error1 + error2) / 2.0;
            totalError += err;

            std::cout << "  Image " << i + 1 << " average error: " << err << " pixels" << std::endl;
        }

        // Calculate overall average reprojection error
        double meanError = totalError / objectPointsVec.size();
        std::cout << "Overall mean reprojection error: " << meanError << " pixels" << std::endl;

        std::cout << "Evaluation completed!" << std::endl;
        return true;
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
