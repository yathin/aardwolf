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

#include <exiv2/image.hpp>
#include <QDate>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSqlRecord>
#include <QTime>
#include <QtConcurrentRun>


#include "Folder/folder.h"
#include "Helpers/mediator.h"
#include "Project/partitionmanager.h"

#include "download.h"
#include "ui_download.h"

Download::Download(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Download)
{
    ui->setupUi(this);

    cameraModel_ = new QSqlQueryModel;
    cameraModel_->setQuery("SELECT id, name, desc_name, info FROM camera");
    cameraModel_->setHeaderData(0, Qt::Horizontal, tr("Camera Name"));
    this->ui->cameraList->setModel(cameraModel_);
    this->ui->cameraList->hideColumn(0);
    this->ui->cameraList->hideColumn(2);
    this->ui->cameraList->hideColumn(3);
    this->ui->cameraList->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    // this->ui->cameraList->resizeColumnsToContents();

    partitionModel_ = new QSqlQueryModel;
    partitionModel_->setQuery("SELECT id, name, location FROM partition WHERE active = 1");
    this->ui->destination->hideColumn(0);
    partitionModel_->setHeaderData(1, Qt::Horizontal, tr("Partition Name"));
    partitionModel_->setHeaderData(2, Qt::Horizontal, tr("Location"));
    this->ui->destination->setModel(partitionModel_);
    this->ui->destination->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    // this->ui->destination->resizeColumnsToContents();
    this->ui->destination->selectRow(0);
}

Download::~Download()
{
    delete cameraModel_;
    delete partitionModel_;
    delete ui;
}

void Download::on_buttonChooseSource_clicked()
{
    QString source = QFileDialog::getExistingDirectory(this, tr("Choose Source"));

    if (!source.isEmpty())
    {
        this->ui->source->setPlainText(source);
        this->ui->buttonDownload->setEnabled(true);
    }
}

void Download::on_cameraList_pressed(const QModelIndex &index)
{
    QString cameraInfo;
    cameraName_ = cameraModel_->record(index.row()).value("name").toString();
    cameraInfo += "<b>Camera Name:</b> " + cameraName_ + "<br/>";
    cameraInfo += "<b>Camera Descriptive Name:</b> " + cameraModel_->record(index.row()).value("desc_name").toString() + "<br/>";
    cameraInfo += "<b>Additional Info:</b> " + cameraModel_->record(index.row()).value("info").toString() + "<br/>";
    this->ui->cameraInfo->setHtml(cameraInfo);
    this->ui->buttonChooseSource->setEnabled(true);
    if (this->ui->destination->rowAt(0) == -1)
    {
        QMessageBox::critical(this, "No Destinations!", "There are no partitions defined to download data into. Please set at least one active partition through Project > Partition Manager.", QMessageBox::Ok);
        emit this->close();
    }
}

void Download::doDownload()
{
    // Choose Partition
    QModelIndex partitionIndex = this->ui->destination->currentIndex();
    if (!partitionIndex.isValid()) partitionIndex = this->ui->destination->indexAt(QPoint(0,0));
    QString partitionBase = partitionModel_->record(partitionIndex.row()).value("location").toString();
    int partitionID = partitionModel_->record(partitionIndex.row()).value("id").toInt();

    if (!QDir(partitionBase).exists())
    {
        // partition not active
        PartitionManager::setPartitionState(partitionID, PartitionManager::InActivePartition);
        QMessageBox::critical(this, "Inactive Partition", "The partition seems to be inactive. Please choose another partition, or activate partition in Partition Manager.", QMessageBox::Ok);
        partitionModel_->setQuery("SELECT id, name, location FROM partition WHERE active = 1");
        return;
    }

    QString destinationDir = partitionBase + "/" + cameraName_;

    if (!QDir(destinationDir).exists())
    {
        // if camera directory does not exist, create one
        QDir(partitionBase).mkdir(cameraName_);
    }

    QString sourceDir = this->ui->source->toPlainText();

    QDir srcDir(sourceDir);
    QDir destDir(destinationDir);

    // Filter only JPG/JPEG Files
    QStringList fileType;
    fileType.push_back("*.JPG");
    fileType.push_back("*.jpg");
    fileType.push_back("*.JPEG");
    fileType.push_back("*.jpeg");
    QStringList list = srcDir.entryList (fileType);
    QStringList dateTimeList;

    // Create temp. directory to move files to
    if (QDir(destinationDir + "/" + "tmp.aardwolf").exists())
        destDir.rmdir("tmp.aardwolf");
    destDir.mkdir("tmp.aardwolf");
    qxtLog->debug() << "Copying " << list.count() << " files to destination: " << destinationDir;

    QProgressDialog progress(tr("Copying..."),
                             tr("Abort"),
                             0,
                             (ulong)list.count(),
                             this
                             );
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    int completed = 0;

    Exiv2::ExifKey dateTimeOriginal("Exif.Photo.DateTimeOriginal");
    Exiv2::ExifKey dateTimeDefault ("Exif.Image.DateTime");


    for (QStringList::iterator i = list.begin(); i != list.end(); i++)
    {
        progress.setValue(++completed);
        if (progress.wasCanceled())
            return;

        QFile::copy(sourceDir + tr("/") + *i, destinationDir+"/tmp.aardwolf/" + *i);

        try
        {
            Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open( QString(destinationDir+"/tmp.aardwolf/" + *i).toStdString() );
            exiv->readMetadata();
            Exiv2::ExifData data = exiv->exifData();

            Exiv2::Value::AutoPtr v = Exiv2::Value::create(Exiv2::asciiString);;
            if      (data.findKey(dateTimeOriginal) != data.end())
            {
                v = (data.findKey(dateTimeOriginal))->getValue();
            }
            else if (data.findKey(dateTimeDefault ) != data.end())
            {
                v = (data.findKey(dateTimeDefault ))->getValue();
            }
            QString dateTime(v->toString().c_str());
            dateTimeList.append(dateTime);
        }
        catch(...)
        {
            QString error = "Aborting copy. Could not read file: ";
            error += *i;
            error += ". Please check image or contact developer.";
            QMessageBox::critical(this, "Aborting", error, QMessageBox::Ok);
            return;
        }
    }

    progress.hide();

    QProgressDialog waitForComplete(tr("Writing to database. Please Wait."),
                             NULL,
                             0,
                             0,
                             this
                             );
    waitForComplete.setWindowModality(Qt::WindowModal);
    waitForComplete.show();

    // Move tmp dir to real place.
    QString toDir = srcDir.dirName();
    if (!this->ui->useSrcFolderName->isChecked())
    {
        QTime t = QTime::currentTime();
        QDate d = QDate::currentDate();
        toDir = d.toString(tr("yyyyMMdd"))+ "-" + t.toString(tr("hhmmss"));
    }
    destDir.rename("tmp.aardwolf", toDir);

    Folder f;
    f.cameraID = Cameras::instance().camera(cameraName_).id;
    f.partitionID = partitionID;
    f.name = toDir;
    f.imagesAll = list.count();
    Folders::instance().addFolder(f, list, dateTimeList);

    waitForComplete.close();

    // Clear out fields
    ui->source->clear();
    ui->buttonDownload->setEnabled(false);
}

void Download::on_buttonDownload_clicked()
{
    doDownload();
}

void Download::on_buttonClose_clicked()
{
    if (ui->buttonDownload->isEnabled())
    {
        int ret = QMessageBox::warning(this, "Download?", "Do you want to close without Downloading?", QMessageBox::Cancel|QMessageBox::Yes);
        if (ret == QMessageBox::Cancel)
            return;
    }
    emit this->close();
}
