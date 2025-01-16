#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>

using namespace cv;
using namespace std;

// 区域增长算法函数
void regionGrow(const Mat& image, Mat& mask, Point seed, int threshold, Mat& displayImage) {
    // 定义 8 邻域方向
    vector<Point> directions = {
        Point(-1, 0), Point(1, 0), Point(0, -1), Point(0, 1),
        Point(-1, -1), Point(-1, 1), Point(1, -1), Point(1, 1)
    };

    queue<Point> toVisit;
    toVisit.push(seed);

    int rows = image.rows;
    int cols = image.cols;

    while (!toVisit.empty()) {
        Point current = toVisit.front();
        toVisit.pop();

        for (const auto& dir : directions) {
            Point neighbor = current + dir;

            // 检查邻点是否在图像范围内
            if (neighbor.x >= 0 && neighbor.x < cols && neighbor.y >= 0 && neighbor.y < rows) {
                // 检查邻点是否已经在 mask 中
                if (mask.at<uchar>(neighbor) == 0) {
                    // 判断灰度值是否在阈值范围内
                    if (abs(image.at<uchar>(neighbor) - image.at<uchar>(seed)) <= threshold) {
                        mask.at<uchar>(neighbor) = 255;
                        toVisit.push(neighbor);
                    }
                }
            }
        }
    }

    // 实时显示区域增长的状态
    displayImage = image.clone(); // 创建一个克隆图像，不影响原图
    cvtColor(displayImage, displayImage, COLOR_GRAY2BGR); // 转为彩色图，便于可视化
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            if (mask.at<uchar>(y, x) == 255) {
                displayImage.at<Vec3b>(y, x) = Vec3b(0, 255, 0); // 绿色表示 ROI 区域
            }
        }
    }
}

// 鼠标点击回调函数
void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        vector<Point>* seeds = reinterpret_cast<vector<Point>*>(userdata);
        seeds->emplace_back(x, y);
        cout << "Seed point: (" << x << ", " << y << ")" << endl;
    }
}

// 主函数
int main(int argc, char** argv) {
    // 读取输入图像
    Mat image = imread("../2.bmp", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "无法加载图像!" << endl;
        return -1;
    }

    // 初始化区域增长的掩膜
    Mat roiMask = Mat::zeros(image.size(), CV_8UC1);

    // 定义种子点 (通过鼠标选取)
    vector<Point> seeds;

    // 显示图像并设置鼠标回调
    namedWindow("Select Seeds", WINDOW_NORMAL);
    // 设置窗口的大小（宽和高）
    int width = 800;
    int height = 600;
    cv::resizeWindow("Select Seeds", width, height);  // 设置显示窗口的大小
    imshow("Select Seeds", image);
    setMouseCallback("Select Seeds", onMouse, &seeds);

    cout << "Please select any seedpoints, click any key to end." << endl;

    Mat displayImage = image.clone(); // 克隆一份原始图像用于显示

    while (true) {
        // 如果种子点为空，则等待新的种子点
        if (!seeds.empty()) {
            Point seed = seeds.back(); // 获取最新选取的种子点

            // 确保种子点未被处理
            if (roiMask.at<uchar>(seed) == 0) {
                roiMask.at<uchar>(seed) = 255;
                regionGrow(image, roiMask, seed, 45, displayImage); // 执行区域增长
            }

            // 直接更新显示
            imshow("Select Seeds", displayImage);

            // 等待用户点击新的种子点
            seeds.clear(); // 清空种子点，准备下一次点击

            cout << "Please select another seed point or press Enter to end." << endl;
        }

        int key = waitKey(1); // 持续等待用户点击
        if (key == 13) { // 按下回车键结束
            break;
        }
    }

    return 0;
}
