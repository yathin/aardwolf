#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QImage>
#include <QThread>

class ImageLoaderPrivate;
class ImageLoader : public QThread
{
    Q_OBJECT
public:
    ImageLoader(QObject *parent = 0);
    ~ImageLoader();

    enum Type{
        Thumbnail,
        Light,
        Hard
    };

    void start(const QString & image , Type type);

    const QString & image() const;
    const QString & thumbnail() const;

    const QImage & lightImage() const;
    const QImage & hardImage() const;

protected:
    void run();

    void loadThumbnail();
    void loadLight();
    void loadHard();

signals:
    void completed(QThread *thread);
    
private:
    ImageLoaderPrivate *p;
};

#endif // IMAGELOADER_H
