﻿#pragma once

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
    static int CalibrationIndex;
    static int ComputeIndex;

private:
    CalibrationDialog* calibrationDialog;
    ComputeDialog* computeDialog;
    QStringList CalibrationFileNames;//储存标定文件信息
    QStringList ComputeFileNames;//储存散斑文件信息
    QStringList LeftCameraFilePath;
    QStringList RightCameraFilePath;
    ChessCalibration* chessCalibration;
    //****************************************************工作区控件****************************************************//
    QTabWidget* TabWidget;
    QTreeWidget* TreeWidget1;
    QTreeWidget* TreeWidget2;
    //****************************************************状态栏****************************************************//
    QAction* CalibrationButton;
    QAction* ComputeButton;
    QToolBar* chessToolBar;
    QSpinBox* squareSizeSpinBox;
    QSpinBox* rowsSpinBox;
    QToolBar* computeToolBar;
    QAction* rectAction;
    QAction* circleAction ;
    QAction* polygonAction ;
    QAction* cropPolygonAction ;
    QAction* dragROIAction ;
    QAction* deleteAction;
    QAction* seedPoints;
    QAction* autoROI ;
    QSpinBox* stepSizeSpinBox;
    QSpinBox* subSizeSpinBox;
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
    // cv::Mat 转换为 QImage
    QImage cvMatToQImage(const cv::Mat& mat);

    //****************************************************ROI****************************************************//
};