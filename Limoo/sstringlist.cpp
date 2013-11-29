#include "sstringlist.h"

#include <QListIterator>
#include <QStringList>

SStringList::SStringList()
{
}

SStringList::SStringList(const QList<QString> & other)
{
    QListIterator<QString> i(other);
    while(i.hasNext())
        append(i.next());
}

QStringList SStringList::toQStringList() const
{
    QStringList result;
    QListIterator<SString> i(*this);
    while(i.hasNext())
        result.append(i.next());

    return result;
}
