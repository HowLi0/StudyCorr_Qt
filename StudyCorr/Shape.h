﻿#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QPen>
#include<QPainter>
#include <QDebug>
#include <QtMath>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QPainterPath>

class ShapeDrawer {
public:
    virtual ~ShapeDrawer() = default;
    virtual void handleMousePress(QGraphicsSceneMouseEvent* event) = 0;
    virtual void handleMouseMove(const QPointF& scenePos) = 0;
    virtual void handleMouseRelease(QGraphicsSceneMouseEvent* event) = 0;
    virtual void updateCalculationPoints(int stepSize, int subSize) = 0;
};

// 矩形绘制器
class RectangleDrawer : public ShapeDrawer {
public:
    RectangleDrawer(QGraphicsPixmapItem* parent);
    void handleMousePress(QGraphicsSceneMouseEvent* event) override;
    void handleMouseMove(const QPointF& scenePos) override;
    void handleMouseRelease(QGraphicsSceneMouseEvent* event) override {};
    void updateCalculationPoints(int stepSize, int subSize) override; 
    QVector<QGraphicsRectItem*> finalRects; // 存储多个最终绘制的矩形
private:
	QPixmap originalPixmap;
    QGraphicsPixmapItem* pixmapItem;
    QPointF startPoint;
    QGraphicsRectItem* previewRect = nullptr;
    bool isDrawing = false;
};

// 圆形绘制器
class CircleDrawer : public ShapeDrawer {
public:
    CircleDrawer(QGraphicsPixmapItem* parent);
    void handleMousePress(QGraphicsSceneMouseEvent* event) override;
    void handleMouseMove(const QPointF& scenePos) override;
    void handleMouseRelease(QGraphicsSceneMouseEvent* event) override {};
    void updateCalculationPoints(int stepSize, int subSize) override;
    QVector<QGraphicsEllipseItem*> finalCircles; // 存储多个最终绘制的圆形
private:
    QPixmap originalPixmap;
    QGraphicsPixmapItem* pixmapItem;
    QList<QPointF> circlePoints;
    QGraphicsEllipseItem* previewCircle = nullptr;
    void updateRadiusPreview(const QPointF& endPos);
    void updateCircumcirclePreview(const QPointF& thirdPos);
    void createFinalCircle();
    bool calculateCircumcircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, qreal& radius);
    void reset();
};

// 多边形绘制器
class PolygonDrawer : public ShapeDrawer {
public:
    PolygonDrawer(QGraphicsPixmapItem* parent);
    void handleMousePress(QGraphicsSceneMouseEvent* event) override;
    void handleMouseMove(const QPointF& scenePos) override;
    void handleMouseRelease(QGraphicsSceneMouseEvent* event) override;
    void updateCalculationPoints(int stepSize, int subSize) override;
    QVector<QGraphicsPolygonItem*> finalPolygons; // 存储多个最终绘制的多边形
private:
    QPixmap originalPixmap;
    QGraphicsPixmapItem* pixmapItem;
    QPolygonF currentPolygon;
    QGraphicsPolygonItem* currentPolygonItem = nullptr;
    QGraphicsPolygonItem* targetPolygon = nullptr;
    void updatePolygonPreview(const QPointF& scenePos);
    void finalizePolygon();
    void reset();
};

// 裁剪多边形绘制器
class ClipPolygonDrawer : public ShapeDrawer {
public:
    ClipPolygonDrawer(QGraphicsPixmapItem* parent);
    void handleMousePress(QGraphicsSceneMouseEvent* event) override;
    void handleMouseMove(const QPointF& scenePos) override;
    void handleMouseRelease(QGraphicsSceneMouseEvent* event) override;
    void updateCalculationPoints(int stepSize, int subSize) override;
    QVector<QGraphicsPathItem*> finalClipPolygons; // 存储多个最终绘制的裁剪多边形
private:
    QPixmap originalPixmap;
    QGraphicsPixmapItem* pixmapItem;
    QPolygonF clipPolygon;
    QGraphicsPolygonItem* previewClipItem = nullptr;
    void updateClipPreview(const QPointF& scenePos);
    void finalizeClip();
    void reset();
};

// 删除绘制器
class DeleteDrawer : public ShapeDrawer {
public:
    DeleteDrawer(QGraphicsPixmapItem* parent);
    void handleMousePress(QGraphicsSceneMouseEvent* event) override;
    void handleMouseMove(const QPointF&) override {}
    void handleMouseRelease(QGraphicsSceneMouseEvent*) override {}
    void updateCalculationPoints(int stepSize, int subSize) override {};
private:
    QGraphicsPixmapItem* pixmapItem;
};

// 拖拽绘制器
class DragDrawer : public ShapeDrawer {
public:
    DragDrawer(QGraphicsPixmapItem* parent);
    void handleMousePress(QGraphicsSceneMouseEvent* event) override;
    void handleMouseMove(const QPointF& scenePos) override;
    void handleMouseRelease(QGraphicsSceneMouseEvent* event) override;
    void updateCalculationPoints(int stepSize, int subSize) override;
private:
    QGraphicsPixmapItem* pixmapItem;
    QGraphicsItem* draggedItem = nullptr;  // 当前被拖拽的图形项
    QPointF dragStartPos;                  // 拖拽起始位置（场景坐标）
    QPointF itemStartPos;                  // 图形项原始位置（父项坐标）
};