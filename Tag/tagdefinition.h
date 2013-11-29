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

#ifndef TAGDEFINITION_H
#define TAGDEFINITION_H

#include <QString>
#include <QMap>
#include <QVector>

#include "tag.h"

class Project;

struct PhotoTagDefinition
{
    QString     tagGroup;
    QString     name;
    QString     valueType;
    QString     shortcut;
    QString     xmpName;
    unsigned    updatedOn;
    unsigned    updatedBy;

    PhotoTagDefinition(const TagGroup& pTagGroup,
                       const TagName&  pName,
                       const QString&  pValueType,
                       const QString&  pShortcut,
                       const QString&  pXMPName);
    PhotoTagDefinition();

private:
    PhotoTagDefinition(const TagGroup& pTagGroup,
                       const TagName&  pName,
                       const QString&  pValueType,
                       const QString&  pShortcut,
                       const QString&  pXMPName,
                       unsigned        pUpdatedOn,
                       unsigned        pUpdatedBy);

    friend class TagDefinition;
};


class TagDefinition
{
public:
    TagDefinition(bool initFromDB = false);

    void addPhotoTagDefinition(const PhotoTagDefinition& photoTagDef);
    void removePhotoTagDefinition(const TagGroup& tagGroup, const TagName& tagName);

    void getPhotoTagDefinition(const TagGroup& tagGroup, QVector< PhotoTagDefinition >& tagDefs) const;
    void getPhotoTagDefinition(QVector< PhotoTagDefinition >& tagDefs) const;

    void removeTagGroup(const TagGroup& tagGroup);
    void getTagGroups(QList< TagGroup >& tagGroups) const;

private:
    typedef QMap< TagName, PhotoTagDefinition >     PhotoTagDefinitionMap;
    typedef QMap< TagGroup, PhotoTagDefinitionMap > TagDefinitionMap;
    TagDefinitionMap tagDef_;

    friend class Project;
};

#endif // TAGDEFINITION_H
