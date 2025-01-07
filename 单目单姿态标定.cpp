#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>

int main() {
    // 1. ����ͼ��ͱ궨�����������
    std::vector<cv::Point3f> objectPoints;
    float squareSize = 3.0;  // �궨���ÿ������߳�����λ��mm��

    // ����궨����12x11�����飨ͨ���궨��Ľǵ���Ϊ12x11��
    for (int i = 0; i < 11; ++i) {  // 11��
        for (int j = 0; j < 12; ++j) {  // 12��
            objectPoints.push_back(cv::Point3f(j * squareSize, i * squareSize, 0.0f));
        }
    }

    // 2. ����ͼ�񲢼��ǵ�
    cv::Mat image = cv::imread("E:\\һЩ����Matlab\\����У׼\\20241201\\calibration_images\\CameraA\\9.bmp");

    if (image.empty()) {
        std::cerr << "ͼ�����ʧ��!" << std::endl;
        return -1;
    }

    // ת��Ϊ�Ҷ�ͼ��
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    std::vector<cv::Point2f> imagePoints;
    bool found = cv::findChessboardCorners(grayImage, cv::Size(12, 11), imagePoints);

    if (!found) {
        std::cerr << "�궨��ǵ�δ�ܼ�⵽!" << std::endl;
        return -1;
    }

    // 3. ��߽ǵ��⾫��
    cv::cornerSubPix(grayImage, imagePoints, cv::Size(11, 11), cv::Size(-1, -1),
        cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.1));

    // 4. ��objectPoints��imagePointsת��Ϊcv::Mat��ʽ
    std::vector<std::vector<cv::Point3f>> objectPointsVec(1, objectPoints);
    std::vector<std::vector<cv::Point2f>> imagePointsVec(1, imagePoints);

    // 5. ����ڲκͻ���ϵ����ʼ��
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);  // ��ʼ������ڲξ���
    cv::Mat distCoeffs = cv::Mat::zeros(1, 5, CV_64F);  // ��ʼ������ϵ��

    // 6. ʹ��calibrateCamera�������ڲκ����
    cv::Mat rvecs, tvecs;
    double rms = cv::calibrateCamera(objectPointsVec, imagePointsVec, image.size(),
        cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "��ͶӰ�� " << rms << std::endl;

    // �������ڲΡ�����ϵ�������
    std::cout << "����ڲξ���" << std::endl << cameraMatrix << std::endl;
    std::cout << "����ϵ����" << std::endl << distCoeffs << std::endl;

    // ��ӡ��Σ���ת������ƽ��������
    std::cout << "��Σ���ת�������� " << std::endl << rvecs << std::endl;
    std::cout << "��Σ�ƽ���������� " << std::endl << tvecs << std::endl;

    // ����ת����ת��Ϊ��ת����
    cv::Mat R;
    cv::Rodrigues(rvecs, R);
    std::cout << "��ת���� " << std::endl << R << std::endl;

    return 0;
}
