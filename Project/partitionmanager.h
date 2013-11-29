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

#ifndef PARTITIONMANAGER_H
#define PARTITIONMANAGER_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class PartitionManager;
class AddPartition;
}

class PartitionManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit PartitionManager(QWidget *parent = 0);
    ~PartitionManager();
    enum PartitionState
    {
        ActivePartition,
        InActivePartition
    };

    static void setPartitionState(int partitionID, PartitionState state);
    static void getPartitionLocation(QMap<int, QString> &partitionMap);
    static bool partitionCheck();
    
private slots:
    void on_closeButton_clicked();

    void on_buttonInactive_clicked();

    void on_buttonActive_clicked();

    void on_buttonAddPartition_clicked();

    void on_buttonChoosePartition_clicked();

    void on_buttonSavePartition_clicked();

    void on_buttonCancelPartition_clicked();

private:
    Ui::PartitionManager *ui;
    Ui::AddPartition     *uiAdd;
    QDialog              *addDialog_;
    QSqlQueryModel *activePartitions_;
    QSqlQueryModel *inactivePartitions_;

    void refreshModels();
    void setupNewPartition(const QString& basePath);
};

#endif // PARTITIONMANAGER_H
