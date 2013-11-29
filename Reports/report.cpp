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

#include "report.h"
#include "ui_report.h"
#include "ui_createreport.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>

Report::Report(QWidget *parent) :
    QDialog(parent),
    reportModel_(0),
    queryModel_(0),
    ui(new Ui::Report),
    uiCreateReport(new Ui::CreateReport),
    createReport_(0)
{
    ui->setupUi(this);
    reportModel_ = new QSqlQueryModel;
    reportQuery_ = "SELECT id, name, query FROM reports";
    reportModel_->setQuery(reportQuery_);
    this->ui->reports->setModel(reportModel_);
    this->ui->reports->hideColumn(0);
    this->ui->reports->hideColumn(2);
    reportModel_->setHeaderData(1, Qt::Horizontal, tr("Report Name"));
    this->ui->reports->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    createReport_ = new QDialog(this);
    uiCreateReport->setupUi(createReport_);

    connect(this->ui->reports, SIGNAL(pressed(QModelIndex)), this, SLOT(recordSelected(QModelIndex)));
    connect(uiCreateReport->buttonBox, SIGNAL(accepted()), this, SLOT(on_buttonBox_accepted()));
}

void Report::recordSelected(QModelIndex index)
{
    selectedIndex_ = index;
    this->ui->query->setPlainText(reportModel_->record(index.row()).value("query").toString());
}

Report::~Report()
{
    if (queryModel_ != 0)
        delete queryModel_;
    delete reportModel_;
    delete createReport_;
    delete ui;
    delete uiCreateReport;
}

void Report::createDefaultReports()
{

}

void Report::on_buttonDone_clicked()
{
    emit this->close();
}

void Report::on_buttonRun_clicked()
{
    if (queryModel_ != 0)
        delete queryModel_;
    queryModel_ = new QSqlQueryModel;
    queryModel_->setQuery(this->ui->query->toPlainText());
    this->ui->report->setModel(queryModel_);
}

void Report::on_buttonNewReport_clicked()
{
    uiCreateReport->reportName->clear();
    uiCreateReport->reportQuery->clear();
    createReport_->show();
}

void Report::on_buttonDelReport_clicked()
{
    int ret = QMessageBox::warning(this, "Delete Report?", "Are you sure you want to delete this report?", QMessageBox::Yes|QMessageBox::No);
    if (ret == QMessageBox::No)
        return;

    QSqlQuery delReport;
    delReport.prepare("DELETE FROM reports WHERE id = :id");
    delReport.bindValue(":id", reportModel_->record(selectedIndex_.row()).value("id").toInt());
    delReport.exec();

    this->ui->query->clear();
    reportModel_->setQuery(reportQuery_);
}

void Report::on_buttonBox_accepted()
{
    if (uiCreateReport->reportName->text().isEmpty() || uiCreateReport->reportQuery->toPlainText().isEmpty())
    {
        QMessageBox::critical(this, "Bad Input", "Report name or query cannot be empty. Please retry.", QMessageBox::Ok);
        return;
    }

    QSqlQuery newReport;
    newReport.prepare("INSERT INTO reports(name, query) VALUES (:name, :query)");
    newReport.bindValue(":name", uiCreateReport->reportName->text());
    newReport.bindValue(":query", uiCreateReport->reportQuery->toPlainText());
    newReport.exec();
    reportModel_->setQuery(reportQuery_);
}



void Report::on_buttonSave_clicked()
{
    int ret = QMessageBox::warning(this, "Update Report?", "Are you sure you want to update this report?", QMessageBox::Yes|QMessageBox::No);
    if (ret == QMessageBox::No)
        return;

    QSqlQuery updateReport;
    updateReport.prepare("UPDATE reports SET query = :query WHERE id = :id");
    updateReport.bindValue(":query", ui->query->toPlainText());
    updateReport.bindValue(":id", reportModel_->record(selectedIndex_.row()).value("id").toInt());
    updateReport.exec();

    reportModel_->setQuery(reportQuery_);
}
