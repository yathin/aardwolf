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

#ifndef TAG_H
#define TAG_H

#include <QMap>
#include <QString>

struct Tag
{
    unsigned    id;
    unsigned    photo_id;
    QString     tagGroup;
    QString     name;
    QString     value;
    unsigned    updatedOn;
    unsigned    updatedBy;
};

typedef QString TagGroup;
typedef QString TagName;

typedef QMap< TagName, Tag >            PhotoTagMap;
typedef QMap< TagGroup, PhotoTagMap >   TagMap;


#endif // TAG_H

