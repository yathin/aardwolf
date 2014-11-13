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

#include "filemanager.h"
#include "Helpers/mediator.h"

#include <QFile>
#include <QSqlQuery>
#include <qmath.h>

#include "historybar.h"
#include "mediator.h"
#include "search.h"
#include "Project/partitionmanager.h"

static int imgPerPage = 100;

FileManager::FileManager(QObject *parent) :
    QObject(parent),
    currentLevel_(LevelProject),
    currentCameraName_(""),
    currentFolderName_(""),
    totalPages_(0),
    currentPage_(1)
{
}

const File &FileManager::file(unsigned index)
{
    return fileList_[((currentPage_ - 1) * imgPerPage) + index];
}

void FileManager::openDir(const QString &dirName)
{
    Mediator::instance().closeFile();

    QStringList cameraList, folderList;
    QStringList files;
    unsigned cameraID = 0;
    unsigned folderID = 0;

    QMap< int, QString > partitionMap;
    PartitionManager::getPartitionLocation(partitionMap);

    cameraList.clear();
    folderList.clear();

    if (dirName.startsWith("AardwolfSearch: "))
    {
        QString searchString = dirName.mid(16, dirName.length());

        fileList_.clear();
        Mediator::instance().clearLimoo();

        HistoryBar::instance().setSearch(searchString);

        if (currentLevel_ == LevelCamera)
        {
            Search::instance().search(searchString, fileList_);
        }
        else if (currentLevel_ == LevelFolder)
        {
            cameraID = Cameras::instance().camera(currentCameraName_).id;
            Search::instance().search(searchString, fileList_, cameraID);
        }
        else if (currentLevel_ == LevelFiles)
        {
            folderID = Folders::instance().folder(currentFolderName_).id;
            Search::instance().search(searchString, fileList_, -1, folderID);
        }

        if (fileList_.empty())
            return;

        totalPages_ = ceil(fileList_.count() / float(imgPerPage));
        currentPage_ = 1;
        qxtLog->debug() << "Setting page limits: " << 1 << ", " << totalPages_;

        QString pageInfo = QString::number(totalPages_) + " pages, " + QString::number(fileList_.count()) + " images.";
        Mediator::instance().setPageLimits(1, totalPages_, pageInfo);

        int begin = 0;
        int end = 0;
        getFileIndex(begin, end);

        for (int f = begin; f < end; ++f)
        {
            const Folder &folder = Folders::instance().folder(fileList_[f].folderID);
            const Camera &camera = Cameras::instance().camera(folder.cameraID);
            files.append(partitionMap[folder.partitionID] + "/" + camera.name + "/" + folder.name + "/" + fileList_[f].name);
        }
        Mediator::instance().addFilesToLimoo(files);
        Mediator::instance().enableDirFilter(false);
        Mediator::instance().enableFileFilter(false);
        return;
    }

    switch (currentLevel_)
    {
        case LevelProject:
            qxtLog->debug() << "Setting up Project Cameras in dir: " << dirName;
            for (CameraList::const_iterator camera = Cameras::instance().getCameraList().begin(); camera != Cameras::instance().getCameraList().end(); ++camera)
            {
                if (camera->name.isEmpty())
                    continue;
                qxtLog->debug() << "Attempting to add Camera: " << dirName << "/" << camera->name;
                if (QFile(dirName + "/" + camera->name).exists())
                    cameraList.append(dirName + "/" + camera->name);
            }
            Mediator::instance().clearLimoo();
            Mediator::instance().addFilesToLimoo(cameraList);
            Mediator::instance().enableDirFilter(false);
            Mediator::instance().enableFileFilter(false);
            currentLevel_ = LevelCamera;
            return;
            break;
        case LevelCamera:
            qxtLog->debug() << "Setting up Camera Folders...";
            currentCameraName_ = dirName;
            cameraID = Cameras::instance().camera(currentCameraName_).id;
            Folders::instance().refresh();
            foreach (QString folder, Folders::instance().getFolderNameList(cameraID).toList())
            {
                int folderPartition = Folders::instance().folder(folder).partitionID;
                qxtLog->debug() << "Folder Partition: " << folderPartition;
                QString folderLocation = partitionMap[folderPartition] + "/" + currentCameraName_ + "/" + folder;
                qxtLog->debug() << "Adding folder location: " << folderLocation;
                folderList.append(folderLocation);
            }
            Mediator::instance().clearLimoo();
            Mediator::instance().addFilesToLimoo(folderList);
            HistoryBar::instance().setCamera(currentCameraName_);
            Mediator::instance().enableDirFilter(true);
            Mediator::instance().enableFileFilter(false);
            currentLevel_ = LevelFolder;
            return;
            break;
        case LevelFolder:
        case LevelFiles:
            qxtLog->debug() << "Setting up Folder Files...";
            currentFolderName_ = dirName;
            folderID = Folders::instance().folder(currentFolderName_).id;
            fileList_.clear();
            getFiles(folderID, fileList_);
            totalPages_ = ceil(fileList_.count() / float(imgPerPage));
            qxtLog->debug() << "Setting page limits: " << 1 << ", " << totalPages_;
            QString pageInfo = QString::number(totalPages_) + " pages, " + QString::number(fileList_.count()) + " images.";
            Mediator::instance().setPageLimits(1, totalPages_, pageInfo);
            int begin = 0, end = 0;
            getFileIndex(begin, end, 1);
            for (int f = begin; f < end; ++f)
            {
                files.append(partitionMap[fileList_[f].partitionID] + "/" + currentCameraName_ + "/" + currentFolderName_ + "/" + fileList_[f].name);
            }
            HistoryBar::instance().setFolder(currentFolderName_);
            Mediator::instance().clearLimoo();
            Mediator::instance().addFilesToLimoo(files);
            Mediator::instance().applyGrouping();
            Mediator::instance().enableDirFilter(false);
            Mediator::instance().enableFileFilter(true);
            currentLevel_ = LevelFiles;
            return;
            break;
    }
}

void FileManager::setLevel(FileLevel fileLevel)
{
    currentLevel_ = fileLevel;
    if (currentLevel_ == LevelProject) openDir(CurrentProject::instance().project().location);
    else if (currentLevel_ == LevelCamera) openDir(currentCameraName_);
    else if (currentLevel_ == LevelFolder) openDir(currentFolderName_);
}

const QString& FileManager::currentFolderName()
{
    return currentFolderName_;
}

FileManager::FileLevel FileManager::currentLevel()
{
    return currentLevel_;
}

void FileManager::pageChanged(int page)
{
    currentPage_ = page;
    QStringList files;
    Mediator::instance().setFocusOnLimoo();

    int begin = 0;
    int end = 0;
    getFileIndex(begin, end, page);

    for (int f = begin; f < end; ++f)
    {
        files.append(CurrentProject::instance().project().location + "/" + currentCameraName_ + "/" + currentFolderName_ + "/" + fileList_[f].name);
    }
    Mediator::instance().clearLimoo();
    Mediator::instance().addFilesToLimoo(files);
    Mediator::instance().applyGrouping();
}

void FileManager::getFileIndex(int &begin, int &end, int page)
{
    begin = (imgPerPage*(page-1));
    end   = begin + imgPerPage;
    if (end > fileList_.count())
        end = fileList_.count();
}


void FileManager::getFileMeta(QVector<int> photoIDs, FileMeta &fileMeta)
{
    QSqlQuery tagQuery;
    QString query = "SELECT group_name, name, value FROM tag pm WHERE photo_id IN  ";
    QStringList qsl;
    foreach (int i, photoIDs) qsl.push_back(QString::number(i));
    QString photoID = "(" + qsl.join(",") + ")";
    query += photoID;
    tagQuery.prepare(query);
    tagQuery.exec();
    qxtLog->debug() << "Query: " << tagQuery.lastQuery();

    QMap< QString, QMap< QString, int > > tagCount;
    while (tagQuery.next())
    {
        tagCount[tagQuery.value(0).toString()][tagQuery.value(1).toString()] = tagCount[tagQuery.value(0).toString()][tagQuery.value(1).toString()] + 1;
        fileMeta[tagQuery.value(0).toString()][tagQuery.value(1).toString()] = tagQuery.value(2).toString();
    }

    // Keep only tags that are present in all of the photoIDs
    // NOTE: text fields may pass through
    foreach (QString tagGroup, tagCount.keys())
    {
        foreach (QString tagName, tagCount[tagGroup].keys())
        {
            if (tagCount[tagGroup][tagName] != photoIDs.count())
                fileMeta[tagGroup].remove(tagName);
        }
        if (fileMeta[tagGroup].count() == 0) fileMeta.remove(tagGroup);
    }
}


void FileManager::getFiles(unsigned folderID, FileList &fileList)
{
    qxtLog->debug() << "Fetching files for folderID: " << folderID;
    QSqlQuery query;
    Filter::FilterType currentFilter = Filter::instance().fileFilter();

    if (currentFilter == Filter::FilterAll)
            query.prepare("SELECT photo.* FROM photo WHERE photo.folder_id = :folderID");
    else if (currentFilter == Filter::FilterTagged)
        query.prepare("SELECT photo.* FROM photo LEFT JOIN tag ON (photo.id = tag.photo_id) WHERE photo.folder_id = :folderID GROUP BY photo.id HAVING tag.id IS NOT NULL");
    else if (currentFilter == Filter::FilterUntagged)
        query.prepare("SELECT photo.* FROM photo LEFT JOIN tag ON (photo.id = tag.photo_id) WHERE photo.folder_id = :folderID GROUP BY photo.id HAVING tag.id IS NULL");

    query.bindValue(":folderID", folderID);
    query.exec();
    while (query.next())
    {
        File f;
        f.id            = query.value(0).toUInt();
        f.folderID      = query.value(1).toUInt();
        f.name          = query.value(2).toString();
        f.updatedOn     = query.value(3).toUInt();
        f.updatedBy     = query.value(4).toUInt();
        f.partitionID   = Folders::instance().folder(f.folderID).partitionID;

        fileList.push_back(f);
    }
}


const Camera &FileManager::currentCamera()
{
    return Cameras::instance().camera(currentCameraName_);
}
