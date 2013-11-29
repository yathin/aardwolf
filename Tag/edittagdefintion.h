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

#ifndef EditTagDefinition_H
#define EditTagDefinition_H

#include <QDialog>
#include <QSqlTableModel>
#include <QWidget>

#include "readonlyfilterproxymodel.h"

// TODO: change name to EditMeta

namespace Ui {
class EditTagDefinition;
class CreateTag;
}

class EditTagDefinition : public QDialog
{
    Q_OBJECT
    
public:
    explicit EditTagDefinition(const QString& groupName=QString(""), QWidget *parent = 0);
    ~EditTagDefinition();
    
private slots:
    void on_buttonDone_clicked();

    void on_buttonAddTag_clicked();

    void on_buttonCreateRemove_clicked();

    void on_buttonAdd_clicked();

    void on_buttonCancel_clicked();

    void on_uiTag_name_Edited();

    void on_buttonRemoveTag_clicked();

public slots:

private:
    Ui::EditTagDefinition       *ui;
    Ui::CreateTag               *uiTag;
    QSqlTableModel              *model_;
    bool                        isCreated_;
    QString                     groupName_;
    ReadOnlyFilterProxyModel    *filter_;
    QWidget                     *createTag_;

    void groupCreated();
};

#endif // EditTagDefinition_H
