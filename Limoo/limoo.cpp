#define LOAD_STEP_SIZE 100

#include "limoo.h"
#include "imageprovider.h"
#include "thumbnailbaritem.h"
#include "limoocolors.h"
#include "settings.h"
#include "limoo_global.h"
#include "sstring.h"
#include "sstringlist.h"

#include <QList>
#include <QVariant>
#include <QMetaObject>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include <QCloseEvent>
#include <QWheelEvent>
#include <QUrl>
#include <QSet>
#include <QSetIterator>
#include <QFileInfo>
#include <QDir>
#include <QKeySequence>
#include <QApplication>

#include "Helpers/historybar.h"
#include "Helpers/mediator.h"

class ThumbnailBarPrivate
{
public:
    QList<ThumbnailBarItem*> list;
    QString id;

    QDeclarativeEngine *engine;
    QDeclarativeContext *root;

    QObject *thumbnailbar;

    Settings *settings;

    bool on_process;
    int prev_index;

    QSet<ThumbnailBarItem*> loaded_lights;
};

Limoo::Limoo(QWidget *parent) :
    QDeclarativeView(parent)
{
    QString thumbnails_path = THUMBNAILS_PATH;
    if(!QDir(thumbnails_path).exists())
        QDir().mkpath(thumbnails_path);
    const QStringList & thumbnails = QDir(thumbnails_path).entryList(QDir::Files,QDir::Time);
    // Clean up thumnails
    for(int i=0 ; i<thumbnails.count() ; i++)
        QFile::remove(thumbnails_path + "/" + thumbnails.at(i));

    p = new ThumbnailBarPrivate;
        p->id = ImageProvider::getId();
        p->on_process = false;
        p->prev_index = -1;

    p->settings = new Settings(this);

    // ImageProvider::addPixmap("folder" , QIcon(":/limoo/pics/folder.png").pixmap(128,128));

    ImageProvider::addPixmap("folder-camera", QIcon(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-devices-camera-photo.svg.png").pixmap(128,128));
    ImageProvider::addPixmap("folder-green", QIcon(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-places-folder-green.svg.png").pixmap(128,128));
    ImageProvider::addPixmap("folder-grey", QIcon(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-places-folder-grey.svg.png").pixmap(128,128));
    ImageProvider::addPixmap("folder-yellow", QIcon(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-places-folder-yellow.svg.png").pixmap(128,128));
    ImageProvider::addPixmap("folder-orange", QIcon(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-places-folder-orange.svg.png").pixmap(128,128));


    p->engine = engine();
        p->engine->addImageProvider(QLatin1String("pixmaps"), new ImageProvider());

    p->root = rootContext();
        p->root->setContextProperty("Colors",new LimooColors(this));
        p->root->setContextProperty("View",this);
        p->root->setContextProperty("Settings", p->settings);

    setSource(QUrl("qrc:/limoo/qml/main.qml"));

    p->thumbnailbar = rootObject()->findChild<QObject*>("thumbnailbar");


#ifdef Q_OS_LINUX
    rootObject()->setProperty("linux" , true);
    rootObject()->setProperty("os" , "linux");
#endif
#ifdef Q_OS_MAC
    rootObject()->setProperty("mac" , true);
    rootObject()->setProperty("os" , "mac");
#endif
#ifdef Q_OS_WIN32
    rootObject()->setProperty("windows" , true);
    rootObject()->setProperty("os" , "windows");
#endif

    setResizeMode(QDeclarativeView::SizeRootObjectToView);

    p->settings->value("general/size",QSize(parent->width(),parent->height()));
    resize(QSize(parent->width(), parent->height()));
    setAcceptDrops(true);
    show();

    connect(p->thumbnailbar , SIGNAL(openDir(QString)) , SLOT(openDir(QString)) , Qt::QueuedConnection);
    connect(p->thumbnailbar , SIGNAL(preview(int))     , SLOT(currentIndexChanged(int)));
    connect(p->thumbnailbar , SIGNAL(loadMore(int))    , SLOT(loadMore(int)));
    connect(p->thumbnailbar , SIGNAL(select(int, int)) , SLOT(select(int, int)));
}

void Limoo::select(int index, int modifier)
{
    group_unselect();

    switch (modifier)
    {
    case 0:
        this->selected.clear();
        this->selectedPaths.clear();
        this->selected.push_back(index);
        this->selectedPaths.push_back(at(index)->filePath());
        break;
    case 1:             // Alt + Mouse Click
        this->selected.clear();
        this->selectedPaths.clear();
        for (int i = 0; i < this->count(); ++i)
        {
            if (at(i)->group() == at(index)->group())
            {
                this->selected.push_back(i);
                this->selectedPaths.push_back(at(i)->filePath());
            }
        }
        break;
    case 2:             // Ctrl + Mouse Click
        // TODO check index if it is already selected
        this->selected.push_back(index);
        this->selectedPaths.push_back(at(index)->filePath());
        break;
    case 3:             // Shift + Mouse Click
        if (this->selected.count() == 0)
        {
            this->selected.push_back(index);
            this->selectedPaths.push_back(at(index)->filePath());
        }
        else if (index <= this->selected[0])
        {
            int end = this->selected[0];
            this->selected.clear();
            for (int i = index; i <= end; ++i)
            {
                this->selected.push_back(i);
                this->selectedPaths.push_back(at(i)->filePath());
            }
        }
        else
        {
            int begin = this->selected[0];
            this->selected.clear();
            for (int i = begin; i <= index; ++i)
            {
                this->selected.push_back(i);
                this->selectedPaths.push_back(at(i)->filePath());
            }
        }
        break;
    }


    group_select();
}

void Limoo::group_unselect()
{
    foreach (unsigned idx, this->selected)
    {
        at(idx)->unselect();
    }
    emit this->imageClosed();
}

void Limoo::group_select()
{
    foreach (unsigned idx, this->selected)
    {
        at(idx)->select();
    }
    emit this->imageLoaded(this->selected, this->selectedPaths, at(this->selected[0])->exif());
}

void Limoo::autoAdd(const QStringList & list)
{
    for(int i=0 ; i<list.count() ; i++)
        autoAdd(list.at(i));
}


void Limoo::autoAdd(const QString & path , const QString & show)
{
    QString abs_path = QDir(path).absolutePath();
    QString abs_show = (show.isEmpty())? QString() : QDir(show).absolutePath();

    QFileInfo file(abs_path);
    int start = count();

    if(!show.isEmpty())
    {
        QFileInfo shf(abs_show);
        insert(start , abs_show , shf.fileName());

        QMetaObject::invokeMethod(p->thumbnailbar , "show" , Q_ARG(QVariant,start));
    }
    add(abs_path , file.fileName());

    return;


    if(!file.isDir())
    {
        add(abs_path , file.fileName());
    }
    else
    {
        SStringList folders = QDir(abs_path).entryList(QDir::NoDotAndDotDot|QDir::Dirs,QDir::Name);
        SStringList files = QDir(abs_path).entryList(QDir::NoDotAndDotDot|QDir::Files,QDir::Name);
        SStringList list;

        qSort(files.begin() , files.end());
        qSort(folders.begin() , folders.end());

        // list << ".." << folders << files;
        list << folders << files;

        p->on_process = true;
        for(int i=0 ; i<list.count() ; i++)
        {
            if(i%20 == 0)
                QApplication::processEvents();

            QString ip = abs_path + "/" + list.at(i);
            if(ip == abs_show)
            {
                QMetaObject::invokeMethod(p->thumbnailbar , "show" , Q_ARG(QVariant,i));
                continue;
            }

            insert(start+i , ip , list.at(i));
        }

        p->on_process = false;
    }
}

ThumbnailBarItem *Limoo::add(const QString & path , const QString & name)
{
    return insert(count() , path , name);
}

ThumbnailBarItem *Limoo::insert(int index , const QString & path , const QString & name)
{
    QFileInfo file(path);
    ThumbnailBarItem *item = insert(index , name);

    if(!file.isDir())
    {
        if(!item->setPixmap(path))
        {
            delete item;
            item = 0;
        }
    }
    else
    {
        item->setDirectoryPath(path);
    }

    return item;
}

ThumbnailBarItem *Limoo::insert(int index , const QString &)
{
    QVariant returnedValue;
    QMetaObject::invokeMethod(p->thumbnailbar , "add" , Q_RETURN_ARG(QVariant, returnedValue));

    QObject     *obj  = qvariant_cast<QObject*>(returnedValue);
    ThumbnailBarItem *item = new ThumbnailBarItem(obj);
        obj->setProperty("index" , index);

    p->list.insert(index , item);
    p->thumbnailbar->setProperty("count",count());

    connect(item , SIGNAL(destroyed(QObject*))  , SLOT(itemDestroyed(QObject*))  );
    connect(item , SIGNAL(add_request(QString)) , SLOT(item_add_request(QString)));

    reindex();
    return item;
}

void Limoo::remove(ThumbnailBarItem *item)
{
    if(p->loaded_lights.contains(item))
        p->loaded_lights.remove(item);

    delete item;
}

void Limoo::remove(int index)
{
    remove(p->list.at(index));
}

void Limoo::clear()
{
    p->on_process = true;
    while(!isEmpty())
    {
        QApplication::processEvents();
        remove(count()-1);
    }
    p->thumbnailbar->setProperty("highlighted_row" , -1);
    p->thumbnailbar->setProperty("highlighted_column" , -1);
    p->on_process = false;
}

void Limoo::loadMore(int index)
{
    if(index < LOAD_STEP_SIZE)
        return;
}

ThumbnailBarItem *Limoo::at(int index) const
{
    return p->list.at(index);
}

int Limoo::indexOf(ThumbnailBarItem *item) const
{
    return p->list.indexOf(item);
}

QObject *Limoo::objectAt(int index)
{
    return at(index)->obj();
}

int Limoo::count() const
{
    return p->list.count();
}

bool Limoo::isEmpty() const
{
    return count() == 0;
}

int Limoo::currentIndex()
{
    return p->thumbnailbar->property("on_preview").toInt();
}

bool Limoo::onProcess() const
{
    return p->on_process;
}

Settings *Limoo::settings() const
{
    return p->settings;
}

void Limoo::openDir(const QString & path)
{
    if(p->on_process)
        return;

    Mediator::instance().openDir(QFileInfo(path+"/").dir().dirName());
}

void Limoo::itemDestroyed(QObject *obj)
{
    p->list.removeOne(static_cast<ThumbnailBarItem*>(obj));
    p->thumbnailbar->setProperty("count",count());
    reindex();
}

void Limoo::reindex()
{
    for(int i=0 ; i<p->list.count() ; i++)
    {
        QObject *obj = p->list.at(i)->obj();
        if(obj->property("selected").toBool())
            p->thumbnailbar->setProperty("on_preview",i);

        obj->setProperty("index",i);
    }
}

void Limoo::setPreviewStyle(PreviewStyle previewStyle)
{
    if (previewStyle == FilmStrip)
    {
        p->thumbnailbar->setProperty("on_preview", 0);
    }
    else if (previewStyle == Preview)
    {
        p->thumbnailbar->setProperty("on_preview", -1);
        p->thumbnailbar->setProperty("preview", true);
    }
}

Limoo::PreviewStyle Limoo::getPreviewStyle()
{
    if (p->thumbnailbar->property("on_preview").toInt() == -1)
        return Preview;
    return FilmStrip;
}

void Limoo::currentIndexChanged(int index)
{
    QSet<ThumbnailBarItem*> loadeds;

    int min_cs = rootObject()->property("minimum_cache_size").toInt();
    int max_cs = rootObject()->property("maximum_cache_size").toInt();

    if(index != -1)
    {
        emit imageLoaded(index, at(index)->filePath(), at(index)->exif());

        int start_index = index - min_cs;
        int end_index   = index + min_cs;

        if(p->prev_index != -1 && abs(index-p->prev_index) == 1)
        {
            if(index - p->prev_index == 1)
                end_index = index + max_cs;
            if(index - p->prev_index == -1)
                start_index = index - max_cs;
        }

        for(int i=start_index ; i < end_index+1 ; i++)
        {
            if(i < 0)
                continue;
            if(i >= count())
                break;

            ThumbnailBarItem *item = at(i);
            if(!item->directoryPath().isEmpty())
                continue;
            if(!p->loaded_lights.contains(item))
                item->loadLight();

            loadeds.insert(item);

        }
    }
    else
    {
        emit imageClosed();
    }

    QSetIterator<ThumbnailBarItem *> i(p->loaded_lights);
    while (i.hasNext())
    {
        ThumbnailBarItem *item = i.next();
        if(loadeds.contains(item))
            continue;

        item->unloadLight();
    }

    if(index != -1 && index < count())
        at(index)->loadHard();
    if(p->prev_index != -1 && p->prev_index < count() && p->prev_index != index)
        at(p->prev_index)->unloadHard();

    p->loaded_lights = loadeds;
    p->prev_index = index;
}

void Limoo::item_add_request(const QString & path)
{
    int index = 0;
    for(int i=0 ; i<count() ; i++)
    {
        ThumbnailBarItem *c = at(i);
        if(c->directoryPath().isEmpty() && c->pixmap() > path)
            continue;

        index = i+1;
    }

    insert(index , path , QString());
    QMetaObject::invokeMethod(p->thumbnailbar , "show" , Q_ARG(QVariant,index));
}

void Limoo::wheelEvent(QWheelEvent *event)
{
    QDeclarativeView::wheelEvent(event);
    emit wheel(event->delta());
}

void Limoo::dragEnterEvent(QDragEnterEvent *event)
{ Q_UNUSED(event); }

void Limoo::dropEvent(QDropEvent *event)
{ Q_UNUSED(event); }

void Limoo::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void Limoo::resizeEvent(QResizeEvent *event)
{
    QDeclarativeView::resizeEvent(event);
    p->settings->setValue("general/size",event->size());
}

QObject *Limoo::obj() const
{
    return p->thumbnailbar;
}

Limoo::~Limoo()
{
    delete p;
}
