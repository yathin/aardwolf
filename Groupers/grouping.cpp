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

#include "grouping.h"

#include <QDateTime>
#include <qmath.h>

Grouping::Grouping(QObject *parent) :
    QObject(parent)
{
}

GroupingAuto::GroupingAuto()
: Grouping()
{
    colors_.push_back("aquamarine");
    colors_.push_back("bisque");
    colors_.push_back("tan");
    colors_.push_back("darkolivegreen");
    colors_.push_back("lightcoral");
    colors_.push_back("plum");
    colors_.push_back("orchid");
    colors_.push_back("lightsteelblue");
    colors_.push_back("lightsalmon");
}

void GroupingAuto::group(GroupingData &groupingData, bool doColoring)
{
    if (groupingData.count() <= 1)
        return;

    QString format("yyyy:MM:dd hh:mm:ss");
    int group = -1;
    QDateTime prevDateTime;
    QDateTime currentDateTime;
    for (GroupingData::iterator i = groupingData.begin(); i != groupingData.end(); ++i)
    {
        if (group == -1)
        {
            group = 1;
            prevDateTime = QDateTime::fromString(i->find("DateTimeOriginal").value().toString(), format);
            i->insert("group", group);
            if (doColoring )
                i->insert("group_color", color(group));
            continue;
        }


        currentDateTime = QDateTime::fromString(i->find("DateTimeOriginal").value().toString(), format);

        // 10-second rule
        if (prevDateTime.secsTo(currentDateTime) > 10)
        {
            ++group;
        }

        i->insert("group", group);
        if (doColoring )
            i->insert("group_color", color(group));

        prevDateTime = currentDateTime;
    }
}

QString GroupingAuto::color(int group)
{
    return colors_[(colors_.count()-1) % group];
}
