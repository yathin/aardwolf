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

#include <QSqlTableModel>
#include <QMessageBox>

#include "Helpers/mediator.h"

#include "addcamera.h"
#include "camera.h"
#include "ui_addcamera.h"

AddCamera::AddCamera(const QString &cameraName, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::AddCamera)
{
    ui->setupUi(this);

    camera_.id = 0;
    if (cameraName.isEmpty())
        return;

    camera_ = Cameras::instance().camera(cameraName);

    this->ui->cameraName->setText(camera_.name);
    this->ui->cameraName->setEnabled(false);
    this->ui->cameraDescName->setText(camera_.descName);
    this->ui->cameraLatitude->setText(camera_.latitude);
    this->ui->cameraLongitude->setText(camera_.longitude);
    this->ui->cameraAltitude->setText(camera_.altitude);
    this->ui->cameraOrientation->setText(camera_.orientation);
    this->ui->additionalInfo->setPlainText(camera_.info);
}

AddCamera::~AddCamera()
{
    delete ui;
}

void AddCamera::on_cancelButton_clicked()
{
    emit this->close();
}

void AddCamera::on_saveButton_clicked()
{
    camera_.projectID = CurrentProject::instance().project().id();
    camera_.name      = this->ui->cameraName->text();
    camera_.descName  = this->ui->cameraDescName->text();
    camera_.latitude  = this->ui->cameraLatitude->text();
    camera_.longitude = this->ui->cameraLongitude->text();
    camera_.altitude  = this->ui->cameraAltitude->text();
    camera_.orientation   = this->ui->cameraOrientation->text();
    camera_.info          = this->ui->additionalInfo->toPlainText();

    if (camera_.id == 0)
    {
        if (!Cameras::instance().addCamera(camera_))
        {
            QMessageBox::critical(this, "Camera Add Failed.", "Could not add camera. Please look at error logs for details.", QMessageBox::Ok);
            return;
        }
    }
    else
    {
        if (!Cameras::instance().editCamera(camera_))
        {
            QMessageBox::critical(this, "Camera Add Failed.", "Could not edit camera. Please look at error logs for details.", QMessageBox::Ok);
            return;
        }
    }

    emit this->close();
}
