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

#ifndef FOLDER_H
#define FOLDER_H

#include <QMap>
#include <QString>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSharedPointer>

struct File
{
    unsigned        id;
    unsigned        folderID;
    unsigned        partitionID;
    QString         name;
    unsigned long   updatedOn;
    unsigned        updatedBy;   
};

struct Folder
{
    unsigned        id;
    unsigned        cameraID;
    unsigned        partitionID;
    QString         name;
    QString         createdOn;
    unsigned        createdBy;
    QString         updatedOn;
    unsigned        updatedBy;
    unsigned        imagesAll;
    unsigned        imagesDone;

    Folder();
};

typedef QMap< QString, Folder > FolderList;
typedef QVector< QString > FolderNameList;
typedef QVector< File > FileList;

class Folders
{
public:
    static  Folders &instance();

    void    addFolder(const Folder &folder);
    void    addFolder(const Folder &folder, const QStringList &fileList, const QStringList &dateTimeList);

    const Folder &folder(unsigned id);
    const Folder &folder(const QString &folderName);
    const FolderList &getFolderList();

    const FolderNameList &getFolderNameList(unsigned cameraID);

    void    refresh();

private:
    Folders();

    FolderList  folderList_;
    QSharedPointer< QSqlQueryModel > folderModel_;
    QMap< QString, unsigned > nameRowMap_;
    QMap< unsigned, FolderNameList > cameraFolderMap_;
    QMap< unsigned, QString > idNameMap_;
};

#endif // FOLDER
