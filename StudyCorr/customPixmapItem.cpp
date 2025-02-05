#include "customPixmapItem.h"

CustomPixmapItem::CustomPixmapItem(QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent), drawable(nullptr) {
    setAcceptHoverEvents(true); // 必须启用悬停事件
    setFlag(QGraphicsItem::ItemIsFocusable); // 启用焦点
}

void CustomPixmapItem::setDrawable(Drawable* drawable) {
    this->drawable = drawable;
}

void CustomPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (drawable) {
        drawable->handleMousePress(event);
        update();
        event->accept();
    }
    else {
        QGraphicsPixmapItem::mousePressEvent(event);
    }
}

void CustomPixmapItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (drawable) {
        drawable->handleMouseMove(mapToScene(event->pos()));
        update();
    }
    QGraphicsPixmapItem::hoverMoveEvent(event);
}

void CustomPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (drawable) {
        drawable->handleMouseRelease(event);
        update();
        event->accept();
    }
    else {
        QGraphicsPixmapItem::mouseReleaseEvent(event);
    }
}