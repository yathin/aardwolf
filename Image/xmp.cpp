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

#include "xmp.h"

XMP::XMP()
{

}

XMP::~XMP()
{ }

void XMP::makeXMPName(const QString &tagGroup, const QString &tagName, QString &xmpName)
{
    xmpName = tagGroup + "_" + tagName;
}
