#include "shape.h"
#include "Drawable.h"
#include <opencv2/opencv.hpp>

cv::Mat QImageToCvMat(const QImage& image) {
    return cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.bits(), image.bytesPerLine()).clone();
}

QImage CvMatToQImage(const cv::Mat& mat) {
    return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32).copy();
}

void drawPointsOnImage(QPixmap& pixmap, const QVector<QPointF>& points) {
    cv::Mat img = QImageToCvMat(pixmap.toImage()); // QPixmap 转 cv::Mat

    for (const QPointF& point : points) {
        cv::circle(img, cv::Point(point.x(), point.y()), 1, cv::Scalar(0, 0, 255), -1); // 红色点
    }

    pixmap = QPixmap::fromImage(CvMatToQImage(img)); // cv::Mat 转 QPixmap
}


// 矩形绘制器实现
RectangleDrawer::RectangleDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent)
{
    originalPixmap = pixmapItem->pixmap();
}

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
            finalRects.append(finalRect); // 添加到 QVector 中
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

//void RectangleDrawer::updateCalculationPoints(int stepSize, int subSize) {
//    if (finalRects.isEmpty()) return;
//
//    // **重置 pixmap，清除所有旧点**
//    pixmapItem->setPixmap(originalPixmap);
//    pixmapItem->update();
//
//    QPixmap pixmap = pixmapItem->pixmap(); // 获取原始 pixmap
//    cv::Mat img = QImageToCvMat(pixmap.toImage()); // 转 OpenCV Mat
//
//    cv::Mat mask(img.rows, img.cols, CV_8UC1, cv::Scalar(0)); // 创建空白 mask
//    QVector<QPointF> calculationPoints;
//
//    // 计算 ROI 内的点
//    for (QGraphicsRectItem* rect : finalRects) {
//        QRectF roi = rect->rect();
//        cv::rectangle(mask,
//            cv::Point(roi.left(), roi.top()),
//            cv::Point(roi.right(), roi.bottom()),
//            cv::Scalar(255), cv::FILLED);
//    }
//
//    std::vector<cv::Point> points;
//    cv::findNonZero(mask, points);
//
//    for (const cv::Point& pt : points) {
//        if ((pt.x % stepSize == 0) && (pt.y % stepSize == 0)) {
//            calculationPoints.append(QPointF(pt.x + subSize / 2, pt.y + subSize / 2));
//        }
//    }
//
//    // **清空并更新 Drawable 计算点**
//    Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
//    if (drawable) {
//        drawable->calculationPoints.clear();
//        drawable->calculationPoints = calculationPoints;
//    }
//
//    // **绘制新点**
//   drawPointsOnImage(pixmap, calculationPoints);
//
//    // 更新 pixmapItem
//    pixmapItem->setPixmap(pixmap);
//    pixmapItem->update();
//}

void RectangleDrawer::updateCalculationPoints(int stepSize, int subSize) {
    if (finalRects.isEmpty()) return;

    pixmapItem->setPixmap(originalPixmap);
    pixmapItem->update();

    QPixmap pixmap = pixmapItem->pixmap();
    cv::Mat img = QImageToCvMat(pixmap.toImage());
    cv::Mat mask(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    QVector<QPointF> calculationPoints;

    // 创建ROI的mask
    for (QGraphicsRectItem* rect : finalRects) {
        QRectF roi = rect->rect();
        cv::rectangle(mask,
            cv::Point(roi.left(), roi.top()),
            cv::Point(roi.right(), roi.bottom()),
            cv::Scalar(255), cv::FILLED);
    }

    // 按步长遍历窗口
    for (int y = 0; y <= mask.rows - subSize; y += stepSize) {
        for (int x = 0; x <= mask.cols - subSize; x += stepSize) {
            cv::Rect window(x, y, subSize, subSize);
            cv::Mat roiMask = mask(window);
            if (cv::countNonZero(roiMask) == subSize * subSize) {
                calculationPoints.append(QPointF(x + subSize / 2, y + subSize / 2));
            }
        }
    }

    Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
    if (drawable) {
        drawable->calculationPoints = calculationPoints;
    }

    drawPointsOnImage(pixmap, calculationPoints);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
}


// 圆形绘制器实现
CircleDrawer::CircleDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {
    originalPixmap = pixmapItem->pixmap();
}

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

void CircleDrawer::updateCalculationPoints(int stepSize, int subSize) {
    if (finalCircles.isEmpty()) return;

    pixmapItem->setPixmap(originalPixmap);
    pixmapItem->update();

    QPixmap pixmap = pixmapItem->pixmap();
    cv::Mat img = QImageToCvMat(pixmap.toImage());
    cv::Mat mask(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    QVector<QPointF> calculationPoints;

    // 创建圆形区域的mask
    for (QGraphicsEllipseItem* circle : finalCircles) {
        QRectF rect = circle->rect();
        cv::Point center(rect.center().x(), rect.center().y());
        int radius = rect.width() / 2;
        cv::circle(mask, center, radius, cv::Scalar(255), cv::FILLED);
    }

    // 按步长遍历窗口
    for (int y = 0; y <= mask.rows - subSize; y += stepSize) {
        for (int x = 0; x <= mask.cols - subSize; x += stepSize) {
            cv::Rect window(x, y, subSize, subSize);
            cv::Mat roiMask = mask(window);
            if (cv::countNonZero(roiMask) == subSize * subSize) {
                calculationPoints.append(QPointF(x + subSize / 2, y + subSize / 2));
            }
        }
    }

    Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
    if (drawable) {
        drawable->calculationPoints = calculationPoints;
    }

    drawPointsOnImage(pixmap, calculationPoints);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
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
        finalCircles.append(ellipseItem); // 添加到 QVector 中
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
PolygonDrawer::PolygonDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {
    originalPixmap = pixmapItem->pixmap();
}

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

void PolygonDrawer::updateCalculationPoints(int stepSize, int subSize) {
    if (finalPolygons.isEmpty()) return;

    pixmapItem->setPixmap(originalPixmap);
    pixmapItem->update();

    QPixmap pixmap = pixmapItem->pixmap();
    cv::Mat img = QImageToCvMat(pixmap.toImage());
    cv::Mat mask(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    QVector<QPointF> calculationPoints;

    // 填充多边形区域到mask
    for (QGraphicsPolygonItem* polygon : finalPolygons) {
        QPolygonF poly = polygon->polygon();
        std::vector<cv::Point> polyPoints;
        for (const QPointF& p : poly) {
            polyPoints.emplace_back(cv::Point(p.x(), p.y()));
        }
        const cv::Point* pts[] = { polyPoints.data() };
        int numPoints[] = { static_cast<int>(polyPoints.size()) };
        cv::fillPoly(mask, pts, numPoints, 1, cv::Scalar(255));
    }

    // 按步长遍历窗口
    for (int y = 0; y <= mask.rows - subSize; y += stepSize) {
        for (int x = 0; x <= mask.cols - subSize; x += stepSize) {
            cv::Rect window(x, y, subSize, subSize);
            cv::Mat roiMask = mask(window);
            if (cv::countNonZero(roiMask) == subSize * subSize) {
                calculationPoints.append(QPointF(x + subSize / 2, y + subSize / 2));
            }
        }
    }

    Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
    if (drawable) {
        drawable->calculationPoints = calculationPoints;
    }

    drawPointsOnImage(pixmap, calculationPoints);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
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
    finalPolygons.append(finalItem); // 添加到 QVector 中
}

void PolygonDrawer::reset() {
    currentPolygon.clear();
    delete currentPolygonItem;
    currentPolygonItem = nullptr;
}

ClipPolygonDrawer::ClipPolygonDrawer(QGraphicsPixmapItem* parent) : pixmapItem(parent) {
    originalPixmap = pixmapItem->pixmap();
}

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

void ClipPolygonDrawer::updateCalculationPoints(int stepSize, int subSize) {
    if (finalClipPolygons.isEmpty()) return;

    pixmapItem->setPixmap(originalPixmap);
    pixmapItem->update();

    QPixmap pixmap = pixmapItem->pixmap();
    cv::Mat img = QImageToCvMat(pixmap.toImage());
    cv::Mat mask(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    QVector<QPointF> calculationPoints;

    // 遍历最终裁剪多边形并填充 mask
    for (QGraphicsPathItem* polygon : finalClipPolygons) {
        QPainterPath path = polygon->path();
        QPolygonF poly = path.toFillPolygon();
        std::vector<cv::Point> polyPoints;
        for (const QPointF& p : poly) {
            polyPoints.emplace_back(cv::Point(p.x(), p.y()));
        }
        const cv::Point* pts[] = { polyPoints.data() };
        int numPoints[] = { static_cast<int>(polyPoints.size()) };
        cv::fillPoly(mask, pts, numPoints, 1, cv::Scalar(255));
    }

    // 按步长遍历窗口
    for (int y = 0; y <= mask.rows - subSize; y += stepSize) {
        for (int x = 0; x <= mask.cols - subSize; x += stepSize) {
            cv::Rect window(x, y, subSize, subSize);
            cv::Mat roiMask = mask(window);
            if (cv::countNonZero(roiMask) == subSize * subSize) {
                calculationPoints.append(QPointF(x + subSize / 2, y + subSize / 2));
            }
        }
    }

    Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
    if (drawable) {
        drawable->calculationPoints = calculationPoints;
    }

    drawPointsOnImage(pixmap, calculationPoints);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
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
			finalClipPolygons.append(newItem);
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
        Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
        if (drawable) {
            QPointF clickPos = pixmapItem->mapFromScene(event->scenePos());
            drawable->calculationPoints.erase(
                std::remove_if(drawable->calculationPoints.begin(), drawable->calculationPoints.end(),
                    [&](const QPointF& p) { return QLineF(p, clickPos).length() < 5; }),
                drawable->calculationPoints.end());

            QPixmap pixmap = pixmapItem->pixmap();
            drawPointsOnImage(pixmap, drawable->calculationPoints);
            pixmapItem->setPixmap(pixmap);
            pixmapItem->update();
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

void DragDrawer::updateCalculationPoints(int stepSize, int subSize) {
    Drawable* drawable = dynamic_cast<Drawable*>(pixmapItem->parentItem());
    if (!drawable) return;

    QVector<QPointF> updatedPoints;
    for (const QPointF& point : drawable->calculationPoints) {
        updatedPoints.append(point + draggedItem->scenePos() - itemStartPos);
    }
    drawable->calculationPoints = updatedPoints;

    QPixmap pixmap = pixmapItem->pixmap();
    drawPointsOnImage(pixmap, updatedPoints);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
}
