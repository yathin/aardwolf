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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>

#include "Camera/camera.h"
#include "Folder/folder.h"
#include "Project/project.h"

typedef QMap< QString, QMap< QString, QString > > FileMeta;

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = 0);

    enum FileLevel
    {
        LevelProject,
        LevelCamera,
        LevelFolder,
        LevelFiles
    };

    void openDir(const QString &dirName);

    FileLevel currentLevel();
    void setLevel(FileLevel fileLevel);

    const QString& currentFolderName();

    const File &file(unsigned index);

    void    getFileMeta(QVector< int > photoIDs, FileMeta &fileMeta);

    void    getFiles(unsigned folderID, FileList& fileList);

    const Camera& currentCamera();

private:
    FileLevel   currentLevel_;

    QString     currentCameraName_;
    QString     currentFolderName_;
    FileList    fileList_;

    int         totalPages_;
    int         currentPage_;
    void        getFileIndex(int& begin, int& end, int page = 1);

signals:
    
public slots:

    void        pageChanged(int page);
    
};

#endif // FILEMANAGER_H
