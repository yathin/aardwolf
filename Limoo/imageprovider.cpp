#include "imageprovider.h"

#include <QHash>
#include <QPainter>

QHash<QString,QPixmap> provider_pixmap_hash;

ImageProvider::ImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
}

QString ImageProvider::getId()
{
    static int id_pointer = -1;
    id_pointer++;
    return QString::number(id_pointer);
}

void ImageProvider::addPixmap(const QString & id , const QPixmap & pixmap)
{
    provider_pixmap_hash.insert(id , pixmap);
    if(pixmap.isNull())
        provider_pixmap_hash.remove(id);
}

QPixmap ImageProvider::pixmap(const QString & id)
{
    return provider_pixmap_hash.value(id);
}

QPixmap ImageProvider::requestPixmap(const QString & id, QSize *size, const QSize & requestedSize)
{
    int width  = 22;
    int height = 22;

    if (size)
        *size = QSize(width, height);

    QPixmap pixmap = provider_pixmap_hash.value(id).scaled(requestedSize.width() > 0 ? requestedSize.width() : width,
                                                            requestedSize.height() > 0 ? requestedSize.height() : height,
                                                            Qt::KeepAspectRatio , Qt::FastTransformation);

    return pixmap;
}

ImageProvider::~ImageProvider()
{
}
