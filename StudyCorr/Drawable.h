#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>

class  ShapeDrawer;

class Drawable : public QObject {
    Q_OBJECT
public:
    enum DrawMode { None, Rectangle, Circle, Polygon, ClipPolygon, Delete, Drag};
    explicit Drawable(QGraphicsPixmapItem* pixmapItem);
    void setDrawMode(DrawMode mode);
    void handleMousePress(QGraphicsSceneMouseEvent* event);
    void handleMouseMove(const QPointF& scenePos);
    void handleMouseRelease(QGraphicsSceneMouseEvent* event);
    void resetDrawing();
    void updateCalculationPoints(int stepSize, int subSize);
    QVector<QPointF> getCalculationPoints() const; // 添加方法获取计算点
    QVector<QPointF> calculationPoints; // 存储所有 ROI 中的计算点
private:
    QGraphicsPixmapItem* pixmapItem;
    DrawMode drawMode = None;
    ShapeDrawer* shapeDrawer = nullptr;
};