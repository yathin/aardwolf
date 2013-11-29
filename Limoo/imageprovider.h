#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class ImageProvider : public QDeclarativeImageProvider
{
public:
    ImageProvider();
    ~ImageProvider();

    static QString getId();
    static void addPixmap(const QString & id , const QPixmap & pixmap);
    static QPixmap pixmap(const QString & id);

    QPixmap requestPixmap(const QString & id, QSize * size, const QSize & requestedSize);
};

#endif // IMAGEPROVIDER_H
