#include "zoomablechartview.h"
#include <QtGui/QGuiApplication>
#include <QtGui/QMouseEvent>

#include <QDebug>

ZoomableChartView::ZoomableChartView(QWidget *parent) :
    QChartView(parent)
{
    setRubberBand(QChartView::RectangleRubberBand);
    // default values
    rangeLimitedAxisX = false;
    rangeLimitedAxisY = false;
    nZoomIterations = 0;
    maxZoom = 10;
}

void ZoomableChartView::mousePressEvent(QMouseEvent *event)
{
    m_isTouching = true;
    m_lastMousePos = event->localPos();
    qWarning() << "Press" << m_lastMousePos;
    QChartView::mousePressEvent(event);
}

void ZoomableChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isTouching)
        return;

    if (dragMode() == ScrollHandDrag) {
        if (event->buttons() & Qt::LeftButton) {
            bool moveHorizontalAxis = false;
            for (auto axis : chart()->axes()) {
                if (axis->orientation() == Qt::Horizontal && isAxisTypeZoomableWithMouse(axis->type())) {
                    moveHorizontalAxis = true;
                    break;
                }
            }

            if (QGuiApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
                moveHorizontalAxis = !moveHorizontalAxis;

            if (moveHorizontalAxis) {
                qreal dx = -(event->localPos().x() - m_lastMousePos.x());
                qWarning() << "Move" << event->localPos().x() << dx;
                for (auto series : this->chart()->series()) {
                    for (auto axis : series->attachedAxes()) {
                        if (axis->orientation() != Qt::Horizontal)
                            continue;
                            rangeXAxis = static_cast<RangeLimitedValueAxis*>(axis);
                            rangeXAxis->setLowerLimit(dxMin);
                            rangeXAxis->setUpperLimit(dxMax);
                            if (rangeXAxis->orientation() != Qt::Horizontal)
                                continue;
                            QPointF oldPoint = getSeriesCoordFromChartCoord(m_lastMousePos, series);
                            QPointF newPoint = getSeriesCoordFromChartCoord(event->localPos(), series);
                            qreal dxAxis = -(newPoint.x() - oldPoint.x());
                            if (rangeLimitedAxisX
                                    && (rangeXAxis->min() + dxAxis) < rangeXAxis->lowerLimit())
                            {
                                dxAxis = -(rangeXAxis->min() - rangeXAxis->lowerLimit());
                                if (qFuzzyIsNull(dxAxis))
                                {
                                    dx = 0.0;
                                } else
                                {
                                    QPointF dummyCoord(oldPoint.x() - dxAxis, m_lastMousePos.y());
                                    dummyCoord = getChartCoordFromSeriesCoord(dummyCoord, series);
                                    dx = (m_lastMousePos.x() - dummyCoord.x());
                                }
                            }


                            if (rangeLimitedAxisX
                                    && (rangeXAxis->max() + dxAxis) > rangeXAxis->upperLimit())
                            {
                                dxAxis = -(rangeXAxis->upperLimit() - rangeXAxis->max());
                                if (qFuzzyIsNull(dxAxis)) {
                                    dx = 0.0;
                                } else {
                                    QPointF dummyCoord(oldPoint.x() - dxAxis, m_lastMousePos.y());
                                    dummyCoord = getChartCoordFromSeriesCoord(dummyCoord, series);
                                    dx = -(m_lastMousePos.x() - dummyCoord.x());
                                }
                            }
                    }
                }
                chart()->scroll(dx, 0);
            } else {
                qreal dy = event->pos().y() - m_lastMousePos.y();
                for (auto series : this->chart()->series()) {
                    for (auto axis : series->attachedAxes()) {
                        if (axis->orientation() != Qt::Vertical)
                            continue;

                            rangeYAxis = static_cast<RangeLimitedValueAxis*>(axis);
                            rangeYAxis->setLowerLimit(dyMin);
                            rangeYAxis->setUpperLimit(dyMax);
                            if (rangeYAxis->orientation() != Qt::Vertical)
                                continue;
                            QPointF oldPoint = getSeriesCoordFromChartCoord(m_lastMousePos, series);
                            QPointF newPoint = getSeriesCoordFromChartCoord(event->localPos(), series);
                            qreal dyAxis = -(newPoint.y() - oldPoint.y());
                            if (rangeLimitedAxisY
                                    && (rangeYAxis->min() + dyAxis) < rangeYAxis->lowerLimit())
                            {
                                dyAxis = rangeYAxis->min() - rangeYAxis->lowerLimit();
                                if (qFuzzyIsNull(dyAxis))
                                {
                                    dy = 0.0;
                                } else
                                {
                                    QPointF dummyCoord(m_lastMousePos.x(), oldPoint.y() - dyAxis);
                                    dummyCoord = getChartCoordFromSeriesCoord(dummyCoord, series);
                                    dy = (m_lastMousePos.y() - dummyCoord.y());
                                }
                            }

                            if (rangeLimitedAxisY
                                    && (rangeYAxis->max() + dyAxis) > rangeYAxis->upperLimit())
                            {
                                dyAxis = rangeYAxis->upperLimit() - rangeYAxis->max();
                                if (qFuzzyIsNull(dyAxis)) {
                                    dy = 0.0;
                                } else {
                                    QPointF dummyCoord(m_lastMousePos.x(), oldPoint.y() - dyAxis);
                                    dummyCoord = getChartCoordFromSeriesCoord(dummyCoord, series);
                                    dy = -(m_lastMousePos.y() - dummyCoord.y());
                                }
                            }
                    }
                }
                chart()->scroll(0, dy);
            }
        }
        m_lastMousePos = event->pos();
    }

    QChartView::mouseMoveEvent(event);
}

void ZoomableChartView::wheelEvent(QWheelEvent *event)
{
    bool zoomHorizontalAxis = false;
    for (auto axis : chart()->axes()) {
        if (axis->orientation() == Qt::Horizontal && isAxisTypeZoomableWithMouse(axis->type())) {
            zoomHorizontalAxis = true;
            break;
        }
    }

    if (QGuiApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
        zoomHorizontalAxis = !zoomHorizontalAxis;

    if (zoomHorizontalAxis) {
        if (event->angleDelta().y() > 0) {
            zoomX(2, event->pos().x() - chart()->plotArea().x());
        } else if (event->angleDelta().y() < 0) {
            zoomX(0.5, event->pos().x() - chart()->plotArea().x());
        }
    } else {
        if (event->angleDelta().y() > 0) {
            zoomY(2, event->pos().y() - chart()->plotArea().y());
        } else if (event->angleDelta().y() < 0) {
            zoomY(0.5, event->pos().y() - chart()->plotArea().y());
        }
    }
}

bool ZoomableChartView::isAxisTypeZoomableWithMouse(const QAbstractAxis::AxisType type)
{
    return (type == QAbstractAxis::AxisTypeValue
            || type == QAbstractAxis::AxisTypeLogValue
            || type == QAbstractAxis::AxisTypeDateTime);
}

QPointF ZoomableChartView::getSeriesCoordFromChartCoord(const QPointF &chartPos, QAbstractSeries *series) const
{
    auto const chartItemPos = chart()->mapFromScene(chartPos);
    auto const valueGivenSeries = chart()->mapToValue(chartItemPos, series);
    return valueGivenSeries;
}

QPointF ZoomableChartView::getChartCoordFromSeriesCoord(const QPointF &seriesPos, QAbstractSeries *series) const
{
    QPointF ret = chart()->mapToPosition(seriesPos, series);
    ret = chart()->mapFromScene(ret);
    return ret;
}

ZoomableChartView::ZoomMode ZoomableChartView::zoomMode() const
{
    return m_zoomMode;
}

void ZoomableChartView::setZoomMode(const ZoomMode &zoomMode)
{
    if (m_zoomMode != zoomMode) {
        m_zoomMode = zoomMode;
        switch (zoomMode) {
        case Pan:
            setRubberBand(QChartView::NoRubberBand);
            setDragMode(QChartView::ScrollHandDrag);
            break;
        case RectangleZoom:
            setRubberBand(QChartView::RectangleRubberBand);
            setDragMode(QChartView::NoDrag);
            break;
        case HorizontalZoom:
            setRubberBand(QChartView::HorizontalRubberBand);
            setDragMode(QChartView::NoDrag);
            break;
        case VerticalZoom:
            setRubberBand(QChartView::VerticalRubberBand);
            setDragMode(QChartView::NoDrag);
            break;
        }
    }
}

void ZoomableChartView::zoomX(qreal factor, qreal xcenter)
{

    if (factor > 1)
    {
        if ((nZoomIterations + 1) > maxZoom)
        {
            return;
        }
        nZoomIterations = nZoomIterations + 1;
    }
    else
    {
        if ((nZoomIterations - 1) < -maxZoom)
        {
            return;
        }
        nZoomIterations = nZoomIterations - 1;
    }
    QRectF rect = chart()->plotArea();
    qreal widthOriginal = rect.width();
    rect.setWidth(widthOriginal / factor);
    qreal centerScale = (xcenter / widthOriginal);

    qreal leftOffset = (xcenter - (rect.width() * centerScale) );

    rect.moveLeft(rect.x() + leftOffset);
    chart()->zoomIn(rect);
}

void ZoomableChartView::zoomX(qreal factor)
{

    QRectF rect = chart()->plotArea();
    qreal widthOriginal = rect.width();
    QPointF center_orig = rect.center();

    rect.setWidth(widthOriginal / factor);

    rect.moveCenter(center_orig);

    chart()->zoomIn(rect);
}

void ZoomableChartView::zoomY(qreal factor, qreal ycenter)
{
    if (factor > 1)
    {
        if ((nZoomIterations + 1) > maxZoom)
        {
            return;
        }
        nZoomIterations = nZoomIterations + 1;
    }
    else
    {
        if ((nZoomIterations - 1) < -maxZoom)
        {
            return;
        }
        nZoomIterations = nZoomIterations - 1;
    }
    QRectF rect = chart()->plotArea();
    qreal heightOriginal = rect.height();
    rect.setHeight(heightOriginal / factor);
    qreal centerScale = (ycenter / heightOriginal);

    qreal topOffset = (ycenter - (rect.height() * centerScale) );

    rect.moveTop(rect.x() + topOffset);
    chart()->zoomIn(rect);
}

void ZoomableChartView::zoomY(qreal factor)
{
    QRectF rect = chart()->plotArea();
    qreal heightOriginal = rect.height();
    QPointF center_orig = rect.center();

    rect.setHeight(heightOriginal / factor);

    rect.moveCenter(center_orig);

    chart()->zoomIn(rect);
}


void ZoomableChartView::mouseReleaseEvent(QMouseEvent *event)
{
    m_isTouching = false;
    QChartView::mouseReleaseEvent(event);
}

//![1]
void ZoomableChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
        //![1]
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void ZoomableChartView::setRangeX(double dxMin, double dxMax)
{
    rangeLimitedAxisX = true;
    this->dxMin = dxMin;
    this->dxMax = dxMax;

}

void ZoomableChartView::setRangeY(double dyMin, double dyMax)
{
    rangeLimitedAxisY = true;
    this->dyMin = dyMin;
    this->dyMax = dyMax;
}

void ZoomableChartView::setMaxZoomIteration(int max)
{
    maxZoom = max;
}
