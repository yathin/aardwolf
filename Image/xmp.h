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

#ifndef XMP_H
#define XMP_H

#include <QMap>
#include <QString>

// name => value map
typedef QMap< QString, QString > XMPMap;

// Project Keys
#define XMP_PROJECT_NAME        "ProjectName"
#define XMP_PROJECT_INFO        "ProjectInfo"

// Camera Keys
#define XMP_CAMERA_NAME         "CameraName"
#define XMP_CAMERA_DESC_NAME    "CameraDescName"
#define XMP_CAMERA_LATITUDE     "CameraLatitude"
#define XMP_CAMERA_LONGITUDE    "CameraLongitude"
#define XMP_CAMERA_ALTITUDE     "CameraAltitude"
#define XMP_CAMERA_ORIENTATION  "CameraOrientation"
#define XMP_CAMERA_INFO         "CameraInfo"

class XMP
{
public:
    /* Assumes that tagGroup and tagName only contain ASCII characters */
    static void makeXMPName(const QString& tagGroup, const QString& tagName, QString& xmpName);

    XMP();
    ~XMP();

private:

};

#endif // XMP_H
