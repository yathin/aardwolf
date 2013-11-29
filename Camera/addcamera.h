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

#ifndef ADDCAMERA_H
#define ADDCAMERA_H

#include <QDialog>
#include <Camera/camera.h>

namespace Ui {
class AddCamera;
}

class AddCamera : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddCamera(const QString& cameraName = QString(), QWidget *parent = 0);
    ~AddCamera();
    
private slots:
    void on_cancelButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::AddCamera   *ui;
    Camera          camera_;
};

#endif // ADDCAMERA_H
