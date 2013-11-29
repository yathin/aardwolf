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

#ifndef HISTORYBAR_H
#define HISTORYBAR_H

#include <QEvent>
#include <QLayout>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <QPushButton>
#include <QWidget>

class FileManager;

class HistoryBar : public QObject
{
    Q_OBJECT

public:
    static HistoryBar&  instance();
    static void         init(QWidget* container);
    static void         close();

    void    setProject(const QString& project);
    void    setCamera(const QString& camera);
    void    setFolder(const QString& folder);
    void    setSearch(const QString& search);

    ~HistoryBar();
private:
    HistoryBar();
    void    refreshLayout();
    void    setLabel(QPushButton *label, const QString& text);


    QWidget             *container_;

    QPushButton         *projectIcon_;
    QPushButton         *cameraIcon_;
    QPushButton         *folderIcon_;
    QPushButton         *searchIcon_;

    QHBoxLayout         *hLayout_;

    FileManager         *fileManager_;

    static HistoryBar   *historyBar;

signals:

private slots:
    void                projectClicked();
    void                cameraClicked();
    void                folderClicked();
    void                searchClicked();

public slots:

};

#endif // HISTORYBAR_H
