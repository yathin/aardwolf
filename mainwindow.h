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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QVector>
#include <QAction>

namespace Ui {
class MainWindow;
}

class Mediator;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

    bool isTagViewEnabled(const QString& tagGroup);

    ~MainWindow();

private:
    void clearTagMenu();
    void clearCameraMenu();

    void refreshTagMenuActions();
    void refreshCameraMenuActions();

    enum { MaxRecentProjects = 4 };
    QAction *recentProjects[MaxRecentProjects];
    
private slots:

    void on_actionNew_Camera_triggered();

    void on_action_New_Project_triggered();

    void on_action_Open_Project_triggered();

    void on_actionClose_Project_triggered();

    void on_actionCreateTag_triggered();

    void editTag();

    void editCamera();

    void openRecentProject();

    void on_actionAdd_Photographs_triggered();

    void refreshImageMetaHandler();

    void on_actionView_triggered();

    void on_FileFilter_1_clicked();

    void on_FileFilter_2_clicked();

    void on_FileFilter_3_clicked();

    void on_thumbnailSize_valueChanged(int value);

    void on_DirFilter_1_clicked();

    void on_DirFilter_2_clicked();

    void on_DirFilter_3_clicked();

    void on_actionAbout_triggered();

    void on_actionFilmstrip_triggered();

    void on_actionPreview_triggered();

    void splitterResized(int pos, int index);

    void on_radio_log_off_clicked();

    void on_radio_log_debug_clicked();

    void on_radio_log_error_clicked();

    void on_actionReports_triggered();

    void on_searchText_returnPressed();

    void on_search_clicked();

    void on_actionPartitions_triggered();

    void on_actionNone_triggered();

    void on_actionAuto_triggered();


private:
    Ui::MainWindow *ui;

    QVector< QAction * > tagMenuActions_;
    QVector< QAction * > cameraMenuActions_;
    QVector< QAction * > recentProjectActions_;
    QMap< QString, QAction * > tagViewMenuActions_;

    void    openLimoo();
    void    closeLimoo();

    void    clearMenuActions(QMenu *menu, QVector< QAction * > actions);

    void    setRecentProject();
    void    refreshRecentProjectList();

    void    openCurrentProject(const QString &fileName);
    void    closeCurrentProject();

    void    setToolbars();

    friend class Mediator;
};

#endif // MAINWINDOW_H
