#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_StudyCorr.h"
#include "CalibrationDialog.h"
#include "ComputeDialog.h"
#include<QMenu>
#include<QToolBar>
#include<QDebug>
#include<QPushButton>
#include<QStatusBar>
#include<QLabel>
#include<QDockWidget>
#include <QGraphicsView>
#include <QHeaderView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QHBoxLayout>
#include<QPainter>//画家类
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include<QTreeWidget>
#include<QSpinBox>


class StudyCorr : public QMainWindow
{
    Q_OBJECT

public:
    StudyCorr(QWidget *parent = nullptr);
    ~StudyCorr();
    static int CalibrationIndex;
    static int ComputeIndex;

 signals:
     void Calibration_signal();
     void Compute_signal();

private:
    QStringList CalibrationFileNames;//储存标定文件信息
    QStringList ComputeFileNames;//储存散斑文件信息
    QStringList LeftCameraFileNames;
    QStringList RightCameraFileNames;
    QTabWidget* TabWidget;
    QTreeWidget* TreeWidget1;
    QTreeWidget* TreeWidget2;
    CalibrationDialog *calibrationDialog;
    ComputeDialog* computeDialog;
    QAction* CalibrationButton;
    QAction* ComputeButton;
    QToolBar* chessToolBar;
    bool hasRunCalibrationToolbars = false;	// 添加一个布尔变量，确保只调用一次
    bool hasRunComputeToolbars = false;	// 添加一个布尔变量，确保只调用一次

private:
    Ui::StudyCorrClass ui;
    void SetupUi(int CalibrationIndex,int ComputeIndex);
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
};
