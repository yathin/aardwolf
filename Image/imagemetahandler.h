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

#ifndef IMAGEMETAHANDLER_H
#define IMAGEMETAHANDLER_H

#include <QObject>
#include <QSharedPointer>
#include <QTabWidget>
#include <QGridLayout>
#include <QWidget>
#include <QMap>
#include <QVector>
#include <QTextEdit>

#include "Helpers/mediator.h"
#include "Tag/tag.h"
#include "xmp.h"

class FileManager;

class ImageMeta : public QObject
{
    Q_OBJECT

public:
    // ImageMeta();
    ImageMeta(const QVector<int> &selectedIndex, const QVector< QString > &selectedPaths);
    ~ImageMeta();

    void show();
    void writeXMP();

private:
    QVector< int > selectedIndex_;
    QVector< QString > selectedPaths_;

    QMap< TagGroup, QMap< TagName, QWidget* > > widget_;
    QMap< TagGroup, QGridLayout* > layout_;
    QMap< TagGroup, QWidget* > tabWidget_;
    XMPMap xmpMap_;

    void insertValue(const QString &groupName, const QString &tagName, const QString &value);
    void clear(const QString &groupName, const QString &tagName);

private slots:
    void checboxClicked();
    void textboxChanged();
};

class ImageMetaHandler : public QObject
{
    Q_OBJECT
public:
    explicit ImageMetaHandler(QObject *parent = 0);
    void     refresh();
    
private:
    QSharedPointer< ImageMeta > imgMeta_;

    QVector< int > currentSelected_;
    QVector< QString > currentSelectedPaths_;
    QString currentExif_;

signals:
    
public slots:
    void imageLoaded(int index, const QString& filePath, const QString &exif);
    void imageLoaded(const QVector<int> &selectedIndex, const QVector< QString > &selectedPaths, const QString &exif);
    void imageClosed();

};

#endif // IMAGEMETAHANDLER_H
