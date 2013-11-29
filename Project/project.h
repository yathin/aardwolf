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

#ifndef PROJECT_H
#define PROJECT_H

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVector>

#include "Camera/camera.h"
#include "Tag/edittagdefintion.h"
#include "Tag/tagdefinition.h"
#include "Image/xmp.h"

class CurrentProject;

class Project {
public:
    QString                 name;
    QString                 location;
    QString                 info;
    bool                    xmp;

    const TagDefinition&    getTagDefinition() const;
    void                    refreshTagDefinition();

    void                    getInfoInHTML(QString& html);

    unsigned                id();

    void                    addToXMPMap(XMPMap   &xmpMap);

    Project();
    Project(const Project& p);
    virtual ~Project();

private:
    Project(const QString& fileName);

    void            create();
    void            close();

    TagDefinition   tagDef_;

    bool            insertDB(bool update=false);
    bool            createTableFromFile(const QString& fileName);

    friend class    CurrentProject;
    friend class    CreateProject;

    unsigned        id_;
};

class CurrentProject {
public:
    static      CurrentProject& instance();
    Project&    project();
    bool        initFromFile(const QString& fileName);
    void        close();
    void        create();
private:
    CurrentProject();
    CurrentProject(const CurrentProject&) {}
    ~CurrentProject();

    void setCurrentProject(const Project& project);
    QSharedPointer< Project > currentProject_;

    friend class CreateProject;
};

#endif // PROJECT_H
