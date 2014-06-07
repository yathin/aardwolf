#-------------------------------------------------------------------------
#    This file is part of Aardwolf.
#
#    Aardwolf is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Aardwolf is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Aardwolf.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------

#----------------------------------------------------------------------
#
# Aardwolf Camera Trap Data Management.
#   Originally developed for monitoring herbivore grazing patterns in the 
#   Etosha National Park, Namibia. 
#       Yathin S Krishnappa / Wendy Turner
#           Centre for Ecological and Evolutionary Synthesis (CEES)
#               Universitetet i Oslo, Norway
#
#----------------------------------------------------------------------

QT       += core gui declarative sql xml

TARGET = Aardwolf
TEMPLATE = app

#QMAKE_CXX = clang++
#QMAKE_CC = clang
#QMAKE_RUN_CXX = $(CXX) $(CXXFLAGS) $(INCPATH) -c $src -o $obj
#QMAKE_RUN_CC = $(CC) $(CCFLAGS) $(INCPATH) -c $src -o $obj

QMAKE_CFLAGS += -mmacosx-version-min=10.9
QMAKE_CXXFLAGS += -mmacosx-version-min=10.9

win32 {
    CONFIG      += static staticlib qxt
    INCLUDEPATH += $$quote(C:/Qxt/include) $$quote(C:/Qxt/include/QxtCore)
    INCLUDEPATH += $$quote(C:/msys/1.0/local/include)
    LIBS        += $$quote(C:/msys/1.0/local/lib/libexpat.dll.a)
    LIBS        += $$quote(C:/msys/1.0/local/lib/libz.dll.a)
    LIBS        += $$quote(C:/msys/1.0/local/lib/libexiv2.dll.a)
    RC_FILE      = aardwolf.rc
    QXT         += core
}


macx{
    CONFIG       += static staticlib qxt
    INCLUDEPATH  += /usr/local/include /usr/local/Qxt/include/QxtCore/
    LIBS         += -L/usr/local/lib  /usr/local/lib/libexiv2.a -lsqlite3 -lexpat -lz -liconv
    ICON          = pics/icon.icns
    QXT          += core
}

SOURCES += main.cpp \
    mainwindow.cpp \
    Limoo/thumbnailbaritem.cpp \
    Limoo/limoocolors.cpp \
    Limoo/imageprovider.cpp \
    Limoo/limoo.cpp \
    Limoo/settings.cpp \
    Limoo/imageloader.cpp \
    Limoo/sstringlist.cpp \
    Limoo/sstring.cpp \
    Camera/addcamera.cpp \
    Camera/camera.cpp \
    Camera/download.cpp \
    Project/createproject.cpp \
    Project/project.cpp \
    Tag/edittagdefintion.cpp \
    Tag/tagdefinition.cpp \
    Data/datatableview.cpp \
    Helpers/historybar.cpp \
    Helpers/filemanager.cpp \
    Helpers/mediator.cpp \
    Helpers/filter.cpp \
    Helpers/search.cpp \
    Helpers/loggerengine.cpp \
    Etc/aboutdialog.cpp \
    Folder/folder.cpp \
    Image/imagemetahandler.cpp \
    Groupers/grouping.cpp \ 
    Image/xmp.cpp \
    Tag/readonlyfilterproxymodel.cpp \
    Reports/report.cpp \
    Project/partitionmanager.cpp

HEADERS  += \
    mainwindow.h \
    Limoo/thumbnailbaritem.h \
    Limoo/limoocolors.h \
    Limoo/imageprovider.h \
    Limoo/limoo.h \
    Limoo/settings.h \
    Limoo/limoo_global.h \
    Limoo/imageloader.h \
    Limoo/sstringlist.h \
    Limoo/sstring.h \
    Camera/addcamera.h \
    Camera/camera.h \
    Project/createproject.h \
    Project/project.h \
    Tag/edittagdefintion.h \
    Tag/tagdefinition.h \
    Tag/tag.h \
    Camera/download.h \
    Data/datatableview.h \
    Etc/aboutdialog.h \
    Helpers/historybar.h \
    Helpers/filemanager.h \
    Helpers/mediator.h \
    Helpers/filter.h \
    Helpers/loggerengine.h \
    Helpers/search.h \
    Image/imagemetahandler.h \
    Folder/folder.h \
    Groupers/grouping.h \ 
    Image/xmp.h \
    Tag/readonlyfilterproxymodel.h \
    Reports/report.h \
    Project/partitionmanager.h

RESOURCES += \
    aardwolf.qrc

OTHER_FILES += \
    aardwolf.rc \
    qml/ThumbnailBarItem.qml \
    qml/ThumbnailBar.qml \
    qml/ScrollBar.qml \
    qml/main.qml \
    qml/ZoomImage.qml \
    SQL/create_project.sql \
    SQL/create_user.sql \
    SQL/create_camera.sql \
    SQL/create_folder.sql \
    SQL/create_photo.sql \
    SQL/create_data_basic_view.sql \
    SQL/create_tag_definition.sql \
    SQL/create_tag.sql \
    SQL/create_xmp.sql \
    SQL/create_reports.sql \
    SQL/create_reports_default.sql \
    SQL/create_partition.sql \
    SQL/create_folder_tagged_view.sql

FORMS += \
    mainwindow.ui \
    Camera/addcamera.ui \
    Project/createproject.ui \
    Tag/edittagdefintion.ui \
    Camera/download.ui \
    Data/datatableview.ui \
    Etc/aboutdialog.ui \
    Tag/createtag.ui \
    Reports/report.ui \
    Reports/createreport.ui \
    Project/partitionmanager.ui \
    Project/addpartition.ui

