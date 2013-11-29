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

#ifndef CREATEPROJECT_H
#define CREATEPROJECT_H

#include <QDialog>

#include "project.h"

namespace Ui {
class CreateProject;
}

/** Create Project Dialog
 */
class CreateProject : public QDialog
{
    Q_OBJECT
    
public:
    explicit CreateProject(QWidget *parent = 0);
    ~CreateProject();
    
private slots:
    void on_chooseFolder_clicked();
    void on_CancelButton_clicked();
    void on_CreateButton_clicked();

private:
    Ui::CreateProject   *ui;
    Project             project_;
};

#endif // CREATEPROJECT_H
