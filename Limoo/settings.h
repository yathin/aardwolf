#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(QObject *parent = 0);
    
public slots:
    void setValue(const QString & key, const QVariant & value);
    QVariant value(const QString & key , const QVariant & defaultValue = QVariant()) const;
    void remove(const QString & key);
    
};

#endif // SETTINGS_H
