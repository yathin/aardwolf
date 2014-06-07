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

#include "folder.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>

#include "Helpers/filter.h"
#include "Helpers/mediator.h"

Folder::Folder()
    : id(0),
      cameraID(0),
      name(""),
      createdOn(""),
      createdBy(0),
      updatedOn(""),
      updatedBy(0),
      imagesAll(0),
      imagesDone(0)
{ }


Folders::Folders()
{
    refresh();
}

void Folders::refresh()
{
    this->folderModel_ = QSharedPointer< QSqlQueryModel >(new QSqlQueryModel);
    QString query = "SELECT * FROM folder f ";

    Filter::FilterType currentFilter = Filter::instance().folderFilter();

    if (currentFilter != Filter::FilterAll)
    {
        query += " LEFT JOIN folder_tagged ft ON f.id = ft.folder_id ";

        if (currentFilter == Filter::FilterTagged)
            query += " WHERE images_all != 0 AND images_all = images_done ";
        else if (currentFilter == Filter::FilterUntagged)
            query += " WHERE images_all != 0 AND images_all <> images_done OR images_done IS NULL";
    }

    qxtLog->debug() << "Folder Query: " << query;
    this->folderModel_->setQuery(query);

    folderList_.clear();
    nameRowMap_.clear();
    cameraFolderMap_.clear();
    idNameMap_.clear();

    for (int i=0; i < folderModel_->rowCount(); ++i)
    {
        QSqlRecord folderRecord = folderModel_->record(i);
        Folder f;
        f.id = folderRecord.value("id").toUInt();
        f.cameraID = folderRecord.value("camera_id").toUInt();
        f.partitionID = folderRecord.value("partition_id").toInt();
        f.name = folderRecord.value("name").toString();
        f.imagesAll = folderRecord.value("images_all").toUInt();
        f.imagesDone = folderRecord.value("images_done").toUInt();

        f.createdBy = folderRecord.value("created_by").toUInt();
        f.createdOn = folderRecord.value("created_on").toString();

        f.updatedBy = folderRecord.value("updated_by").toUInt();
        f.updatedOn = folderRecord.value("updated_on").toString();

        folderList_[f.name] = f;
        nameRowMap_[f.name] = i;

        idNameMap_[f.id] = f.name;

        cameraFolderMap_[f.cameraID].push_back(f.name);
    }
}

Folders& Folders::instance()
{
    static Folders folders;
    return folders;
}

void Folders::addFolder(const Folder &folder)
{
    // Folder id must NOT be set
    if (folder.id)
        return;

    QSqlQuery query;
    query.prepare("INSERT INTO folder(camera_id, name, partition_id, images_all) VALUES (:camera_id, :name, :partition_id, :images_all)");

    query.bindValue(":camera_id",    folder.cameraID);
    query.bindValue(":name",         folder.name);
    query.bindValue(":partition_id", folder.partitionID);
    query.bindValue(":images_all",   folder.imagesAll);

    query.exec();
    refresh();
}

void Folders::addFolder(const Folder &folder, const QStringList &fileList, const QStringList &dateTimeList)
{
    // Folder id must NOT be set
    if (folder.id)
        return;

    addFolder(folder);
    unsigned folderID = this->folder(folder.name).id;

    QSqlQuery q;
    q.prepare("INSERT INTO photo(folder_id, name, created_on) VALUES (?, ?, ?)");

    QVariantList folderIDList;
    QVariantList fileNameList;
    QVariantList dateTimeVList;
    int count = 0;
    for (QStringList::const_iterator file = fileList.begin(); file != fileList.end(); ++file)
    {
        folderIDList << folderID;
        fileNameList << *file;
        dateTimeVList << dateTimeList[count++];
    }

    fileNameList <<  QVariant(QVariant::String);
    q.addBindValue(folderIDList);
    q.addBindValue(fileNameList);
    q.addBindValue(dateTimeVList);

    q.execBatch();
}

const Folder &Folders::folder(unsigned id)
{
    return folder(idNameMap_[id]);
}

const Folder &Folders::folder(const QString &folderName)
{
    return folderList_[folderName];
}

const FolderList &Folders::getFolderList()
{
    return folderList_;
}

const FolderNameList &Folders::getFolderNameList(unsigned cameraID)
{
    if (cameraFolderMap_.find(cameraID) == cameraFolderMap_.end())
        cameraFolderMap_[cameraID] = FolderNameList();

    return cameraFolderMap_[cameraID];
}
