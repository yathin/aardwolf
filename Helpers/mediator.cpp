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

#include "mediator.h"

// Aardwolf class
#include "Limoo/limoo.h"
#include "Limoo/thumbnailbaritem.h"

#include "Helpers/filemanager.h"
#include "Image/imagemetahandler.h"
#include "Groupers/grouping.h"
#include "Reports/report.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDate>
#include <QTime>

Mediator& Mediator::instance()
{
    static Mediator mediator;
    return mediator;
}

Mediator::Mediator(QObject *parent) :
    QObject(parent),
    main_(0),
    limoo_(0),
    imageMeta_(0),
    fileManager_(0)
{
}

void Mediator::registerMainWindow(MainWindow *mainWindow)
{
    main_ = mainWindow;
}

void Mediator::registerFileManager(FileManager *fileManager)
{
    fileManager_ = fileManager;
}

void Mediator::registerImageMetaHandler(ImageMetaHandler *imageMeta)
{
    imageMeta_ = imageMeta;
}

void Mediator::registerLimoo(Limoo *limoo)
{
    limoo_ = limoo;
}

void Mediator::openDir(const QString &dir)
{
    if (!fileManager_)
        return;

    fileManager_->openDir(dir);
}

unsigned Mediator::fileID(unsigned fileIndex)
{
    if (!fileManager_)
        return 0;

    return fileManager_->file(fileIndex).id;
}

unsigned Mediator::folderID(unsigned fileIndex)
{
    if (!fileManager_)
        return 0;
    return fileManager_->file(fileIndex).folderID;
}

void Mediator::refreshProject()
{
    if (!fileManager_)
        return;

    fileManager_->setLevel(FileManager::LevelProject);
}

void Mediator::refreshCamera()
{
    if (!fileManager_)
        return;

    fileManager_->setLevel(FileManager::LevelCamera);
}

void Mediator::refreshFolder()
{
    if (!fileManager_)
        return;

    fileManager_->setLevel(FileManager::LevelFolder);
}

void Mediator::setEXIF(const QString &exif)
{
    if (!main_)
        return;

    main_->ui->exif->setHtml(exif);
}

void Mediator::clearMetaTables()
{
    if (!main_)
        return;

    main_->ui->tables->clear();
}

void Mediator::addMetaTable(QWidget *widget, const QString &tableName)
{
    if (!main_)
        return;

    main_->ui->tables->addTab(widget, tableName);
}

void Mediator::enableDirFilter(bool enable)
{
    if (!main_)
        return;
    main_->ui->DirFilter->setEnabled(enable);
}

void Mediator::enableFileFilter(bool enable)
{
    if (!main_)
        return;
    main_->ui->FileFilter->setEnabled(enable);
}

void Mediator::clearLimoo()
{
    if (!limoo_)
        return;

    limoo_->clear();
}

void Mediator::addFilesToLimoo(const QStringList &files)
{
    if (!limoo_)
        return;

    limoo_->autoAdd(files);
}

void Mediator::setFocusOnLimoo()
{
    if (!main_ || !limoo_)
        return;

    limoo_->setFocus();
}

void Mediator::getCurrentDateTime(QString &datetime)
{
    QTime t = QTime::currentTime();
    QDate d = QDate::currentDate();
    datetime = d.toString(tr("yyyy-MM-dd")) + " " + t.toString(tr("hh:mm:ss"));
}

void Mediator::refreshProjectInfo()
{
    if (!limoo_)
        return;

    QString html;
    CurrentProject::instance().project().getInfoInHTML(html);
}


bool Mediator::isMetaEnabled(const QString &metaName)
{
    if (!main_)
        return false;

    return main_->isTagViewEnabled(metaName);
}

void Mediator::appendLog(const QString &str)
{
    main_->ui->log->append(str);
    main_->ui->log->moveCursor(QTextCursor::End);
}

void Mediator::setStatus(const QString &status)
{
    main_->ui->status->setText(status);
}

bool Mediator::isCurrentLevelCamera()
{
    if (fileManager_->currentLevel() == FileManager::LevelCamera)
        return true;
    return false;
}

bool Mediator::isCurrentLevelFolder()
{
    if (fileManager_->currentLevel() == FileManager::LevelFolder)
        return true;
    return false;
}

bool Mediator::isCurrentLevelProject()
{
    if (fileManager_->currentLevel() == FileManager::LevelProject)
        return true;
    return false;
}

void Mediator::setPageLimits(int min, int max, const QString &pageInfo)
{
    if (!main_)
        return;
    main_->ui->page->setMinimum(min);
    main_->ui->page->setMaximum(max);
    main_->ui->pageLabel->setText(pageInfo);
}

void Mediator::closeFile()
{
    if (!imageMeta_)
        return;

    imageMeta_->imageClosed();
}

void Mediator::applyGrouping()
{
    QSettings settings;
    QString grouper = settings.value("grouper").toString();

    if (grouper.isEmpty())
    {
        // default group number and color
        for (int i = 0; i < limoo_->count(); ++i)
        {
            limoo_->at(i)->setGroup(1);
            limoo_->at(i)->setGroupColor("lightsteelblue");
        }
        return;
    }

    // TODO: Modify to allow different kinds of groupers
    // picking grouping strategy
    GroupingAuto grouping;
    GroupingData groupingData;

    for (int i = 0; i < limoo_->count(); ++i)
    {
        groupingData[i] = limoo_->at(i)->exifMap();
    }

    grouping.group(groupingData, true);
    for (int i = 0; i < limoo_->count(); ++i)
    {
        limoo_->at(i)->setGroup(groupingData[i]["group"].toInt());
        limoo_->at(i)->setGroupColor(groupingData[i]["group_color"].toString());
    }

}

void Mediator::refreshTagMenuActions()
{
    if (this->main_)
        main_->refreshTagMenuActions();
}

void Mediator::refreshCameraMenuActions()
{
    if (this->main_)
        main_->refreshCameraMenuActions();
}

void Mediator::getFileMeta(QVector<int> photoIDs, FileMeta &fileMeta)
{
    if (this->fileManager_)
        return this->fileManager_->getFileMeta(photoIDs, fileMeta);
}

const Camera &Mediator::getCurrentCamera()
{
    return this->fileManager_->currentCamera();
}

