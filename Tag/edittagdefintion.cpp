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

#include <QSqlRecord>
#include <QSqlQuery>
#include <QString>
#include <QMessageBox>
#include <QWidget>
#include <QRegExpValidator>

#include "Helpers/mediator.h"
#include "Image/xmp.h"
#include "Project/project.h"

#include "edittagdefintion.h"
#include "ui_createtag.h"
#include "ui_edittagdefintion.h"

EditTagDefinition::EditTagDefinition(const QString& groupName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTagDefinition),
    uiTag(new Ui::CreateTag),
    model_(0),
    isCreated_(false),
    groupName_(groupName),
    filter_(0),
    createTag_(0)
{
    ui->setupUi(this);

    createTag_ = new QWidget;
    uiTag->setupUi(createTag_);
    uiTag->name->setFocus();
    connect(uiTag->buttonAdd, SIGNAL(clicked()), this, SLOT(on_buttonAdd_clicked()));
    connect(uiTag->buttonCancel, SIGNAL(clicked()), this, SLOT(on_buttonCancel_clicked()));
    connect(uiTag->name, SIGNAL(editingFinished()), this, SLOT(on_uiTag_name_Edited()));

    this->model_ = new QSqlTableModel();
    model_->setTable("tag_definition");
    model_->setEditStrategy(QSqlTableModel::OnFieldChange);

    this->filter_ = new ReadOnlyFilterProxyModel(this);
    QList< int > readOnlyCols;
    readOnlyCols.append(model_->fieldIndex("name"));
    readOnlyCols.append(model_->fieldIndex("shortcut"));
    readOnlyCols.append(model_->fieldIndex("value_type"));
    readOnlyCols.append(model_->fieldIndex("xmp_name"));
    filter_->setColumns(readOnlyCols);

    filter_->setSourceModel(model_);
    this->ui->table->setModel(filter_);

    this->ui->table->hideColumn(0);
    this->ui->table->hideColumn(5);
    this->ui->table->hideColumn(6);
    this->ui->table->hideColumn(7);
    this->ui->table->hideColumn(8);

    this->ui->table->horizontalHeader()->moveSection(1,2);
    this->ui->table->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    this->ui->table->horizontalHeader()->setResizeMode(4, QHeaderView::Stretch);

    model_->setFilter("group_name='"+this->groupName_+"'");

    if (!groupName_.isEmpty())
    {
        this->ui->groupName->setText(groupName);
        this->groupCreated();
        this->ui->buttonCreateRemove->hide();
    }

    model_->select();
    model_->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model_->setHeaderData(2, Qt::Horizontal, tr("Value Type"));
    model_->setHeaderData(3, Qt::Horizontal, tr("Shortcut"));
    model_->setHeaderData(4, Qt::Horizontal, tr("XMP Name"));
}

EditTagDefinition::~EditTagDefinition()
{   
    delete model_;
    delete filter_;
    delete uiTag;
    delete createTag_;
    delete ui;
}


void EditTagDefinition::on_buttonCreateRemove_clicked()
{
    if (this->ui->groupName->text().isEmpty())
    {
        qxtLog->error() << "Creation of empty group requested";
        QMessageBox::critical(this, tr("Empty Group"), tr("Cannot create empty group"), QMessageBox::Ok);
        return;
    }

    QRegExp regEx("[A-Za-z]+");
    QRegExpValidator validator(regEx, 0);
    QString group = ui->groupName->text();
    int pos = 0;
    if (!validator.validate(group, pos))
    {
        QMessageBox::critical(this, tr("Invalid Tag Group"),
                                    tr("Tag group name must consist only of characters from A to Z."),
                                    QMessageBox::Ok);
        return;
    }

    if (!isCreated_)
    {
        QSqlQuery groupNameQuery;
        groupNameQuery.prepare("SELECT group_name FROM tag_definition GROUP BY group_name");
        groupNameQuery.exec();

        bool nameExists = false;
        while (groupNameQuery.next())
        {
            if (this->ui->groupName->text() == groupNameQuery.value(0).toString())
                nameExists = true;
        }

        groupNameQuery.finish();

        if (nameExists)
        {
            QMessageBox::critical(this, tr("Group Already Exists"),
                                       tr(QString("Tag group '" + this->ui->groupName->text() + "' already exists. Please choose a different name.").toStdString().c_str()),
                                       QMessageBox::Ok);
            return;
        }
        this->groupCreated();
        this->ui->buttonCreateRemove->hide();
    }
}

void EditTagDefinition::groupCreated()
{
    groupName_ = this->ui->groupName->text();
    this->ui->table->setEnabled(true);
    this->ui->buttonAddTag->setEnabled(true);
    this->ui->buttonRemoveTag->setEnabled(true);
    this->ui->groupName->setEnabled(false);
    this->ui->buttonCreateRemove->setText("Remove Group");
    isCreated_ = true;
    model_->setFilter("group_name='"+this->groupName_+"'");
}

void EditTagDefinition::on_buttonDone_clicked()
{
    // Note: Does not refresh if the window is closed by other means.
    CurrentProject::instance().project().refreshTagDefinition();
    Mediator::instance().refreshTagMenuActions();
    emit this->close();
}

void EditTagDefinition::on_buttonAddTag_clicked()
{
    uiTag->name->setText("");
    uiTag->shortcut->setText("");
    uiTag->type->setCurrentIndex(0);
    uiTag->label_XMP->setText("");
    uiTag->name->setFocus();
    createTag_->show();
}

void EditTagDefinition::on_buttonAdd_clicked()
{
    QRegExp regEx("[A-Za-z]+");
    QRegExpValidator validator(regEx, 0);
    QString tagName = uiTag->name->text();
    int pos = 0;
    if (!validator.validate(tagName, pos))
    {
        QMessageBox::critical(this, tr("Invalid tag name"),
                                    tr("Tag name must consist only of characters from A to Z."),
                                    QMessageBox::Ok);
        return;
    }

    bool nameExists = false;
    for (int i = 0; i < model_->rowCount(); ++i)
    {
        if (model_->record(i).value(1).toString().toLower() == uiTag->name->text().toLower())
        {
            nameExists = true;
            break;
        }
    }

    if (nameExists)
    {
        QMessageBox::critical(this, tr("Invalid tag name"),
                                    tr("Tag names must be unique. Please choose another name."),
                                    QMessageBox::Ok);
        return;
    }

    qxtLog->debug() << "Adding definition for new tag: " << uiTag->name->text();

    // NOTE: QT does not set default values for fields that are not set here.
    // TODO: Fix above 'bug' (not really used anywhere, so low priority)
    QSqlRecord record = model_->record();
    record.setValue("group_name",   ui->groupName->text());
    record.setValue("name",         uiTag->name->text());
    record.setValue("shortcut",     uiTag->shortcut->text());
    record.setValue("value_type",   uiTag->type->currentText());
    record.setValue("xmp_name",     uiTag->label_XMP->text());
    this->model_->insertRecord(-1, record);
    createTag_->hide();
}

void EditTagDefinition::on_buttonCancel_clicked()
{
    createTag_->hide();
}

void EditTagDefinition::on_uiTag_name_Edited()
{
    QString xmpName;
    XMP::makeXMPName(this->ui->groupName->text(), this->uiTag->name->text(), xmpName);
    uiTag->label_XMP->setText(xmpName);
}

void EditTagDefinition::on_buttonRemoveTag_clicked()
{
    QItemSelectionModel *select = this->ui->table->selectionModel();
    if (!select->hasSelection())
        return;

    int ret = QMessageBox::warning(this, "Remove?", "Are you sure you want to remove the selected tags?", QMessageBox::No|QMessageBox::Yes);
    if (ret == QMessageBox::No)
        return;

    foreach (QModelIndex mi, select->selectedRows())
    {
        this->model_->removeRow(mi.row());
    }

}
