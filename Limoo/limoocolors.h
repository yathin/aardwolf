#ifndef LIMOOCOLORS_H
#define LIMOOCOLORS_H

#include <QObject>
#include <QColor>

class LimooColorsPrivate;
class LimooColors : public QObject
{
    Q_OBJECT
public:
    LimooColors(QObject *parent = 0);
    ~LimooColors();

    Q_INVOKABLE QColor window() const;
    Q_INVOKABLE QColor windowText() const;
    Q_INVOKABLE QColor base() const;
    Q_INVOKABLE QColor alternateBase() const;
    Q_INVOKABLE QColor toolTipBase() const;
    Q_INVOKABLE QColor toolTipText() const;
    Q_INVOKABLE QColor text() const;
    Q_INVOKABLE QColor button() const;
    Q_INVOKABLE QColor buttonText() const;
    Q_INVOKABLE QColor brightText() const;
    Q_INVOKABLE QColor light() const;
    Q_INVOKABLE QColor midlight() const;
    Q_INVOKABLE QColor dark() const;
    Q_INVOKABLE QColor mid() const;
    Q_INVOKABLE QColor shadow() const;
    Q_INVOKABLE QColor highlight() const;
    Q_INVOKABLE QColor highlightedText() const;
    Q_INVOKABLE QColor link() const;
    Q_INVOKABLE QColor linkVisited() const;
    
private:
    LimooColorsPrivate *p;
};

#endif // LIMOOCOLORS_H
