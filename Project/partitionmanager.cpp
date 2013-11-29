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

#include "partitionmanager.h"
#include "ui_partitionmanager.h"
#include "ui_addpartition.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include <Helpers/mediator.h>
#include "Camera/camera.h"

PartitionManager::PartitionManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartitionManager),
    uiAdd(new Ui::AddPartition)
{
    ui->setupUi(this);

    activePartitions_ = new QSqlQueryModel;
    activePartitions_->setQuery("SELECT id, name, location FROM partition WHERE active = 1");
    activePartitions_->setHeaderData(1, Qt::Horizontal, tr("Active Partition"));
    this->ui->activeView->setModel(activePartitions_);
    this->ui->activeView->hideColumn(0);
    this->ui->activeView->hideColumn(2);
    this->ui->activeView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    inactivePartitions_ = new QSqlQueryModel;
    inactivePartitions_->setQuery("SELECT id, name, location FROM partition WHERE active = 0");
    inactivePartitions_->setHeaderData(1, Qt::Horizontal, tr("Inactive Partition"));
    this->ui->inactiveView->setModel(inactivePartitions_);
    this->ui->inactiveView->hideColumn(0);
    this->ui->inactiveView->hideColumn(2);
    this->ui->inactiveView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    addDialog_ = new QDialog;
    uiAdd->setupUi(addDialog_);

    connect(uiAdd->buttonCancelPartition, SIGNAL(clicked()), this, SLOT(on_buttonCancelPartition_clicked()));
    connect(uiAdd->buttonChoosePartition, SIGNAL(clicked()), this, SLOT(on_buttonChoosePartition_clicked()));
    connect(uiAdd->buttonSavePartition, SIGNAL(clicked()), this, SLOT(on_buttonSavePartition_clicked()));
}

void PartitionManager::refreshModels()
{
   activePartitions_->setQuery("SELECT id, name, location FROM partition WHERE active = 1");
   inactivePartitions_->setQuery("SELECT id, name, location FROM partition WHERE active = 0");
}

PartitionManager::~PartitionManager()
{
    delete activePartitions_;
    delete inactivePartitions_;
    delete uiAdd;
    delete addDialog_;
    delete ui;
}

void PartitionManager::on_closeButton_clicked()
{
    emit this->close();
}

void PartitionManager::on_buttonInactive_clicked()
{
    QModelIndex index = this->ui->inactiveView->currentIndex();
    if (!index.isValid())
    {
        // TODO: throw warning
        return;
    }

    int id = inactivePartitions_->record(index.row()).value("id").toInt();

    QString location = inactivePartitions_->record(index.row()).value("location").toString();
    if (!QDir(location).exists())
    {
        int ret = QMessageBox::critical(this, "Invalid Location", "Cannot find Aardwolf partition at given location. Choose new location?", QMessageBox::Cancel|QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
        {
            QString dirName = QFileDialog::getExistingDirectory(this->parentWidget(), tr("Choose Location"), tr(""));
            if (!dirName.isEmpty())
            {
                QSqlQuery query;
                query.prepare("UPDATE partition SET location = :location WHERE id=:id");
                query.bindValue(":location", dirName);
                query.bindValue(":id", id);
                query.exec();
                refreshModels();
                setupNewPartition(dirName);
            }
        }
        else
        {
            return;
        }
    }

    QSqlQuery query;
    query.prepare("UPDATE partition SET active = 1 WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    refreshModels();
}

void PartitionManager::on_buttonActive_clicked()
{
    QModelIndex index = this->ui->activeView->currentIndex();
    if (!index.isValid())
    {
        // TODO: throw warning
        return;
    }

    int id = activePartitions_->record(index.row()).value("id").toInt();
    QSqlQuery query;
    query.prepare("UPDATE partition SET active = 0 WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    refreshModels();
}

void PartitionManager::on_buttonAddPartition_clicked()
{
    uiAdd->partitionLocation->clear();
    uiAdd->partitionName->clear();
    addDialog_->show();
}

void PartitionManager::on_buttonCancelPartition_clicked()
{
    addDialog_->hide();
}

void PartitionManager::on_buttonSavePartition_clicked()
{
    QRegExp regEx("[A-Za-z]+");
    QRegExpValidator validator(regEx, 0);
    QString partitionName =  uiAdd->partitionName->text();
    QString partitionLocation =  uiAdd->partitionLocation->toPlainText();
    int position = 0;
    if (!validator.validate(partitionName, position))
    {
        QMessageBox::critical(addDialog_, "Invalid partition name", "Only characters A-Z are allowed in partition names. Please retry.", QMessageBox::Ok);
        return;
    }

    if (partitionLocation.isEmpty())
    {
        QMessageBox::critical(addDialog_, "Invalid partition location", "Partition location cannot be empty. Please retry.", QMessageBox::Ok);
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO partition(name, location, active) VALUES (:name, :location, 1)");
    query.bindValue(":name", partitionName);
    query.bindValue(":location", partitionLocation);
    query.exec();
    refreshModels();
    addDialog_->hide();
}

void PartitionManager::on_buttonChoosePartition_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this->parentWidget(), tr("Choose Location"), tr(""));
    if (dirName.isEmpty())
        return;
    uiAdd->partitionLocation->setPlainText(dirName);
    setupNewPartition(dirName);
}

void PartitionManager::setupNewPartition(const QString &basePath)
{
    const CameraList cameras = Cameras::instance().getCameraList();
    QDir baseDir(basePath);
    foreach (Camera c, cameras)
    {
        QString dir = basePath + "/" + c.name;
        if (!QDir(dir).exists())
        {
            baseDir.mkdir(c.name);
        }
    }
}

void PartitionManager::setPartitionState(int partitionID, PartitionState state)
{
    int active = (state == ActivePartition) ? 1 : 0;
    QSqlQuery query;
    query.prepare("UPDATE partition SET active = :active WHERE id=:id");
    query.bindValue(":active", active);
    query.bindValue(":id", partitionID);
    query.exec();
}

void PartitionManager::getPartitionLocation(QMap< int, QString >& partitionMap)
{
    QSqlQuery query;
    query.prepare("SELECT id, location FROM partition");
    query.exec();
    while (query.next())
    {
        partitionMap[query.value(0).toInt()] = query.value(1).toString();
        qxtLog->debug() << "Partition Map: " << query.value(0).toInt() << " => " << query.value(1).toString();
    }
}

bool PartitionManager::partitionCheck()
{
    QStringList badPartitions;
    QSqlQuery q1;
    q1.prepare("SELECT id, location FROM partition");
    q1.exec();
    while (q1.next())
    {
        qxtLog->debug() << "Partition Map: " << q1.value(0).toInt() << " => " << q1.value(1).toString();
        if (!QDir(q1.value(1).toString()).exists())
            badPartitions.append(q1.value(0).toString());
    }
    qxtLog->debug() << "Bad Partitions: " << badPartitions.length();
    if (badPartitions.length() == 0)
        return true;

    QSqlQuery query;
    QString badPartitionList = QString("(") + badPartitions.join(",") + QString(")");
    query.prepare("UPDATE partition SET active = 0 WHERE id IN " + badPartitionList);
    qxtLog->debug() << "Setting partitions as inactive: " << badPartitionList;
    query.exec();

    return false;
}
