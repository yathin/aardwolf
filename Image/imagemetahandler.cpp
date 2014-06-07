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

#include "imagemetahandler.h"

#include <QLabel>
#include <QScrollArea>
#include <QCheckBox>
#include <QScrollArea>
#include <QTextEdit>
#include <QSqlQuery>
#include <QLineEdit>
#include <QVariantList>

#include <exiv2/exiv2.hpp>

#include "Helpers/mediator.h"
#include "Helpers/filemanager.h"

#include "xmp.h"

ImageMeta::ImageMeta(const QVector<int> &selectedIndex, const QVector<QString> &selectedPaths)
    : QObject(),
      selectedIndex_(selectedIndex),
      selectedPaths_(selectedPaths)
{ }

void ImageMeta::show()
{
    const TagDefinition& tagDef = CurrentProject::instance().project().getTagDefinition();

    QList< TagGroup > tagGroups;
    tagDef.getTagGroups(tagGroups);

    FileMeta fm;

    QVector< int > photoIDs;
    foreach (int i, this->selectedIndex_)
    {
        photoIDs.push_back(Mediator::instance().fileID(i));
    }

    CurrentProject::instance().project().addToXMPMap(xmpMap_);
    Cameras::instance().addToXMPMap(Mediator::instance().getCurrentCamera(), xmpMap_);

    Mediator::instance().getFileMeta(photoIDs, fm);

    foreach (TagGroup m, tagGroups)
    {
        if (!Mediator::instance().isMetaEnabled(m))
            continue;

        QVector< PhotoTagDefinition > photoTag;
        tagDef.getPhotoTagDefinition(m, photoTag);

        tabWidget_[m]   = new QScrollArea;
        layout_[m]      = new QGridLayout;

        tabWidget_[m]->setMinimumHeight(30*photoTag.count());

        foreach (PhotoTagDefinition pm, photoTag)
        {
            xmpMap_[pm.xmpName] = "";
            if (pm.valueType == "Checkbox")
            {
                widget_[m][pm.name] = new QCheckBox(pm.name);
                widget_[m][pm.name]->setObjectName(m + "___" + pm.name);

                if (!fm.isEmpty())
                {
                    if (fm.find(m) != fm.end())
                    {
                        if ((fm[m]).find(pm.name) != (fm[m]).end())
                        {
                            dynamic_cast< QCheckBox * >(widget_[m][pm.name])->setChecked(true);
                        }
                    }
                }
                connect(dynamic_cast< QCheckBox * >(widget_[m][pm.name]), SIGNAL(clicked()), this, SLOT(checboxClicked()));
                dynamic_cast< QCheckBox * >(widget_[m][pm.name])->setShortcut(pm.shortcut);
                layout_[m]->addWidget(dynamic_cast< QCheckBox *>(widget_[m][pm.name]));
            }
            else if (pm.valueType == "Text")
            {
                widget_[m][pm.name] = new QLineEdit();
                widget_[m][pm.name]->setObjectName(m + "___" + pm.name);
                widget_[m][pm.name]->setToolTip(pm.name);
                dynamic_cast< QLineEdit * >(widget_[m][pm.name])->setPlaceholderText(pm.name);
                if (!fm.isEmpty())
                {
                    if (fm.find(m) != fm.end())
                    {
                        if ((fm[m]).find(pm.name) != (fm[m]).end())
                        {
                            dynamic_cast< QLineEdit * >(widget_[m][pm.name])->setText(fm[m][pm.name]);
                        }
                    }
                }
                connect(dynamic_cast< QLineEdit * >(widget_[m][pm.name]), SIGNAL(editingFinished()), this, SLOT(textboxChanged()));
                // dynamic_cast< QLineEdit * >(widget_[m][pm.name])->setShortcut(pm.shortcut);
                layout_[m]->addWidget(dynamic_cast< QLineEdit *>(widget_[m][pm.name]));
            }
        }

        tabWidget_[m]->setLayout(layout_[m]);
        Mediator::instance().addMetaTable(tabWidget_[m], m);
    }
}

ImageMeta::~ImageMeta()
{
    Mediator::instance().clearMetaTables();

    const TagDefinition& tagDef = CurrentProject::instance().project().getTagDefinition();


    QList< TagGroup > tagGroups;
    tagDef.getTagGroups(tagGroups);

    foreach (TagGroup m, tagGroups)
    {
        QVector< PhotoTagDefinition > photoTag;
        tagDef.getPhotoTagDefinition(m, photoTag);

        foreach (PhotoTagDefinition pm, photoTag)
        {
            delete widget_[m][pm.name];
        }

        delete layout_[m];
        delete tabWidget_[m];
    }

}

void ImageMeta::checboxClicked()
{
    QCheckBox *checkbox = qobject_cast< QCheckBox * >(sender());
    if (checkbox)
    {
        QStringList meta = checkbox->objectName().split("___");
        QString groupName = meta.at(0);
        QString tagName = meta.at(1);

        QString xmpName;
        XMP::makeXMPName(groupName, tagName, xmpName);
        if (checkbox->isChecked())
        {
            xmpMap_[xmpName] = "Checked";
            qxtLog->debug() << "Updating XMP Map. " << xmpName << " => Checked";
            this->insertValue(groupName, tagName, "1");
        }
        else
        {
            xmpMap_[xmpName] = "";
            qxtLog->debug() << "Clearing XMP Map key: " << xmpName;
            this->clear(groupName, tagName);
        }
    }
    Mediator::instance().setFocusOnLimoo();
}

void ImageMeta::textboxChanged()
{
    QLineEdit *lineEdit = qobject_cast< QLineEdit * >(sender());
    if (lineEdit)
    {
        QStringList meta = lineEdit->objectName().split("___");
        QString groupName = meta.at(0);
        QString tagName = meta.at(1);
        QString xmpName;
        XMP::makeXMPName(groupName, tagName, xmpName);
        if (!lineEdit->text().isEmpty())
        {
            xmpMap_[xmpName] = lineEdit->text();
            qxtLog->debug() << "Updating XMP Map. " << xmpName << " => " << lineEdit->text();
            this->insertValue(groupName, tagName, lineEdit->text());
        }
        else
        {
            xmpMap_[xmpName] = "";
            qxtLog->debug() << "Clearing XMP Map key: " << xmpName;
            this->clear(groupName, tagName);
        }
    }
    Mediator::instance().setFocusOnLimoo();
}

void ImageMeta::insertValue(const QString &groupName, const QString &tagName, const QString &value)
{
    this->clear(groupName, tagName);
    qxtLog->debug() << "Inserting: (" << groupName << ", " << tagName << ", " << value << " ) ";

    QSqlQuery tagQuery;
    tagQuery.prepare("INSERT INTO tag(photo_id, group_name, name, value) VALUES (?, ?, ?, ?)");

    QVariantList ids, groupNames, tagNames, values;
    foreach (int idx, this->selectedIndex_)
    {
        ids << Mediator::instance().fileID(idx);
        groupNames << groupName;
        tagNames << tagName;
        values << value;
    }
    tagQuery.addBindValue(ids);
    tagQuery.addBindValue(groupNames);
    tagQuery.addBindValue(tagNames);
    tagQuery.addBindValue(values);

    tagQuery.execBatch();
    qxtLog->debug() << "Query: " << tagQuery.lastQuery();
    if (CurrentProject::instance().project().xmp)
    {
        qxtLog->debug() << "Attempting to write XMP into files...";
        this->writeXMP( );
    }
}

void ImageMeta::clear(const QString &groupName, const QString &tagName)
{
    qxtLog->debug() << "Deleting: (" << groupName << ", " << tagName << " ) ";
    QSqlQuery tagQuery;
    tagQuery.prepare("DELETE FROM tag WHERE photo_id = ? AND group_name = ? AND name = ?");
    QVariantList ids, groupNames, tagNames;
    foreach (int idx, this->selectedIndex_)
    {
        ids << Mediator::instance().fileID(idx);
        groupNames << groupName;
        tagNames << tagName;
    }
    tagQuery.addBindValue(ids);
    tagQuery.addBindValue(groupNames);
    tagQuery.addBindValue(tagNames);

    tagQuery.execBatch();
    qxtLog->debug() << "Query: " << tagQuery.lastQuery();
    if (CurrentProject::instance().project().xmp)
    {
        qxtLog->debug() << "Attempting to write XMP into files...";
        this->writeXMP();
    }
}

void ImageMeta::writeXMP()
{
    qxtLog->debug() << "Writing XMP... ";

    foreach (QString file, this->selectedPaths_)
    {
        qxtLog->debug() << "Writing to file: " << file;
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(file.toStdString().c_str());
        // assert (image.get() != 0);
        image->readMetadata();
        Exiv2::XmpData &xmpData = image->xmpData();

        Exiv2::XmpProperties::registerNs("aardwolf://aardwolf/1.0/actns/", "actns");
        foreach (QString name, xmpMap_.keys())
        {
            if (name.isEmpty())
                continue;
            QString xmpName = "Xmp.actns." + name;
            xmpData[xmpName.toStdString()] = xmpMap_[name].toStdString();
        }
        image->setXmpData(xmpData);
        image->writeMetadata();
    }
}

ImageMetaHandler::ImageMetaHandler(QObject *parent) :
    QObject(parent)
{ }


void ImageMetaHandler::imageLoaded(int index, const QString &filePath, const QString &exif)
{
    if (index < 0) return;
    currentSelected_.clear();
    currentSelected_.push_back(index);
    currentSelectedPaths_.push_back(filePath);
    currentExif_  = exif;
    refresh();
}

void ImageMetaHandler::imageLoaded(const QVector<int> &selectedIndex, const QVector<QString> &selectedPaths, const QString &exif)
{
    currentSelected_ = selectedIndex;
    currentSelectedPaths_ = selectedPaths;
    currentExif_ = exif;
    refresh();
}

void ImageMetaHandler::refresh()
{
    imgMeta_.clear();
    QSharedPointer< ImageMeta > imgMeta(new ImageMeta(currentSelected_, currentSelectedPaths_));
    imgMeta_ = imgMeta;

    imgMeta_->show();
    Mediator::instance().setEXIF(currentExif_);
}

void ImageMetaHandler::imageClosed()
{
    imgMeta_.clear();
    Mediator::instance().setEXIF("");
}
