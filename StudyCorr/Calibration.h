#pragma once
#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
class ChessCalibration
{
public:
    bool PerformStereoCalibration(const  std::vector< std::string>& cameraPosition1Files, const  std::vector< std::string>& cameraPosition2Files);
    void drawCornersAndAxes(cv::Mat& img, const std::vector<cv::Point2f>& corners, cv::Size boardSize, bool found);
    cv::Mat cameraMatrix1;
    cv::Mat cameraMatrix2;
    cv::Mat distCoeffs1, distCoeffs2;
    cv::Mat R, T, E, F;
};
#endif // CALIBRATION_H

