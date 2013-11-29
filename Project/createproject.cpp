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

#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QxtLogger>

#include <sstream>

#include "createproject.h"
#include "project.h"
#include "ui_createproject.h"

CreateProject::CreateProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateProject)
{
    ui->setupUi(this);
}

CreateProject::~CreateProject()
{
    delete ui;
}

void CreateProject::on_chooseFolder_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this->parentWidget(), tr("Choose Location"), tr(""));
    this->ui->projectLocation->setText(dirName);
    qxtLog->debug() << "Creating Project in Directory: " << dirName;
}

void CreateProject::on_CancelButton_clicked()
{
    qxtLog->debug() << "Project creation cancelled";
    emit this->close();
}

void CreateProject::on_CreateButton_clicked()
{
    // Validate input
    QRegExp projectNameRegex("[a-zA-Z0-9-]+");
    QRegExpValidator projectNameValidator(projectNameRegex, 0);
    int pos = 0;
    project_.name = this->ui->projectName->text();
    if (projectNameValidator.validate(project_.name, pos) != QValidator::Acceptable)
    {
        qxtLog->error() << "Invalid Project Name: " << project_.name;
        QMessageBox::critical(this, tr("Invalid project name."), tr("Invalid project name. Valid names can only consist of letters A-Z and number 0-9"), QMessageBox::Ok);
        return;
    }

    project_.location = this->ui->projectLocation->text();
    if (project_.location.isEmpty())
    {
        qxtLog->error() << "Directory for project is empty";
        QMessageBox::critical(this, tr("Invalid path."), tr("Please choose a valid location for the project."), QMessageBox::Ok);
        return;
    }

    project_.info = this->ui->projectInfo->toPlainText();
    project_.xmp  = (this->ui->storeInXMP->isChecked()) ? true : false;
    project_.create();

    qxtLog->debug() << "Creating Project: " << project_.name << " in " << project_.location << ". XMP support: " << project_.xmp;

    emit this->close();
}

