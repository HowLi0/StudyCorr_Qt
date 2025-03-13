#pragma once

#include "ui_StudyCorr.h"
#include "CalibrationDialog.h"
#include "ComputeDialog.h"
#include "Calibration.h"
#include"customPixmapItem.h"
#include<QtGui>
#include<QtCore>
#include <QtWidgets>


class StudyCorr : public QMainWindow
{
    Q_OBJECT

public:
    StudyCorr(QWidget* parent = nullptr);
    ~StudyCorr();
	static int CalibrationIndex;//标定索引,用于区分不同的标定
	static int ComputeIndex;//计算索引,用于区分不同的计算

private:
    CalibrationDialog* calibrationDialog = nullptr;
    ComputeDialog* computeDialog = nullptr;
    QStringList LeftCameraFilePath;//储存标定文件信息
    QStringList RightCameraFilePath;
    QMap<int, QPair<QStringList, QStringList>> calibrationImageFiles;
    QStringList LeftComputeFilePath;//储存散斑文件信息
    QStringList RightComputeFilePath;
    QMap<int, QPair<QStringList, QStringList>> computeImageFiles;
    ChessCalibration* chessCalibration = nullptr;
    //****************************************************工作区控件****************************************************//
    QTabWidget* TabWidget = nullptr;
    QTreeWidget* TreeWidget1 = nullptr;
    QTreeWidget* TreeWidget2 = nullptr;
    //****************************************************状态栏****************************************************//
    QAction* CalibrationButton = nullptr;
    QAction* ComputeButton = nullptr;
    QToolBar* chessToolBar = nullptr;
    QSpinBox* squareSizeSpinBox = nullptr;
    QSpinBox* rowsSpinBox = nullptr;
    QToolBar* computeToolBar = nullptr;
    QAction* rectAction = nullptr;
    QAction* circleAction = nullptr;
    QAction* polygonAction = nullptr;
    QAction* cropPolygonAction = nullptr;
    QAction* dragROIAction = nullptr;
    QAction* deleteAction = nullptr;
    QAction* seedPoints = nullptr;
    QAction* autoROI = nullptr;
    QSpinBox* stepSizeSpinBox = nullptr;
    QSpinBox* subSizeSpinBox = nullptr;
    bool hasRunCalibrationToolbars = false;	// 添加一个布尔变量，确保只调用一次
    bool hasRunComputeToolbars = false;	// 添加一个布尔变量，确保只调用一次
    //****************************************************图像显示****************************************************//
    int currentFrameIndex = 0;  // 当前显示的帧索引
    QTimer* timer = nullptr;
    //view<<scence<<item
    // 创建一个 QGraphicsView 以显示场景
    QGraphicsView* view1 = nullptr;;
    QGraphicsView* view2 = nullptr;;
    // 创建一个 QGraphicsScene 以包含图像
    QGraphicsScene* scene1 = nullptr; ;
    QGraphicsScene* scene2 = nullptr; ;
    // 加载图像并添加到场景中
    CustomPixmapItem* item1 = nullptr; ;
    CustomPixmapItem* item2 = nullptr; ;
    //显示图像名称
    QGraphicsTextItem* img1TextItem = nullptr;
    QGraphicsTextItem* img2TextItem = nullptr;

    Drawable* drawable1;  // 用于绘制的对象


private:
    Ui::StudyCorrClass ui;
    void SetupUi(int CalibrationIndex, int ComputeIndex);
    void CreateNewProject();
    void OpenExistingProject();
    //****************************************************标定****************************************************//
    void CalibrationButtonClicked(int CalibrationIndex);
    void CalibrationOKButtonClicked();
    void CalibrationToolBar();
    void ChessToolBar();
    //****************************************************计算****************************************************//
    void ComputeButtonClicked(int ComputeIndex);
    void ComputeOKButtonClicked();
    void ComputeToolBar();

    //****************************************************绘图****************************************************//
    void displayImages(const  cv::Mat& img);
    void displayImages(const  cv::Mat& img1, const  cv::Mat& img2);
    void displayImages(const  QPixmap& img1, const  QPixmap& img2);
    QImage cvMatToQImage(const cv::Mat& mat);

    //****************************************************ROI****************************************************//
    void updateROICalculationPoints();
};