#include "Drawable.h"

Drawable::Drawable(QGraphicsPixmapItem* pixmapItem) : pixmapItem(pixmapItem), shapeDrawer(nullptr) {}

void Drawable::setDrawMode(DrawMode mode) {
    if (mode == drawMode) return;
    resetDrawing();
    drawMode = mode;
    switch (drawMode) {
    case Rectangle:
        shapeDrawer = new RectangleDrawer(pixmapItem);
        break;
    case Circle:
        shapeDrawer = new CircleDrawer(pixmapItem);
        break;
    case Polygon:
        shapeDrawer = new PolygonDrawer(pixmapItem);
        break;
    case ClipPolygon:  
        shapeDrawer = new ClipPolygonDrawer(pixmapItem);
        break;
    case Delete:      
        shapeDrawer = new DeleteDrawer(pixmapItem);
        break;
    case Drag:
        shapeDrawer = new DragDrawer(pixmapItem);
        break;
    default:
        shapeDrawer = nullptr;
        break;
    }
}

void Drawable::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (shapeDrawer) shapeDrawer->handleMousePress(event);
}

void Drawable::handleMouseMove(const QPointF& scenePos) {
    if (shapeDrawer) shapeDrawer->handleMouseMove(scenePos);
}

void Drawable::handleMouseRelease(QGraphicsSceneMouseEvent* event) {
    if (shapeDrawer) shapeDrawer->handleMouseRelease(event);
}

void Drawable::resetDrawing() {
    delete shapeDrawer;
    shapeDrawer = nullptr;
}