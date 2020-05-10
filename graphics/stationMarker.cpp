#include "commonConstants.h"
#include "stationMarker.h"
#include "meteoPoint.h"

#include <QMenu>
#include <QtDebug>

StationMarker::StationMarker(qreal radius,bool sizeIsZoomInvariant, QColor fillColor, MapGraphicsView* view, MapGraphicsObject *parent) :
    CircleObject(radius, sizeIsZoomInvariant, fillColor, parent)
{

    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable, false);
    _view = view;
}

void StationMarker::setId(std::string id)
{
    _id = id;
}

std::string StationMarker::id() const
{
    return _id;
}

void StationMarker::setName(const std::string &name)
{
    _name = name;
}


void StationMarker::setToolTip(Crit3DMeteoPoint* meteoPoint_)
{
    QString idpoint = QString::fromStdString(meteoPoint_->id);
    QString name = QString::fromStdString(meteoPoint_->name);
    QString dataset = QString::fromStdString(meteoPoint_->dataset);
    double altitude = meteoPoint_->point.z;
    QString municipality = QString::fromStdString(meteoPoint_->municipality);

    QString toolTipText = QString("<b> %1 </b> <br/> ID: %2 <br/> dataset: %3 <br/> altitude: %4 m <br/> municipality: %5")
                            .arg(name).arg(idpoint).arg(dataset).arg(altitude).arg(municipality);

    if (meteoPoint_->currentValue != NODATA)
    {
        QString value = QString::number(double(meteoPoint_->currentValue));

        QString myQuality = "";
        if (meteoPoint_->quality == quality::wrong_syntactic)
            myQuality = "WRONG DATA (syntax control)";
        if (meteoPoint_->quality == quality::wrong_spatial)
            myQuality = "WRONG DATA (spatial control)";

        toolTipText = QString("value: <b> %1 <br/> %2 <br/> </b>").arg(value).arg(myQuality) + toolTipText;
    }

    CircleObject::setToolTip(toolTipText);
}

void StationMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->button() == Qt::RightButton)
    {
        QMenu menu;
        QString title = "Point: " + QString::fromStdString(this->_name) + " ID: " + QString::fromStdString(this->id());
        menu.addAction(title);
        menu.addSeparator();

        QAction *openMeteoWidget = menu.addAction("Open new meteo widget");
        QAction *appendMeteoWidget = menu.addAction("Append to last meteo widget");

        #ifdef CRITERIA3D
            menu.addSeparator();
            QAction *openCropWidget = menu.addAction("Open crop widget");
        #endif

        QAction *selection =  menu.exec(QCursor::pos());

        if (selection != nullptr)
        {
            if (selection == openMeteoWidget)
            {
                emit newStationClicked(_id);
            }
            else if (selection == appendMeteoWidget)
            {
                emit appendStationClicked(_id);
            }
            #ifdef CRITERIA3D
            else if (selection == openCropWidget)
            {
                emit openCropClicked(_id);
            }
            #endif
        }
    }
}

void StationMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

