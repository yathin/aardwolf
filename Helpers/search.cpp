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

#include "search.h"
#include "Folder/folder.h"

#include <QxtLogger>

#include <QVector>
#include <QSqlQuery>

Search& Search::instance()
{
    static Search search;
    return search;
}

Search::Search(QObject *parent) :
    QObject(parent)
{
}

void Search::search(const QString &searchTerm, FileList &fileList, int cameraID, int folderID)
{
    qxtLog->debug() << "Searching for Term: " << searchTerm << " with cameraID = " << cameraID << " and folderID = " << folderID;

    QSqlQuery query;

    if (cameraID == -1 && folderID == -1)
    {
        // search all
        query.prepare("SELECT photo.* FROM photo LEFT JOIN tag ON (photo.id = tag.photo_id) WHERE tag.name LIKE :search GROUP BY photo.id HAVING tag.id IS NOT NULL");
    }
    else if (cameraID == -1 && folderID != -1)
    {
        // search within folder
        query.prepare("SELECT photo.* FROM photo LEFT JOIN tag ON (photo.id = tag.photo_id) WHERE photo.folder_id = :folderID AND tag.name LIKE :search GROUP BY photo.id HAVING tag.id IS NOT NULL");
        query.bindValue(":folderID", folderID);
    }
    else if (cameraID != -1 && folderID == -1)
    {
        // search within camera
        query.prepare("SELECT photo.* FROM folder LEFT JOIN photo ON (folder.id = photo.folder_id) LEFT JOIN tag ON (photo.id = tag.photo_id) WHERE folder.camera_id = :cameraID AND tag.name LIKE :search GROUP BY photo.id HAVING tag.id IS NOT NULL");
        query.bindValue(":cameraID", cameraID);
    }

    query.bindValue(":search", searchTerm);
    query.exec();

    qxtLog->debug() << "Executed Search Query: " << query.executedQuery();

    while (query.next())
    {
        File f;
        f.id        = query.value(0).toUInt();
        f.folderID  = query.value(1).toUInt();
        f.name      = query.value(2).toString();
        f.updatedOn = query.value(5).toUInt();
        f.updatedBy = query.value(6).toUInt();

        fileList.push_back(f);
    }

    qxtLog->debug() << "Returning " << fileList.count() << " results" ;
}


