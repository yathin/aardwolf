#include "thumbnailbaritem.h"
#include "imageprovider.h"
#include "limoo_global.h"
#include "imageloader.h"

#include <exiv2/image.hpp>

#include <QVariant>
#include <QImageReader>
#include <QImageWriter>
#include <QFileInfo>
#include <QQueue>
#include <QTimer>
#include <QPainter>
#include <QStack>
#include <QFileDialog>
#include <QApplication>
#include <QVariantMap>

#include <sstream>

#include "Helpers/mediator.h"

QQueue<ThumbnailBarItem*> queue;
QHash<ImageLoader*,ThumbnailBarItem*> loaders;

class ThumbnailBarItemPrivate
{
public:
    QObject *item;
    QString id;

    QString thumbnail;

    ImageLoader *hard_loader;
    ImageLoader *light_loader;

    QImage hard_image;
    QImage light_image;

    QStack<QImage> undo_stack;
};

ThumbnailBarItem::ThumbnailBarItem(QObject *item) :
    QObject()
{
    p = new ThumbnailBarItemPrivate;
        p->item = item;
        p->id   = ImageProvider::getId();
        p->hard_loader  = 0;
        p->light_loader = 0;

    connect(p->item , SIGNAL(destroyed())                              , this , SLOT(objDestroyed())                  );

}

void ThumbnailBarItem::setText(const QString & text)
{
    QString little = text;

    if(little.count() > 20)
        little = little.left(17) + "...";

    p->item->setProperty("text" , text);
    p->item->setProperty("text_little" , little);
}

QString ThumbnailBarItem::text() const
{
    return p->item->property("text").toString();
}

void ThumbnailBarItem::setFileName(const QString & fileName)
{
    p->item->setProperty("fileName" , fileName);
}

QString ThumbnailBarItem::fileName() const
{
    return p->item->property("fileName").toString();
}

void ThumbnailBarItem::setEXIF(const QString &exif)
{
    p->item->setProperty("exif" , exif);
}

QString ThumbnailBarItem::exif() const
{
    return p->item->property("exiv").toString();
}

bool ThumbnailBarItem::setPixmap(const QString & path)
{
    QFileInfo file(path);
    if(!file.exists())
        return false;

    QImageReader image(path);
    if(!image.canRead())
        return false;
    if(image.format() ==  "svgz" || image.format() == "svg")
        return false;
    if(image.size().width() <= 0 || image.size().height() <= 0)
        return false;

    int orientation = 1;
    QString exiv_data_str;

    Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open(path.toUtf8().constData());
        exiv->readMetadata();

    Exiv2::ExifData data = exiv->exifData();
    std::ostringstream ostr;

    for(Exiv2::ExifData::const_iterator i = data.begin();i != data.end();++i)
    {
        // TODO: fix this!
        // exiv_data_str += QString() + i->tagName().c_str() + ": " + QString::number(i->value().toLong()) + "\n";
        if(i->key() == "Exif.Image.Orientation")
            orientation = i->value().toLong();

        // const char* tn = i->typeName();
        ostr
                // << std::setw(44) << std::setfill(' ') << std::left
                  // << i->() << " "
                  // << "0x" << std::setw(4) << std::setfill('0') << std::right
                  // << std::hex
                  << "<b>" << i->tagName().c_str() << "</b>: "
                  // << std::setw(9) << std::setfill(' ') << std::left
                  // << (tn ? tn : "Unknown") << " "
                  // << std::dec << std::setw(3)
                  // << std::setfill(' ') << std::right
                  // << i->count() << "  "
                  // << std::dec
                  << i->value() << "<br/>"
                  << "\n";
        std::ostringstream key;
        std::ostringstream value;
        key << i->tagName().c_str();
        value << i->value();

        exifMap_.insert(QString::fromStdString(key.str()), QString::fromStdString(value.str()));
    }
    exiv_data_str = ostr.str().c_str();

    QSize size = image.size();
    setText(file.fileName() + " - " + QString::number(size.width()) + 'x' + QString::number(size.height()));
    setFileName(file.fileName());

    p->item->setProperty("imageWidth" , size.width());
    p->item->setProperty("imageHeight" , size.height());
    p->item->setProperty("ratio" , (qreal)size.width()/size.height());
    p->item->setProperty("source" , "file:/" + path);
    p->item->setProperty("orientation" , orientation);
    p->item->setProperty("exiv" , exiv_data_str);
    p->item->setProperty("group_color", "yellow");
    p->item->setProperty("path", path);

    queue.enqueue(this);
    loadThumbnail();

    return true;
}

QString ThumbnailBarItem::pixmap() const
{
    return p->item->property("source").toString().remove(0,6);
}

void ThumbnailBarItem::setDirectoryPath(const QString & path)
{
    QFileInfo file(path);
    setText(file.fileName());

    p->item->setProperty("dir_path" , path);
    p->item->setProperty("isDir" , !path.isEmpty());

    if (Mediator::instance().isCurrentLevelProject())
        p->item->setProperty("source" , "image://pixmaps/folder-camera");
    else
    {
        p->item->setProperty("source" , "image://pixmaps/folder-green");
    }

    p->item->setProperty("imageWidth" , 128);
    p->item->setProperty("imageHeight" , 128);
    p->item->setProperty("ratio" , 1);
}

QString ThumbnailBarItem::directoryPath() const
{
    return p->item->property("dir_path").toString();
}

QString ThumbnailBarItem::thumbnailAddress() const
{
    return p->thumbnail;
}

void ThumbnailBarItem::show()
{
    QMetaObject::invokeMethod(p->item , "show" , Qt::QueuedConnection);
}

void ThumbnailBarItem::loadLight()
{
    if(p->light_loader)
        return;

    ImageLoader *l = new ImageLoader(this);

    connect(l , SIGNAL(completed(QThread*)) , this , SLOT(light_loader_completed(QThread*)) , Qt::QueuedConnection);
    connect(l , SIGNAL(finished()) , l , SLOT(deleteLater()) , Qt::QueuedConnection);

    l->start(pixmap() , ImageLoader::Light);

    p->light_loader = l;
}

void ThumbnailBarItem::loadHard()
{
    if(p->hard_loader)
        return;

    ImageLoader *l = new ImageLoader(this);

    connect(l , SIGNAL(completed(QThread*)) , this , SLOT(hard_loader_completed(QThread*)) , Qt::QueuedConnection);
    connect(l , SIGNAL(finished()) , l , SLOT(deleteLater()) , Qt::QueuedConnection);

    l->start(pixmap() , ImageLoader::Hard);

    p->hard_loader = l;
}

void ThumbnailBarItem::unloadLight()
{
    if(p->light_loader)
    {
//        delete p->light_loader;
        p->light_loader = 0;
    }

    p->light_image = QImage();

    QString l_id = "light-" + p->id;
    ImageProvider::addPixmap(l_id , QPixmap::fromImage(p->light_image));
    p->item->setProperty("internal_source_light",QString());
    p->item->setProperty("cached",false);
}

void ThumbnailBarItem::unloadHard()
{
    if(p->hard_loader)
    {
//        delete p->hard_loader;
        p->hard_loader = 0;
    }

    p->hard_image = QImage();

    QString h_id = "hard-" + p->id;
    ImageProvider::addPixmap(h_id , QPixmap::fromImage(p->hard_image));
    p->item->setProperty("internal_source_hard",QString());
}

void ThumbnailBarItem::unselect()
{
    p->item->setProperty("selector_opacity", 0.2);
}

void ThumbnailBarItem::select()
{
    p->item->setProperty("selector_opacity", 1.0);
}


void ThumbnailBarItem::reload()
{
    setPixmap(pixmap());

    unloadHard();
    unloadLight();
    loadHard();
    loadLight();
}


void ThumbnailBarItem::objDestroyed()
{
    p->item = 0;
    deleteLater();
}

void ThumbnailBarItem::loadThumbnail()
{
    if(loaders.count() > 0)
        return;
    if(queue.isEmpty())
        return;

    ImageLoader *l = new ImageLoader();
    ThumbnailBarItem *item = queue.dequeue();

    connect(l , SIGNAL(completed(QThread*)) , item , SLOT(thumb_loader_completed(QThread*)) , Qt::QueuedConnection);
    connect(l , SIGNAL(finished()) , l , SLOT(deleteLater()) , Qt::QueuedConnection);

    loaders.insert(l , this);

    l->start(item->pixmap() , ImageLoader::Thumbnail);
}

void ThumbnailBarItem::thumb_loader_completed(QThread *thread)
{
    ImageLoader *loader = static_cast<ImageLoader*>(thread);
    p->item->setProperty("internal_source_thumb" , "file:/" + loader->thumbnail());

    loaders.remove(loader);
    loadThumbnail();
}

void ThumbnailBarItem::light_loader_completed(QThread *thread)
{
    if(!p->light_loader)
        return;

    ImageLoader *loader = static_cast<ImageLoader*>(thread);

    p->light_image  = loader->lightImage();
    p->light_loader = 0;

    QString l_id = "light-" + p->id;
    ImageProvider::addPixmap(l_id , QPixmap::fromImage(p->light_image));
    p->item->setProperty("internal_source_light","image://pixmaps/"+l_id);
    p->item->setProperty("cached",true);
}

void ThumbnailBarItem::hard_loader_completed(QThread *thread)
{
    if(!p->hard_loader)
        return;

    ImageLoader *loader = static_cast<ImageLoader*>(thread);

    p->hard_image  = loader->hardImage();
    p->hard_loader = 0;

    QString l_id = "hard-" + p->id;
    ImageProvider::addPixmap(l_id , QPixmap::fromImage(p->hard_image));
    p->item->setProperty("internal_source_hard","image://pixmaps/"+l_id);
}

void ThumbnailBarItem::refresh_light()
{
}

void ThumbnailBarItem::refresh_hard()
{
}

QObject *ThumbnailBarItem::obj() const
{
    return p->item;
}

ThumbnailBarItem::~ThumbnailBarItem()
{
    if(p->item)
    {
        disconnect(p->item , SIGNAL(destroyed()) , this , SLOT(objDestroyed()));
        delete p->item;
    }
    if(loaders.key(this))
        loaders.remove(loaders.key(this));
    if(queue.contains(this))
        queue.removeOne(this);

    ImageProvider::addPixmap(p->id , QPixmap());
    delete p;
}

const QVariantMap& ThumbnailBarItem::exifMap()
{
    return exifMap_;
}

int ThumbnailBarItem::group()
{
    return p->item->property("group").toInt();
}

void  ThumbnailBarItem::setGroup(int group)
{
    p->item->setProperty("group" , group);
}

void  ThumbnailBarItem::setGroupColor(const QString& color)
{
    p->item->setProperty("group_color", color);
}

QString ThumbnailBarItem::filePath() const
{
    return p->item->property("path").toString();
}
