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

#ifndef REPORT_H
#define REPORT_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class Report;
class CreateReport;
}

class Report : public QDialog
{
    Q_OBJECT
    
public:
    explicit Report(QWidget *parent = 0);
    static void createDefaultReports();
    ~Report();
    
private slots:

    void on_buttonDone_clicked();

    void recordSelected(QModelIndex index);

    void on_buttonRun_clicked();

    void on_buttonNewReport_clicked();

    void on_buttonBox_accepted();

    void on_buttonDelReport_clicked();

    void on_buttonSave_clicked();

private:
    QSqlQueryModel *reportModel_;
    QSqlQueryModel *queryModel_;
    Ui::Report *ui;
    Ui::CreateReport *uiCreateReport;
    QDialog *createReport_;
    QModelIndex selectedIndex_;
    QString reportQuery_;
};

#endif // REPORT_H
