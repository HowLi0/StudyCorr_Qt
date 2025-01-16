#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>

using namespace cv;
using namespace std;

class ImageWidget : public QLabel {
    Q_OBJECT
public:
    ImageWidget(const Mat& image, QWidget* parent = nullptr) : QLabel(parent), image(image.clone()) {
        mask = Mat::zeros(image.size(), CV_8UC1);
        displayImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8);
        setPixmap(QPixmap::fromImage(displayImage));
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            // 使用 position() 获取浮点型坐标并转换为整数型 QPoint
            QPointF pos = event->position();
            Point seed(static_cast<int>(pos.x()), static_cast<int>(pos.y()));

            seeds.push_back(seed);
            regionGrow(image, mask, seed, 20);
            updateDisplay();
        }
    }

private:
    Mat image;
    Mat mask;
    QImage displayImage;
    vector<Point> seeds;

    void regionGrow(const Mat& image, Mat& mask, Point seed, int threshold) {
        vector<Point> directions = { Point(-1, 0), Point(1, 0), Point(0, -1), Point(0, 1),
                                     Point(-1, -1), Point(-1, 1), Point(1, -1), Point(1, 1) };
        queue<Point> toVisit;
        toVisit.push(seed);

        while (!toVisit.empty()) {
            Point current = toVisit.front();
            toVisit.pop();

            for (const auto& dir : directions) {
                Point neighbor = current + dir;
                if (neighbor.x >= 0 && neighbor.x < image.cols && neighbor.y >= 0 && neighbor.y < image.rows) {
                    if (mask.at<uchar>(neighbor) == 0 &&
                        abs(image.at<uchar>(neighbor) - image.at<uchar>(seed)) <= threshold) {
                        mask.at<uchar>(neighbor) = 255;
                        toVisit.push(neighbor);
                    }
                }
            }
        }
    }

    void updateDisplay() {
        Mat visualization;
        cvtColor(image, visualization, COLOR_GRAY2BGR);
        for (int y = 0; y < mask.rows; ++y) {
            for (int x = 0; x < mask.cols; ++x) {
                if (mask.at<uchar>(y, x) == 255) {
                    visualization.at<Vec3b>(y, x) = Vec3b(0, 255, 0);
                }
            }
        }
        QImage updatedImage(visualization.data, visualization.cols, visualization.rows,
            visualization.step, QImage::Format_BGR888);
        setPixmap(QPixmap::fromImage(updatedImage));
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Mat image = imread("../2.bmp", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cerr << "无法加载图像!" << endl;
        return -1;
    }

    ImageWidget widget(image);
    widget.show();

    return app.exec();
}

