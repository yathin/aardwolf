#ifndef SSTRINGLIST_H
#define SSTRINGLIST_H

#include <QList>

#include "sstring.h"

class SStringList : public QList<SString>
{
public:
    SStringList ();
    SStringList (const QList<QString> & other);

    QStringList toQStringList() const;

};

#endif // SSTRINGLIST_H
