#ifndef SSTRING_H
#define SSTRING_H

#include <QString>
#include <QChar>
#include <QLatin1String>
#include <QByteArray>

class SString : public QString
{
public:
    SString ();
    SString (QChar ch);
    SString (int size, QChar ch);
    SString (const QLatin1String & str);
    SString (const QString & other);
    SString (const char * str);
    SString (const QByteArray & ba);
    ~SString();

    bool operator< (const QString & other) const;
    bool operator<= (const QString & other) const;

    bool operator> (const QString & other) const;
    bool operator>= (const QString & other) const;

};

#endif // SSTRING_H
