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

#ifndef DATATABLEVIEW_H
#define DATATABLEVIEW_H

#include <QDialog>
#include <QMap>
#include <QSqlQueryModel>
#include <QString>
#include <QWidget>

namespace Ui {
class DataTableView;
}

class DataTableView : public QDialog
{
    Q_OBJECT
    
public:
    explicit DataTableView(QWidget *parent = 0);
    ~DataTableView();
    
private slots:
    void on_buttonDone_clicked();

    void on_buttonExportCVS_clicked();

private:
    Ui::DataTableView *ui;
    QSqlQueryModel    *queryModel_;

    void vectorToCSVFile(std::ofstream &of, const QVector< QString > &v);
};

#endif // DATATABLEVIEW_H
