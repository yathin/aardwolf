#include "imageloader.h"
#include "limoo_global.h"

#include <QImageReader>
#include <QImageWriter>
#include <QPixmap>
#include <QImage>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDateTime>
#include <QApplication>
#include <QDesktopWidget>

class ImageLoaderPrivate
{
public:
    QString image_path;
    QString thumbnail;

    QImage lightImage;
    QImage hardImage;

    QSize desktop_size;
    qreal desktop_size_ratio;

    ImageLoader::Type type;
};

ImageLoader::ImageLoader(QObject *parent) :
    QThread(parent)
{
    p = new ImageLoaderPrivate;
    p->desktop_size = QApplication::desktop()->screenGeometry().size();
    p->desktop_size_ratio = (qreal)p->desktop_size.width() / p->desktop_size.height();
}

void ImageLoader::start(const QString & image, Type type)
{
    p->image_path = image;
    p->type  = type;

    QFileInfo file(p->image_path);

    switch(static_cast<int>(p->type))
    {
    case Thumbnail:
        {
            QString hidden_text = file.path() + " " +
                                  QString::number(file.size()) +  " " +
                                  file.created().toString("yyyy/MM/dd hh:mm:ss:zzz") + " " +
                                  file.lastModified().toString("yyyy/MM/dd hh:mm:ss:zzz");
            QString md5 = QCryptographicHash::hash(hidden_text.toStdString().c_str(),QCryptographicHash::Md5).toHex();
            p->thumbnail = THUMBNAILS_PATH + "/" + md5 + "." + file.suffix();

            if(QFileInfo(p->thumbnail).exists())
            {
                emit completed(this);
                // emit finished();
                return;
            }
        }
        break;

    case Light:
        break;

    case Hard:
        break;
    }

    QMetaObject::invokeMethod(this , "start" , Qt::QueuedConnection);
}

const QString &ImageLoader::image() const
{
    return p->image_path;
}

const QString &ImageLoader::thumbnail() const
{
    return p->thumbnail;
}

const QImage &ImageLoader::lightImage() const
{
    return p->lightImage;
}

const QImage &ImageLoader::hardImage() const
{
    return p->hardImage;
}

void ImageLoader::run()
{
    switch(static_cast<int>(p->type))
    {
    case Thumbnail:
        loadThumbnail();
        break;
    case Light:
        loadLight();
        break;
    case Hard:
        loadHard();
        break;
    }

    emit completed(this);
}

void ImageLoader::loadThumbnail()
{
    QImageReader reader(p->image_path);
    const QSize & image_size = reader.size();
    qreal image_size_ratio = (qreal)image_size.width() / image_size.height();

    QSize read_size;
    if(image_size_ratio > 1)
    {
        read_size.setWidth(256);
        read_size.setHeight(256 / image_size_ratio);
    }
    else
    {
        read_size.setHeight(256);
        read_size.setWidth(256 * image_size_ratio);
    }

    reader.setScaledSize(read_size);

    const QImage & image = reader.read();
    QImageWriter writer(p->thumbnail);
        writer.write(image);
}

void ImageLoader::loadLight()
{
    QImageReader reader(p->image_path);
    const QSize & image_size = reader.size();
    qreal image_size_ratio = (qreal)image_size.width() / image_size.height();

    QSize read_size;
    if(image_size_ratio >= 1 && p->desktop_size_ratio >= 1)
    {
        read_size.setWidth(p->desktop_size.width());
        read_size.setHeight(p->desktop_size.width() / image_size_ratio);
    }
    else if(image_size_ratio < 1 && p->desktop_size_ratio < 1)
    {
        read_size.setHeight(p->desktop_size.height());
        read_size.setWidth(p->desktop_size.height() * image_size_ratio);
    }
    else if(image_size_ratio > 1 && p->desktop_size_ratio < 1)
    {
        read_size.setWidth(p->desktop_size.height());
        read_size.setHeight(p->desktop_size.height() / image_size_ratio);
    }
    else if(image_size_ratio < 1 && p->desktop_size_ratio > 1)
    {
        read_size.setHeight(p->desktop_size.width());
        read_size.setWidth(p->desktop_size.width() * image_size_ratio);
    }

    reader.setScaledSize(read_size);
    p->lightImage = reader.read();
}

void ImageLoader::loadHard()
{
    QImageReader reader(p->image_path);
        reader.setScaledSize(reader.size());

    p->hardImage = reader.read();
}

ImageLoader::~ImageLoader()
{
    delete p;
}
