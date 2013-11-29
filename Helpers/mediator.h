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

#ifndef MEDIATOR_H
#define MEDIATOR_H

// QT includes
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QxtLogger>

// Limoo includes
#include "Limoo/limoo.h"
#include "Limoo/settings.h"
#include "Limoo/thumbnailbaritem.h"

// Aardwolf includes
#include "Camera/addcamera.h"
#include "Camera/download.h"
#include "Data/datatableview.h"
#include "Image/imagemetahandler.h"
#include "Helpers/filemanager.h"
#include "Helpers/filter.h"
#include "Helpers/historybar.h"
#include "Helpers/loggerengine.h"
#include "Helpers/mediator.h"
#include "Project/createproject.h"
#include "Project/project.h"
#include "Tag/edittagdefintion.h"

class Limoo;
class ImageMetaHandler;
class MainWindow;
class FileManager;

class Mediator : public QObject
{
    Q_OBJECT
public:
    static Mediator& instance();

    void registerMainWindow(MainWindow* mainWindow);
    void registerLimoo(Limoo* limoo);
    void registerImageMetaHandler(ImageMetaHandler *imageMeta);
    void registerFileManager(FileManager *fileManager);

    // ImageMetaHandler functions
    void        closeFile();

    // FileManager Functions
    void        openDir(const QString& dir);
    unsigned    fileID(unsigned fileIndex);
    unsigned    folderID(unsigned fileIndex);
    void        refreshProject();
    void        refreshCamera();
    void        refreshFolder();
    bool        isCurrentLevelProject();
    bool        isCurrentLevelCamera();
    bool        isCurrentLevelFolder();
    void        getFileMeta(QVector<int> photoIDs, FileMeta &fileMeta);
    const Camera &getCurrentCamera();

    // MainWindow UI functions
    void    setEXIF(const QString& exif);
    void    clearMetaTables();
    void    addMetaTable(QWidget *widget, const QString& tableName);
    void    enableFileFilter(bool enable=true);
    void    enableDirFilter(bool enable=true);
    void    setFocusOnLimoo();
    void    refreshProjectInfo();
    bool    isMetaEnabled(const QString& metaName);
    void    refreshTagMenuActions();
    void    refreshCameraMenuActions();
    void    appendLog(const QString& str);
    void    setStatus(const QString& status);
    void    setPageLimits(int min, int max, const QString& pageInfo);

    // Limoo functions
    void    clearLimoo();
    void    addFilesToLimoo(const QStringList& files);
    void    applyGrouping();

    // Etc/Helper functions, for the lack of a better place to put them in
    void        getCurrentDateTime(QString &datetime);

private:
    explicit Mediator(QObject *parent = 0);

    MainWindow          *main_;
    Limoo               *limoo_;
    ImageMetaHandler    *imageMeta_;
    FileManager         *fileManager_;

signals:

public slots:

private slots:

};

#endif // MEDIATOR_H
