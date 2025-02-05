#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include "Shape.h"

class Drawable : public QObject {
    Q_OBJECT
public:
    enum DrawMode { None, Rectangle, Circle, Polygon, ClipPolygon, Delete, Drag};
    explicit Drawable(QGraphicsPixmapItem* pixmapItem);
    void setDrawMode(DrawMode mode);
public slots:
    void handleMousePress(QGraphicsSceneMouseEvent* event);
    void handleMouseMove(const QPointF& scenePos);
    void handleMouseRelease(QGraphicsSceneMouseEvent* event);
private:
    void resetDrawing();
    QGraphicsPixmapItem* pixmapItem;
    DrawMode drawMode = None;
    ShapeDrawer* shapeDrawer = nullptr;
};