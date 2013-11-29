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

#ifndef GROUPING_H
#define GROUPING_H

#include <QMap>
#include <QObject>
#include <QVariantMap>
#include <QVector>

typedef QVariantMap EXIFMap;
typedef unsigned    Index;
typedef QMap< Index, EXIFMap > GroupingData;

// Strategy
class Grouping : public QObject
{
    Q_OBJECT
public:
    explicit Grouping(QObject *parent = 0);

    // Will modify groupingData QVariantMap with key group, and optionally group_name & group_color
    // pure virtual must be overriden in derived grouping method
    virtual void group(GroupingData& groupingData, bool doColoring=true) = 0;

signals:

public slots:

};

// A default implementation
class GroupingAuto : public Grouping
{
public:
    explicit GroupingAuto();

    void    group(GroupingData &groupingData, bool doColoring);

private:
    QVector< QString > colors_;
    QString color(int group);
};

#endif // GROUPING_H
