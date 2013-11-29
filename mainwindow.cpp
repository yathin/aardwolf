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

// QT includes
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QVariant>
#include <QGraphicsObject>

// Qxt includes
#include <QxtLogger>

// Limoo includes
#include "Limoo/limoo.h"
#include "Limoo/settings.h"
#include "Limoo/thumbnailbaritem.h"

// Aardwolf includes
#include "Camera/addcamera.h"
#include "Camera/download.h"
#include "Data/datatableview.h"
#include "Etc/aboutdialog.h"
#include "Helpers/filemanager.h"
#include "Helpers/filter.h"
#include "Helpers/historybar.h"
#include "Helpers/loggerengine.h"
#include "Helpers/mediator.h"
#include "Image/ImageMetaHandler.h"
#include "Project/createproject.h"
#include "Project/project.h"
#include "Project/partitionmanager.h"
#include "Reports/report.h"
#include "Tag/edittagdefintion.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

// Globals
Limoo *w            = NULL;
ImageMetaHandler *i = NULL;
FileManager *f      = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showMaximized();

    // Setup Logging
    qxtLog->addLoggerEngine("aardwolf", new LoggerEngine);
    qxtLog->disableAllLogLevels();
    //qxtLog->setMinimumLevel(QxtLogger::DebugLevel);

    // Setup recent Projects
    refreshRecentProjectList();

    // Init Aardwolf Widgets
    HistoryBar::init(this->ui->historyBar);
    this->setToolbars();

    Mediator::instance().registerMainWindow(this);
    Mediator::instance().setStatus(tr("Open or create a project to begin"));

    // Check most recently used grouper
    // TODO: Move to separate function during grouper code refactoring.
    QSettings settings;
    QString grouper = settings.value("grouper").toString();
    if (grouper.isEmpty())
        this->ui->actionNone->setChecked(true);
    else if (grouper == "auto")
        this->ui->actionAuto->setChecked(true);
}

void MainWindow::setToolbars()
{
    this->ui->fileToolBar->addAction(this->ui->action_New_Project);
    this->ui->fileToolBar->addAction(this->ui->action_Open_Project);
    this->ui->fileToolBar->addAction(this->ui->actionClose_Project);

    this->ui->viewToolBar->addAction(this->ui->actionFilmstrip);
    this->ui->viewToolBar->addAction(this->ui->actionPreview);
    this->ui->viewToolBar->addWidget(this->ui->zoomLabel);
    this->ui->viewToolBar->addWidget(this->ui->thumbnailSize);

    this->ui->projectToolBar->addAction(this->ui->actionNew_Camera);
    this->ui->projectToolBar->addAction(this->ui->actionCreateTag);
    this->ui->projectToolBar->addSeparator();
    this->ui->projectToolBar->addAction(this->ui->actionAdd_Photographs);

    this->ui->dataToolBar->addAction(this->ui->actionView);
    this->ui->dataToolBar->addWidget(this->ui->searchText);
    this->ui->dataToolBar->addWidget(this->ui->search);

    this->ui->statusbar->addWidget(this->ui->status);
}

void MainWindow::openLimoo()
{
    qxtLog->debug() << "Opening image viewer (limoo)";
    w = new Limoo(ui->LimooContainer);
    i = new ImageMetaHandler();
    f = new FileManager();

    Mediator::instance().registerFileManager(f);
    Mediator::instance().registerLimoo(w);
    Mediator::instance().registerImageMetaHandler(i);

    // Setup signals and slots
    connect(w, SIGNAL(imageLoaded(int,QString,QString)), i, SLOT(imageLoaded(int,QString,QString)));
    connect(w, SIGNAL(imageLoaded(QVector<int>,QVector<QString>,QString)), i, SLOT(imageLoaded(QVector<int>,QVector<QString>,QString)));
    connect(w, SIGNAL(imageClosed()), i, SLOT(imageClosed()));
    connect(this->ui->page, SIGNAL(valueChanged(int)), f, SLOT(pageChanged(int)));

    f->openDir(CurrentProject::instance().project().location);

    Mediator::instance().setStatus(QString("Opened project: ") +  CurrentProject::instance().project().name);

    w->activateWindow();
    w->raise();
}

void MainWindow::closeLimoo()
{
    if (w == NULL)
        return;

    disconnect(i, SLOT(imageLoaded(int,QString,QString)));
    disconnect(i, SLOT(imageLoaded(QVector<uint>,QVector<QString>,QString)));
    disconnect(i, SLOT(imageClosed()));

    delete w;
    delete i;
    delete f;

    w = NULL;
    i = NULL;
    f = NULL;
    Mediator::instance().setStatus(tr("Open or create a project to begin"));
}

MainWindow::~MainWindow()
{
    CurrentProject::instance().close();

    if  (QSqlDatabase::database().isOpen())
        QSqlDatabase::database().close();

    delete ui;
}

void MainWindow::on_actionNew_Camera_triggered()
{
    AddCamera addCamera("", this);
    addCamera.exec();
}

void MainWindow::on_action_New_Project_triggered()
{
    CreateProject createProject(this);
    createProject.exec();
}

void MainWindow::on_action_Open_Project_triggered()
{
    QString aardwolfProject = QFileDialog::getOpenFileName(this, tr("Open Aardwolf project"), "", "*.aardwolf");

    if (aardwolfProject.isEmpty())
        return;

    this->openCurrentProject(aardwolfProject);
}

void MainWindow::on_actionClose_Project_triggered()
{
    this->closeCurrentProject();
}

void MainWindow::on_actionCreateTag_triggered()
{
    EditTagDefinition editTag;
    editTag.exec();
}

void MainWindow::clearMenuActions(QMenu *menu, QVector< QAction * > actions)
{
    for (QVector< QAction *>::iterator i = actions.begin(); i != actions.end(); ++i)
    {
        menu->removeAction(*i);
        delete *i;
    }
}

void MainWindow::clearTagMenu()
{
    clearMenuActions(this->ui->menuTag, tagMenuActions_);
    for (QMap< QString, QAction *>::iterator i = tagViewMenuActions_.begin();
             i != tagViewMenuActions_.end(); ++i)
    {
        this->ui->menuViewTag->removeAction(i.value());
        delete *i;
    }
    tagViewMenuActions_.clear();
    tagMenuActions_.clear();
}

void MainWindow::clearCameraMenu()
{
    clearMenuActions(this->ui->menuCameras, cameraMenuActions_);
    cameraMenuActions_.clear();
}

void MainWindow::refreshImageMetaHandler()
{
    i->refresh();
}

void MainWindow::refreshTagMenuActions()
{
    clearTagMenu();
    QList< TagGroup > tagGroups;
    CurrentProject::instance().project().getTagDefinition().getTagGroups(tagGroups);
    for (QList< TagGroup >::iterator i = tagGroups.begin(); i != tagGroups.end(); ++i)
    {
        QAction *action = new QAction(*i, this);
        QAction *viewAction = new QAction(*i, this);
        viewAction->setCheckable(true);
        viewAction->setChecked(true);
        connect(action, SIGNAL(triggered()), this, SLOT(editTag()));
        connect(viewAction, SIGNAL(triggered()), this, SLOT(refreshImageMetaHandler()));
        tagMenuActions_.push_back(action);
        tagViewMenuActions_[*i] = viewAction;
        this->ui->menuTag->addAction(action);
        this->ui->menuViewTag->addAction(viewAction);
    }
}

void MainWindow::refreshCameraMenuActions()
{
    QMenu *cameraMenu = this->ui->menuCameras;
    clearCameraMenu();

    const CameraList cameraList = Cameras::instance().getCameraList();
    for (CameraList::const_iterator i = cameraList.begin(); i != cameraList.end(); ++i)
    {
        QAction *action = new QAction(i->name, this);
        qxtLog->debug() << "Adding camera action: " << i->name;
        connect(action, SIGNAL(triggered()), this, SLOT(editCamera()));
        cameraMenuActions_.push_back(action);
        cameraMenu->addAction(action);
    }
}

void MainWindow::editTag()
{
    QAction *action = qobject_cast< QAction * >(sender());
    if (action)
    {
        EditTagDefinition editTag(action->text());
        editTag.exec();
    }
}

void MainWindow::editCamera()
{
    QAction *action = qobject_cast< QAction * >(sender());
    if (action)
    {
        AddCamera addCamera(action->text());
        addCamera.exec();
    }
}

void MainWindow::on_actionAdd_Photographs_triggered()
{
    Download download;
    download.exec();
}

void MainWindow::closeCurrentProject()
{
    if (i)
        i->imageClosed();

    CurrentProject::instance().close();
    clearTagMenu();
    clearCameraMenu();

    HistoryBar::instance().setProject("");

    closeLimoo();

    // Disable menus and actions
    this->ui->menuProject->setEnabled(false);
    this->ui->actionClose_Project->setEnabled(false);
    this->ui->actionAdd_Photographs->setEnabled(false);
    this->ui->menuCameras->setEnabled(false);
    this->ui->menuTag->setEnabled(false);
    this->ui->actionView->setEnabled(false);
    this->ui->actionPartitions->setEnabled(false);
    this->ui->actionReports->setEnabled(false);
    this->ui->menuGrouping->setEnabled(false);
    this->ui->menuViewTag->setEnabled(false);
    this->ui->menuImages->setEnabled(false);
    this->ui->actionNew_Camera->setEnabled(false);
    this->ui->actionCreateTag->setEnabled(false);
    this->ui->actionFilmstrip->setEnabled(false);
    this->ui->actionPreview->setEnabled(false);
    this->ui->limooControl->setEnabled(false);
    this->ui->thumbnailSize->setEnabled(false);
    this->ui->search->setEnabled(false);
    this->ui->searchText->setEnabled(false);
    this->ui->zoomLabel->setEnabled(false);


    this->ui->FileFilter->setEnabled(false);
    this->ui->DirFilter->setEnabled(false);

    this->ui->limooControl->setEnabled(false);
}

void MainWindow::openCurrentProject(const QString& fileName)
{
    closeCurrentProject();
    CurrentProject::instance().initFromFile(fileName);

    Cameras::instance().refresh();
    Folders::instance().refresh();

    openLimoo();

    this->ui->thumbnailSize->setValue(w->settings()->value("general/preview_size").toInt());

    HistoryBar::instance().setProject(CurrentProject::instance().project().name);

    this->refreshTagMenuActions();
    this->refreshCameraMenuActions();
    this->setRecentProject();

    // Enable menus and actions
    this->ui->menuProject->setEnabled(true);
    this->ui->actionClose_Project->setEnabled(true);
    this->ui->actionAdd_Photographs->setEnabled(true);
    this->ui->menuCameras->setEnabled(true);
    this->ui->menuTag->setEnabled(true);
    this->ui->actionView->setEnabled(true);
    this->ui->limooControl->setEnabled(true);
    this->ui->actionPartitions->setEnabled(true);
    this->ui->actionReports->setEnabled(true);
    this->ui->menuGrouping->setEnabled(true);
    this->ui->menuViewTag->setEnabled(true);
    this->ui->menuImages->setEnabled(true);
    this->ui->actionNew_Camera->setEnabled(true);
    this->ui->actionCreateTag->setEnabled(true);
    this->ui->actionFilmstrip->setEnabled(true);
    this->ui->actionPreview->setEnabled(true);
    this->ui->limooControl->setEnabled(true);
    this->ui->thumbnailSize->setEnabled(true);
    this->ui->search->setEnabled(true);
    this->ui->searchText->setEnabled(true);
    this->ui->zoomLabel->setEnabled(true);


    Mediator::instance().refreshProjectInfo();
    if (!PartitionManager::partitionCheck())
    {
        int ret = QMessageBox::warning(this, "Invalid Partitions", "Some partitions could not be found. Do you want to fix it?", QMessageBox::No|QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
        {
            PartitionManager partitionManager(this);
            partitionManager.exec();
        }
    }
}

void MainWindow::openRecentProject()
{
    QAction *action = qobject_cast< QAction * >(sender());
    if (action)
    {
        openCurrentProject(action->data().toString() + ".aardwolf");
    }
}

void MainWindow::setRecentProject()
{
    QString currentProjectName = CurrentProject::instance().project().location + "/" + CurrentProject::instance().project().name;
    setWindowFilePath(currentProjectName);

    QSettings settings;
    QStringList recentProjs = settings.value("recentProjects").toStringList();
    recentProjs.removeAll(currentProjectName);
    recentProjs.prepend(currentProjectName);
    while(recentProjs.size() > MaxRecentProjects)
        recentProjs.removeLast();

    settings.setValue("recentProjects", recentProjs);

    refreshRecentProjectList();
}

void MainWindow::refreshRecentProjectList()
{
    foreach (QAction *action, recentProjectActions_)
    {
        // disconnect(this, SIGNAL(triggered()), this, SLOT(openRecentProject()));
        delete action;
    }
    recentProjectActions_.clear();

    QSettings settings;
    QStringList recentProjs = settings.value("recentProjects").toStringList();

    foreach (QString project, recentProjs)
    {
        QAction *action = new QAction(QFileInfo(project).fileName(), this);
        action->setData(project);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentProject()));
        recentProjectActions_.push_back(action);
        this->ui->menuRecent_Projects->addAction(action);
    }
}

void MainWindow::on_actionView_triggered()
{
    DataTableView dataTableView(this);
    dataTableView.exec();
}

void MainWindow::on_FileFilter_1_clicked()
{
    Filter::instance().setFileFilter(Filter::FilterAll);
    f->setLevel(FileManager::LevelFolder);
}

void MainWindow::on_FileFilter_2_clicked()
{
    Filter::instance().setFileFilter(Filter::FilterTagged);
    f->setLevel(FileManager::LevelFolder);
}

void MainWindow::on_FileFilter_3_clicked()
{
    Filter::instance().setFileFilter(Filter::FilterUntagged);
    f->setLevel(FileManager::LevelFolder);
}

void MainWindow::on_DirFilter_1_clicked()
{
    Filter::instance().setFolderFilter(Filter::FilterAll);
    f->setLevel(FileManager::LevelCamera);
}

void MainWindow::on_DirFilter_2_clicked()
{
    Filter::instance().setFolderFilter(Filter::FilterTagged);
    f->setLevel(FileManager::LevelCamera);
}

void MainWindow::on_DirFilter_3_clicked()
{
    Filter::instance().setFolderFilter(Filter::FilterUntagged);
    f->setLevel(FileManager::LevelCamera);
}

void MainWindow::on_thumbnailSize_valueChanged(int value)
{
    if (w->getPreviewStyle() == Limoo::Preview)
    {
        w->settings()->setValue("general/preview_size", value);
        w->rootObject()->setProperty("preview_size", value *  160  / 50);
    }
    else
    {
        w->settings()->setValue("general/small_preview_size", value);
        w->rootObject()->setProperty("small_preview_size", value *  160  / 50);
    }
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog about;
    about.exec();
}

void MainWindow::on_actionFilmstrip_triggered()
{
    if (w)
    {
        w->setPreviewStyle(Limoo::FilmStrip);
        this->ui->thumbnailSize->setValue(w->settings()->value("general/small_preview_size").toInt() );
    }
}

void MainWindow::on_actionPreview_triggered()
{
    if (w)
    {
        w->setPreviewStyle(Limoo::Preview);
        this->ui->thumbnailSize->setValue(w->settings()->value("general/preview_size").toInt() );
    }
}

bool MainWindow::isTagViewEnabled(const QString &tagGroup)
{
    if (tagViewMenuActions_[tagGroup]->isChecked())
        return true;
    return false;
}

void MainWindow::splitterResized(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);
    w->resize(this->ui->LimooContainer->size().width(), this->ui->LimooContainer->size().height());
}

void MainWindow::on_radio_log_off_clicked()
{
    qxtLog->disableAllLogLevels();
}

void MainWindow::on_radio_log_debug_clicked()
{
    qxtLog->setMinimumLevel(QxtLogger::DebugLevel);
}

void MainWindow::on_radio_log_error_clicked()
{
    qxtLog->setMinimumLevel(QxtLogger::ErrorLevel);
}


void MainWindow::on_searchText_returnPressed()
{
    f->openDir("AardwolfSearch: " + this->ui->searchText->text());
}

void MainWindow::on_search_clicked()
{
    f->openDir("AardwolfSearch: " + this->ui->searchText->text());
}


void MainWindow::on_actionReports_triggered()
{
    Report report(this);
    report.exec();
}

void MainWindow::on_actionPartitions_triggered()
{
    PartitionManager partitionManager(this);
    partitionManager.exec();
}

void MainWindow::on_actionNone_triggered()
{
    // TODO: Make this extendable to include other types of groupers
    QSettings settings;
    settings.setValue("grouper", "");
    this->ui->actionAuto->setChecked(false);
    this->ui->actionNone->setChecked(true);
}

void MainWindow::on_actionAuto_triggered()
{
    QSettings settings;
    settings.setValue("grouper", "auto");
    this->ui->actionAuto->setChecked(true);
    this->ui->actionNone->setChecked(false);
}
