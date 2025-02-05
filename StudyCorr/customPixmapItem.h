#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include "Drawable.h"

class CustomPixmapItem : public QGraphicsPixmapItem {
public:
    explicit CustomPixmapItem(QGraphicsItem* parent = nullptr);
    void setDrawable(Drawable* drawable);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    Drawable* drawable = nullptr;
};