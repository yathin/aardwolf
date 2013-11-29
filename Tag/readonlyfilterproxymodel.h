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

#ifndef READONLYFILTERPROXYMODEL_H
#define READONLYFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class ReadOnlyFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ReadOnlyFilterProxyModel(QObject *parent = 0);

    void setColumns(const QList<int> &columns);

    QVariant        data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    bool            setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    Qt::ItemFlags   flags ( const QModelIndex & index ) const;

 protected:
    bool            filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

signals:

public slots:

private:
    QList<int> readOnlyColumns_;
};

#endif
