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

#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QString>

#include "Folder/folder.h"

class Search : public QObject
{
    Q_OBJECT
public:
    static Search& instance();
    void   search(const QString& searchTerm, FileList& fileList, int cameraID = -1, int folderID = -1);
private:
    explicit Search(QObject *parent = 0);

signals:

public slots:

};

#endif // SEARCH_H
