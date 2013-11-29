#include "sstring.h"

#include <QStringList>

int cutFirstNumber(const QString & str)
{
    for(int i=0 ; i<str.size() ; i++)
        if(!str.at(i).isNumber())
            return str.mid(0 , i).toInt();

    return 0;
}


SString::SString () {}
SString::SString (QChar ch) : QString(ch) {}
SString::SString (int size, QChar ch) : QString(size , ch) {}
SString::SString (const QLatin1String & str) : QString(str) {}
SString::SString (const QString & other) : QString(other) {}
SString::SString (const char * str) : QString(str) {}
SString::SString (const QByteArray & ba) : QString(ba) {}
SString::~SString() {}

bool SString::operator< (const QString & other) const
{
    QStringList a;
    QStringList b;

    for(int i=0 ; i<other.count() ; i++)
    {
        if(other.at(i).isNumber() && i!=0 && other.at(i-1).isNumber())
            a.replace(a.count()-1 , a.last() + other.at(i));
        else
            a.append(other.at(i));
    }


    for(int i=0 ; i<count() ; i++)
    {
        if(at(i).isNumber() && i!=0 && at(i-1).isNumber())
            b.replace(b.count()-1 , b.last() + at(i));
        else
            b.append(at(i));
    }

    for(int i=0 ; i<a.count() && i<b.count() ; i++)
    {
        QString l = b.at(i).toLower();
        QString r = a.at(i).toLower();

        bool l_is_number = l.count() > 1 || l.at(0).isNumber() && l.at(0) != '.';
        bool r_is_number = r.count() > 1 || r.at(0).isNumber() && r.at(0) != '.';

        if(l_is_number && !r_is_number)
            return true;
        if(!l_is_number && r_is_number)
            return false;
        if(l_is_number && r_is_number)
        {
            l = l.rightJustified(r.count() , '0');
            r = r.rightJustified(l.count() , '0');

            if(l < r)
                return true;
            else if(l > r)
                return false;

            continue;
        }
        if(!l_is_number && !r_is_number)
        {
            if(l < r)
                return true;
            else if(l > r)
                return false;

            continue;
        }
    }

    return false;
}

bool SString::operator<= (const QString & other) const
{
    return !this->operator >(other);
}

bool SString::operator> (const QString & other) const
{
    return !this->operator <(other); // && !this->operator ==(other);
}

bool SString::operator>= (const QString & other) const
{
    return !this->operator <(other);
}
