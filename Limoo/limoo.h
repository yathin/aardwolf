#ifndef THUMBNAILBAR_H
#define THUMBNAILBAR_H

#include <QDeclarativeView>
#include <QPixmap>
#include <QVariant>
#include <QPoint>
#include <QVector>

class Settings;
class ThumbnailBarItem;
class ThumbnailBarPrivate;

class Limoo : public QDeclarativeView
{
    Q_OBJECT
public:
    Limoo(QWidget *parent = 0);
    ~Limoo();

    ThumbnailBarItem *add(const QString & path , const QString & name);
    ThumbnailBarItem *insert(int index , const QString & path , const QString & name);
    ThumbnailBarItem *insert(int index , const QString & name);

    void remove(ThumbnailBarItem *item);
    void remove(int index);

    ThumbnailBarItem *at(int index) const;
    int indexOf(ThumbnailBarItem *item) const;
    Q_INVOKABLE QObject *objectAt(int index);

    int count() const;
    bool isEmpty() const;

    int currentIndex();

    QObject *obj() const;

    Q_INVOKABLE bool onProcess() const;

    Settings *settings() const;

    enum         PreviewStyle{ Preview, FilmStrip };
    void         setPreviewStyle(PreviewStyle previewStyle);
    PreviewStyle getPreviewStyle();

    // int     selected;

public slots:
    void autoAdd(const QStringList & list);
    void autoAdd(const QString & path , const QString & show = QString());

    void clear();
    void loadMore(int index);

signals:
    void wheel(qreal delta);
    void imageLoaded(int currentIndex, const QString &filePath, const QString &exifStr);
    void imageLoaded(const QVector< int > &selectedIndex, const QVector< QString > &filePaths, const QString &exifStr);
    void imageClosed();

public slots:
    void openDir(const QString & path);

private slots:
    void itemDestroyed(QObject *obj);
    void reindex();
    void currentIndexChanged(int index);
    void item_add_request(const QString &path);
    void select(int index, int modifier=0);

protected:
    void wheelEvent(QWheelEvent *event);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    
private:
    ThumbnailBarPrivate *p;
    QPoint      origin;

    void group_select();
    void group_unselect();

    QVector< int > selected;
    QVector< QString  > selectedPaths;

    friend class MainWindow;
};

#endif // THUMBNAILBAR_H
