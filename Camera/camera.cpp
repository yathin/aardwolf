/**
    This file is part of Aardwolf.

    Aardwolf is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Aardwolf is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Aardwolf.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <QDir>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>

#include "Helpers/mediator.h"

#include "camera.h"

Camera::Camera()
    : id(0),
      projectID(0),
      name(""),
      descName(""),
      latitude(""),
      longitude(""),
      altitude(""),
      orientation(""),
      info(""),
      createdOn(0),
      createdBy(0),
      updatedOn(0),
      updatedBy(0)
{ }

Cameras::Cameras()
{
    this->cameraModel_ = new QSqlQueryModel;
    refresh();
}

Cameras::~Cameras()
{
    delete this->cameraModel_;
}

void Cameras::refresh()
{   
    cameraModel_->setQuery("SELECT * FROM camera");

    cameraList_.clear();
    nameRowMap_.clear();
    idNameMap_.clear();

    for (int i=0; i < cameraModel_->rowCount(); ++i)
    {
        QSqlRecord cameraRecord = cameraModel_->record(i);
        Camera c;
        c.id = cameraRecord.value("id").toUInt();
        c.projectID = cameraRecord.value("project_id").toUInt();
        c.name = cameraRecord.value("name").toString();
        c.descName = cameraRecord.value("desc_name").toString();
        c.latitude = cameraRecord.value("latitude").toString();
        c.longitude = cameraRecord.value("longitude").toString();
        c.altitude = cameraRecord.value("altitude").toString();
        c.info = cameraRecord.value("info").toString();
        c.orientation = cameraRecord.value("orientation").toString();
        cameraList_[c.name] = c;
        nameRowMap_[c.name] = i;
        idNameMap_[c.id] = c.name;
    }
}

Cameras& Cameras::instance()
{
    static Cameras cameras;
    return cameras;
}

bool Cameras::addCamera(const Camera &camera)
{
    // camera id must NOT be set
    if (camera.id)
        return false;

    if (this->cameraList_.find(camera.name) != this->cameraList_.end())
        return false;

    QSqlQuery query;
    // NOTE: the column names must not contain space between separators! Otherwise QT throws a parameter count mismatch error
    query.prepare("INSERT INTO camera(project_id,name,desc_name,latitude,longitude,altitude,orientation,info) VALUES (:project_id,:name,:desc_name,:latitude,:longitude,:altitude,:orientation,:info)");
    query.bindValue(":project_id", camera.projectID);
    query.bindValue(":name", camera.name);
    query.bindValue(":desc_name", camera.descName);
    query.bindValue(":latitude", camera.latitude);
    query.bindValue(":longitude", camera.longitude);
    query.bindValue(":altitude", camera.altitude);
    query.bindValue(":orientation", camera.orientation);
    query.bindValue(":info", camera.info);

    if (!query.exec())
    {
        qxtLog->error() << "Camera Insert Failed. Reason: " << query.lastError().text();
        return false;
    }
    refresh();
    Mediator::instance().refreshCameraMenuActions();
    return true;
}

bool Cameras::editCamera(const Camera &camera)
{
    // camera id MUST be set
    if (!camera.id)
        return false;

    QSqlQuery query;
    query.prepare("UPDATE camera SET name=:name,desc_name=:desc_name,latitude=:latitude,longitude=:longitude,altitude=:altitude,orientation=:orientation,info=:info,updated_on=CURRENT_TIMESTAMP WHERE id=:id AND project_id=:project_id");

    query.bindValue(":id", camera.id);
    query.bindValue(":project_id", camera.projectID);
    query.bindValue(":name", camera.name);
    query.bindValue(":desc_name", camera.descName);
    query.bindValue(":latitude", camera.latitude);
    query.bindValue(":longitude", camera.longitude);
    query.bindValue(":altitude", camera.altitude);
    query.bindValue(":orientation", camera.orientation);
    query.bindValue(":info", camera.info);

    if (!query.exec())
    {
        qxtLog->error() << "Camera Update Failed. Reason: " << query.lastError().text();
        return false;
    }
    refresh();
    Mediator::instance().refreshCameraMenuActions();
    return true;
}

const Camera &Cameras::camera(unsigned id)
{
    return camera(idNameMap_[id]);
}

const Camera &Cameras::camera(const QString &cameraName)
{
    return cameraList_[cameraName];
}

const CameraList &Cameras::getCameraList()
{
    return cameraList_;
}

void  Cameras::addToXMPMap(const Camera &camera, XMPMap &xmpMap)
{
    xmpMap[XMP_CAMERA_NAME]         = camera.name;
    xmpMap[XMP_CAMERA_DESC_NAME]    = camera.descName;
    xmpMap[XMP_CAMERA_ALTITUDE]     = camera.altitude;
    xmpMap[XMP_CAMERA_ORIENTATION]  = camera.orientation;
    xmpMap[XMP_CAMERA_LATITUDE]     = camera.latitude;
    xmpMap[XMP_CAMERA_LONGITUDE]    = camera.longitude;
    xmpMap[XMP_CAMERA_INFO]         = camera.info;
}
