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

#ifndef CAMERA_H
#define CAMERA_H

#include <QMap>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QString>

#include "Image/xmp.h"

struct Camera
{
    unsigned        id;
    unsigned        projectID;
    QString         name;
    QString         descName;
    QString         latitude;
    QString         longitude;
    QString         altitude;
    QString         orientation;
    QString         info;
    unsigned long   createdOn;
    unsigned        createdBy;
    unsigned long   updatedOn;
    unsigned        updatedBy;
    Camera();
};

typedef QMap< QString, Camera > CameraList;

class Cameras
{
public:
    static  Cameras &instance();

    bool addCamera(const Camera& camera);
    bool editCamera(const Camera& camera);
    void    addToXMPMap(const Camera &camera, XMPMap &xmpMap);

    const Camera &camera(unsigned id);
    const Camera &camera(const QString &cameraName);
    const CameraList &getCameraList();

    void    refresh();
    ~Cameras();

private:
    Cameras();

    CameraList      cameraList_;
    QSqlQueryModel  *cameraModel_;
    QMap< QString, unsigned > nameRowMap_;
    QMap< unsigned, QString > idNameMap_;

};

#endif // CAMERA_H
