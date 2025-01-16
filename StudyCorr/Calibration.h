#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include<QStringList>
#include<QDebug>

struct cameraParameter
{
    cv::Size boardSize;
    float squareSize;
    std::vector<cv::Point3f> objectPoints;
    std::vector<std::vector<cv::Point3f>> objectPointsVec;
    std::vector<std::vector<cv::Point2f>> imagePointsVec;
    std::vector< cv::Mat> projMatrixVec;
    std::vector< std::string> chessPath;
};

class ChessCalibration
{
public:
    ChessCalibration(int rows,int cols,float squareSize, QStringList chessPath);
    ChessCalibration(int rows, int cols, float squareSize, QStringList chessPathLeft, QStringList chessPathRight);
    ~ChessCalibration();
    bool prefareMonocularCalibration();
    void startMonocularCompute();
    cv::Mat computeProjMatrix(const cv::Mat R, const cv::Mat T, const cv::Mat cameraMatrix) const;//重投影矩阵
    bool prefareStereoCalibration();
    void startStereoCalibration();
    void drawCornersAndAxes(cv::Mat& img, const std::vector<cv::Point2f>& corners, cv::Size boardSize, bool found);
    std::vector<cv::Mat> img1_frames;//用于储存drawCornersAndAxes绘制的图片
    std::vector<cv::Mat> img2_frames;
    cv::Mat cameraMatrix1, cameraMatrix2;
    cv::Mat distCoeffs1, distCoeffs2;
    std::vector<cv::Mat> rvecs1, tvecs1, rvecs2, tvecs2;
    cv::Mat R, T, E, F;
private:
    cv::Size boardSize;
    float squareSize;
    std::vector<cv::Point3f> objectPoints;
    std::vector<std::vector<cv::Point3f>> objectPointsVec;
    std::vector<std::vector<cv::Point2f>> imagePointsVec1, imagePointsVec2;
    std::vector< cv::Mat> projMatrixVec1, projMatrixVec2;
    std::vector< std::string> chessPath, chessPathLeft, chessPathRight;
};

