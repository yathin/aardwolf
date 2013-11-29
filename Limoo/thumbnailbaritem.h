#ifndef THUMBNAILBARITEM_H
#define THUMBNAILBARITEM_H

#include <QObject>
#include <QPixmap>
#include <QVariantMap>

class ThumbnailBarItemPrivate;
class ThumbnailBarItem : public QObject
{
    Q_OBJECT

    friend class Limoo;

public:
    ~ThumbnailBarItem();

    void setText(const QString & text);
    QString text() const;

    bool setPixmap(const QString & path);
    QString pixmap() const;

    void setDirectoryPath(const QString & path);
    QString directoryPath() const;

    void setFileName(const QString & fileName);
    QString fileName() const;

    QString filePath() const;

    void    setEXIF(const QString & exif);
    QString exif() const;

    const QVariantMap& exifMap();

    int     group();
    void    setGroup(int group);
    void    setGroupColor(const QString& color);

    void    unselect();
    void    select();

    QString thumbnailAddress() const;
    
public slots:
    void show();

    void loadLight();
    void loadHard();

    void unloadLight();
    void unloadHard();

    void reload();

private slots:
    void objDestroyed();
    void loadThumbnail();

    void thumb_loader_completed(QThread *thread);
    void light_loader_completed(QThread *thread);
    void hard_loader_completed(QThread *thread);

signals:
    void add_request(const QString & path);

private:
    void refresh_light();
    void refresh_hard();

    QVariantMap exifMap_;
    ThumbnailBarItem(QObject *item);
    QObject *obj() const;
    
private:
    ThumbnailBarItemPrivate *p;
};

#endif // THUMBNAILBARITEM_H
