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

#include <QSqlQuery>
#include <QVariant>

#include "tagdefinition.h"

PhotoTagDefinition::PhotoTagDefinition() { }

PhotoTagDefinition::PhotoTagDefinition(const TagGroup& pTagGroup,
                                       const TagName& pName,
                                       const QString& pValueType,
                                       const QString& pShortcut,
                                       const QString& pXMPName)
    : tagGroup(pTagGroup),
      name(pName),
      valueType(pValueType),
      shortcut(pShortcut),
      xmpName(pXMPName) { }

PhotoTagDefinition::PhotoTagDefinition(const TagGroup& pTagGroup,
                                       const TagName& pName,
                                       const QString& pValueType,
                                       const QString& pShortcut,
                                       const QString& pXMPName,
                                       unsigned pUpdatedOn,
                                       unsigned pUpdatedBy)
    : tagGroup(pTagGroup),
      name(pName),
      valueType(pValueType),
      shortcut(pShortcut),
      xmpName(pXMPName),
      updatedOn(pUpdatedOn), 
      updatedBy(pUpdatedBy) { }


TagDefinition::TagDefinition(bool initFromDB)
{
    if (initFromDB)
    {
        QSqlQuery tagDefQuery;
        tagDefQuery.prepare("SELECT * FROM tag_definition");
        tagDefQuery.exec();

        while (tagDefQuery.next())
        {
            PhotoTagDefinition tagDef(
                                    tagDefQuery.value(0).toString(),
                                    tagDefQuery.value(1).toString(),
                                    tagDefQuery.value(2).toString(),
                                    tagDefQuery.value(3).toString(),
                                    tagDefQuery.value(4).toString(),
                                    tagDefQuery.value(5).toInt(),
                                    tagDefQuery.value(6).toInt()
            );
            this->addPhotoTagDefinition(tagDef);
        }

        tagDefQuery.finish();
    }
}

void TagDefinition::addPhotoTagDefinition(const PhotoTagDefinition &photoTagDef)
{
    tagDef_[photoTagDef.tagGroup][photoTagDef.name] = photoTagDef;
}


void TagDefinition::removePhotoTagDefinition(const TagGroup& tagGroup, const TagName& tagName)
{
    tagDef_[tagGroup].remove(tagName);
}

void TagDefinition::getPhotoTagDefinition(const TagGroup& tagGroup, QVector< PhotoTagDefinition >& tagDefs) const
{
    if (tagDef_.find(tagGroup) == tagDef_.end())
        return;

    for (PhotoTagDefinitionMap::const_iterator i = tagDef_[tagGroup].begin(); 
            i != tagDef_[tagGroup].end(); ++i)
        tagDefs.push_back(*i);
}

void TagDefinition::getPhotoTagDefinition(QVector<PhotoTagDefinition> &tagDefs) const
{
    for (TagDefinitionMap::const_iterator j = tagDef_.begin(); j != tagDef_.end(); ++j)
        for (PhotoTagDefinitionMap::const_iterator i = j->begin(); i != j->end(); ++j)
            tagDefs.push_back(*i);
}

void TagDefinition::removeTagGroup(const TagGroup& tagGroup)
{
    tagDef_.remove(tagGroup);
}

void TagDefinition::getTagGroups(QList< TagGroup >& tagGroups) const
{
    tagGroups = tagDef_.keys();
}
