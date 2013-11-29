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

#include "readonlyfilterproxymodel.h"

#include <QSqlQueryModel>
#include <QSqlRecord>

ReadOnlyFilterProxyModel::ReadOnlyFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{ }

void ReadOnlyFilterProxyModel::setColumns(const QList<int> &columns)
{
    this->readOnlyColumns_.clear();
    this->readOnlyColumns_.append(columns);
}

bool ReadOnlyFilterProxyModel::filterAcceptsRow (int source_row, const QModelIndex & source_parent) const {
    Q_UNUSED(source_row);
    Q_UNUSED(source_parent);
    return true;
}


QVariant ReadOnlyFilterProxyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    return QSortFilterProxyModel::data(index,role);
}

bool ReadOnlyFilterProxyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(!index.isValid())
        return false;

    return QSortFilterProxyModel::setData(index,value,role);
}

Qt::ItemFlags ReadOnlyFilterProxyModel::flags ( const QModelIndex & index ) const {
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    if (this->readOnlyColumns_.contains(index.column()))
        return Qt::ItemIsSelectable;

    return QSortFilterProxyModel::flags(index);
}
