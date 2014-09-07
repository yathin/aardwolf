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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QDialog>
#include <QSqlQueryModel>


namespace Ui {
class Download;
}

class Download : public QDialog
{
    Q_OBJECT
    
public:
    explicit Download(QWidget *parent = 0);
    ~Download();
    
private slots:

    void on_buttonChooseSource_clicked();

    void on_cameraList_pressed(const QModelIndex &index);

    void on_buttonClose_clicked();

    void on_buttonDownload_clicked();

private:
    Ui::Download *ui;

    QSqlQueryModel *cameraModel_;
    QSqlQueryModel *partitionModel_;
    QModelIndex     selectedCameraIndex_;
    QString         cameraName_;

    void            doDownload();
};

#endif // DOWNLOAD_H
