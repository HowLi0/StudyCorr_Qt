#include "shape.h"

// 矩形绘制器实现
RectangleDrawer::RectangleDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {}

void RectangleDrawer::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    QPointF localPos = pixmapItem->mapFromScene(event->scenePos());

    if (!isDrawing) {
        startPoint = localPos;
        isDrawing = true;
        if (!previewRect) {
            previewRect = new QGraphicsRectItem(pixmapItem);
            previewRect->setPen(QPen(Qt::green, 2, Qt::DotLine));
        }
        previewRect->setRect(QRectF(startPoint, QSizeF(2, 2)));
    }
    else {
        if (previewRect) {
            QGraphicsRectItem* finalRect = new QGraphicsRectItem(previewRect->rect(), pixmapItem);
            finalRect->setPen(QPen(Qt::red, 2));
            delete previewRect;
            previewRect = nullptr;
        }
        isDrawing = false;
    }
}

void RectangleDrawer::handleMouseMove(const QPointF& scenePos) {
    if (!isDrawing || !previewRect) return;
    QRectF rect(startPoint, pixmapItem->mapFromScene(scenePos));
    rect = rect.intersected(pixmapItem->boundingRect());
    previewRect->setRect(rect.normalized());
}

// 圆形绘制器实现
CircleDrawer::CircleDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {}

void CircleDrawer::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    QPointF localPos = pixmapItem->mapFromScene(event->scenePos());
    circlePoints.append(localPos);

    if (circlePoints.size() == 3) {
        createFinalCircle();
        reset();
    }
}

void CircleDrawer::handleMouseMove(const QPointF& scenePos) {
    QPointF localPos = pixmapItem->mapFromScene(scenePos);
    if (circlePoints.size() == 1) {
        updateRadiusPreview(localPos);
    }
    else if (circlePoints.size() == 2) {
        updateCircumcirclePreview(localPos);
    }
}

void CircleDrawer::updateRadiusPreview(const QPointF& endPos)
{
    if (!previewCircle) {
        previewCircle = new QGraphicsEllipseItem(pixmapItem);
        previewCircle->setPen(QPen(Qt::green, 2, Qt::DotLine));
    }
    qreal radius = QLineF(circlePoints[0], endPos).length();
    previewCircle->setRect(circlePoints[0].x() - radius, circlePoints[0].y() - radius, radius * 2, radius * 2);
}

void CircleDrawer::updateCircumcirclePreview(const QPointF& thirdPos)
{
    QPointF center;
    qreal radius;
    if (calculateCircumcircle(circlePoints[0], circlePoints[1], thirdPos, center, radius)) {
        if (!previewCircle) {
            previewCircle = new QGraphicsEllipseItem(pixmapItem);
            previewCircle->setPen(QPen(Qt::red, 2, Qt::DotLine));
        }
        previewCircle->setRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
    }
}

void CircleDrawer::createFinalCircle()
{
    QPointF center;
    qreal radius;
    if (calculateCircumcircle(circlePoints[0], circlePoints[1], circlePoints[2], center, radius)) {
        QGraphicsEllipseItem* ellipseItem = new QGraphicsEllipseItem(center.x() - radius, center.y() - radius, radius * 2, radius * 2, pixmapItem);
        ellipseItem->setPen(QPen(Qt::red, 2));
    }
}

bool CircleDrawer::calculateCircumcircle(const QPointF& p1, const QPointF& p2, const QPointF& p3, QPointF& center, qreal& radius)
{
    qreal ax = p2.x() - p1.x();
    qreal ay = p2.y() - p1.y();
    qreal bx = p3.x() - p2.x();
    qreal by = p3.y() - p2.y();
    qreal det = ax * by - ay * bx;
    if (qFuzzyIsNull(det)) return false;
    center.setX((by * (ax * (p1.x() + p2.x()) + ay * (p1.y() + p2.y())) - ay * (bx * (p2.x() + p3.x()) + by * (p2.y() + p3.y()))) / (2 * det));
    center.setY((ax * (bx * (p2.x() + p3.x()) + by * (p2.y() + p3.y())) - bx * (ax * (p1.x() + p2.x()) + ay * (p1.y() + p2.y()))) / (2 * det));
    radius = QLineF(center, p1).length();
    return true;
}

void CircleDrawer::reset()
{
    circlePoints.clear();
    delete previewCircle;
    previewCircle = nullptr;
}

// 构造函数
PolygonDrawer::PolygonDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {}

void PolygonDrawer::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF localPos = pixmapItem->mapFromScene(event->scenePos());
        currentPolygon.append(localPos);
        updatePolygonPreview(localPos);
    }
    else if (event->button() == Qt::RightButton && currentPolygon.size() > 2) {
        currentPolygon.append(currentPolygon.first()); // 闭合多边形
        finalizePolygon();
        reset();
    }
}

void PolygonDrawer::handleMouseMove(const QPointF& scenePos) {
    if (!currentPolygon.isEmpty()) {
        updatePolygonPreview(pixmapItem->mapFromScene(scenePos));
    }
}

void PolygonDrawer::handleMouseRelease(QGraphicsSceneMouseEvent* event) {
    // 无需处理
}

void PolygonDrawer::updatePolygonPreview(const QPointF& scenePos) {
    if (!currentPolygonItem) {
        currentPolygonItem = new QGraphicsPolygonItem(pixmapItem);
        currentPolygonItem->setPen(QPen(Qt::green, 2, Qt::DotLine));
    }
    QPolygonF preview = currentPolygon;
    preview.append(scenePos);
    currentPolygonItem->setPolygon(preview);
}

void PolygonDrawer::finalizePolygon() {
    if (currentPolygon.size() < 3) return;

    QGraphicsPolygonItem* finalItem = new QGraphicsPolygonItem(currentPolygon, pixmapItem);
    finalItem->setPen(QPen(Qt::red, 2));
}

void PolygonDrawer::reset() {
    currentPolygon.clear();
    delete currentPolygonItem;
    currentPolygonItem = nullptr;
}

ClipPolygonDrawer::ClipPolygonDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {}

void ClipPolygonDrawer::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF localPos = pixmapItem->mapFromScene(event->scenePos());
        clipPolygon.append(localPos);
        updateClipPreview(localPos);
    }
    else if (event->button() == Qt::RightButton && clipPolygon.size() > 2) {
        finalizeClip();
        reset();
    }
}

void ClipPolygonDrawer::handleMouseMove(const QPointF& scenePos)
{
    if (!clipPolygon.isEmpty()) {
        updateClipPreview(pixmapItem->mapFromScene(scenePos));
    }
}

void ClipPolygonDrawer::handleMouseRelease(QGraphicsSceneMouseEvent* event)
{
}

void ClipPolygonDrawer::updateClipPreview(const QPointF& scenePos) {
    if (!previewClipItem) {
        previewClipItem = new QGraphicsPolygonItem(pixmapItem);
        previewClipItem->setPen(QPen(Qt::blue, 2, Qt::DotLine));
    }
    QPolygonF preview = clipPolygon;
    preview.append(scenePos);
    previewClipItem->setPolygon(preview);
}

void ClipPolygonDrawer::finalizeClip() {
    // 创建裁剪路径
    QPainterPath clipPath;
    clipPath.addPolygon(clipPolygon);

    // 遍历所有子项
    foreach(QGraphicsItem * item, pixmapItem->childItems()) {
        // 跳过自身预览项和 pixmapItem
        if (item == previewClipItem || item == pixmapItem) continue;

        // 获取原始形状的路径
        QPainterPath subjectPath = item->shape();
        // 计算交集路径
        QPainterPath intersected = subjectPath.intersected(clipPath);

        if (!intersected.isEmpty()) {
            // 创建新的路径项并继承原样式
            QGraphicsPathItem* newItem = new QGraphicsPathItem(intersected, pixmapItem);

            // 复制原项的样式
            if (auto shapeItem = dynamic_cast<QAbstractGraphicsShapeItem*>(item)) {
                newItem->setPen(shapeItem->pen());
                newItem->setBrush(shapeItem->brush());
            }

            // 删除原项
            delete item;
        }
    }
}

void ClipPolygonDrawer::reset() {
    clipPolygon.clear();
    delete previewClipItem;
    previewClipItem = nullptr;
}

DeleteDrawer::DeleteDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {}

void DeleteDrawer::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsScene* scene = pixmapItem->scene();
        if (scene) {
            QGraphicsItem* clickedItem = scene->itemAt(event->scenePos(), QTransform());
            if (clickedItem && clickedItem != pixmapItem) {
                delete clickedItem;
            }
        }
        else {
            qDebug() << "Scene is not initialized!";
        }
    }
}

// 拖拽绘制器
DragDrawer::DragDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {}

void DragDrawer::handleMousePress(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QGraphicsScene* scene = pixmapItem->scene();
    if (!scene) return;

    // 获取点击的图形项（排除 pixmapItem）
    QGraphicsItem* clickedItem = scene->itemAt(event->scenePos(), QTransform());

    // 仅允许拖拽矩形、圆、多边形、裁剪后的图形
    if (clickedItem && clickedItem != pixmapItem &&
        (dynamic_cast<QGraphicsRectItem*>(clickedItem) ||
            dynamic_cast<QGraphicsEllipseItem*>(clickedItem) ||
            dynamic_cast<QGraphicsPolygonItem*>(clickedItem) ||
            dynamic_cast<QGraphicsPathItem*>(clickedItem)))
    {
        draggedItem = clickedItem;
        dragStartPos = event->scenePos();  // 记录拖拽起点（场景坐标）
        itemStartPos = draggedItem->scenePos(); // 记录图形项全局位置

        // 允许该图形项移动（如果它之前未被设置）
        draggedItem->setFlag(QGraphicsItem::ItemIsMovable, true);

        qDebug() << "拖拽开始 - 选中图形:" << draggedItem
            << " 初始位置:" << itemStartPos;
    }
}

void DragDrawer::handleMouseMove(const QPointF& scenePos) {
    if (!draggedItem) return;

    QPointF delta = scenePos - dragStartPos; // 计算位移量（场景坐标差）

    // 在场景坐标系下移动图形
    draggedItem->setPos(itemStartPos + delta);

    qDebug() << "拖拽中 - 当前位置:" << draggedItem->scenePos();
}

void DragDrawer::handleMouseRelease(QGraphicsSceneMouseEvent* event) {
    if (draggedItem) {
        qDebug() << "拖拽结束 - 最终位置:" << draggedItem->scenePos();
    }
    draggedItem = nullptr;  // 释放拖拽项
}

