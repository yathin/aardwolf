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

#include <fstream>
#include <string>

#include <QSqlQuery>
#include <QFileDialog>
#include <QxtJSON>

#include "Helpers/mediator.h"

#include "datatableview.h"
#include "ui_datatableview.h"

DataTableView::DataTableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataTableView),
    queryModel_(0)
{
    ui->setupUi(this);
    queryModel_ = new QSqlQueryModel;
    queryModel_->setQuery("SELECT photo_id, fs_location, camera_name, folder_name, photo_name, photo_time, tags FROM data_basic LIMIT 1000");
    this->ui->table->setModel(queryModel_);
    this->ui->table->horizontalHeader()->setResizeMode(5, QHeaderView::Stretch);
}

DataTableView::~DataTableView()
{
    delete queryModel_;
    delete ui;
}

void DataTableView::on_buttonDone_clicked()
{
    emit this->close();
}

void DataTableView::vectorToCSVFile(std::ofstream &of, const QVector<QString> &v)
{
    bool first = true;
    foreach (QString field, v)
    {
        if (!first)
            of << ",";
        else
            first = false;

        // TODO: escape field?
        std::string oField = std::string("\"") + field.toStdString() + std::string("\"");
        of << oField.c_str();
    }
    of << "\n";
}

void DataTableView::on_buttonExportCVS_clicked()
{

    QString csvFile = QFileDialog::getSaveFileName(this, "Choose Filename", "", "*.csv");
    if (csvFile.isEmpty())
        return;

    QMap< QString, int > tagIndex;
    std::ofstream out(csvFile.toStdString().c_str());

    const TagDefinition& tagDefinition = CurrentProject::instance().project().getTagDefinition();

    QList< TagGroup > tagGroups;
    tagDefinition.getTagGroups(tagGroups);

    QVector< QString > columnNames;
    columnNames.push_back("photo_id");
    columnNames.push_back("fs_location");
    columnNames.push_back("camera_name");
    columnNames.push_back("folder_name");
    columnNames.push_back("photo_name");
    columnNames.push_back("photo_time");

    foreach (TagGroup m, tagGroups)
    {
        QVector< PhotoTagDefinition > photoTagDef;
        tagDefinition.getPhotoTagDefinition(m, photoTagDef);

        foreach (PhotoTagDefinition pm, photoTagDef)
        {
            QString key = m + "_" + pm.name;
            tagIndex[key] = columnNames.size();
            columnNames.push_back(key);
        }
    }

    this->vectorToCSVFile(out, columnNames);

    QSqlQuery query;
    register int numFields = 7;
    query.prepare("SELECT photo_id, fs_location, camera_name, folder_name, photo_name, photo_time, tags FROM data_basic");
    query.exec();
    while (query.next())
    {
        QVector< QString > row;
        row.resize(columnNames.size());
        for (int columnCount = 0; columnCount <= numFields-2; ++columnCount)
        {
            row[columnCount] = query.value(columnCount).toString();
            row[columnCount] = query.value(columnCount).toString();
        }

        QString tagsJson = QString("{") + query.value(numFields-1).toString() + QString("}");
        QVariant tags = QxtJSON::parse(tagsJson);
        foreach (QString key, tags.toMap().keys())
        {
            row[tagIndex[key]] = tags.toMap()[key].toString();
        }
        this->vectorToCSVFile(out, row);
    }

    out.close();
}
