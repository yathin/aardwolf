#include "limoocolors.h"

#include <QPalette>

class LimooColorsPrivate
{
public:
    QPalette palette;
};

LimooColors::LimooColors(QObject *parent) :
    QObject(parent)
{
    p = new LimooColorsPrivate;
}

QColor LimooColors::window() const
{
    return p->palette.window().color();
}

QColor LimooColors::windowText() const
{
    return p->palette.windowText().color();
}

QColor LimooColors::base() const
{
    return p->palette.base().color();
}

QColor LimooColors::alternateBase() const
{
    return p->palette.alternateBase().color();
}

QColor LimooColors::toolTipBase() const
{
    return p->palette.toolTipBase().color();
}

QColor LimooColors::toolTipText() const
{
    return p->palette.toolTipText().color();
}

QColor LimooColors::text() const
{
    return p->palette.text().color();
}

QColor LimooColors::button() const
{
    return p->palette.button().color();
}

QColor LimooColors::buttonText() const
{
    return p->palette.buttonText().color();
}

QColor LimooColors::brightText() const
{
    return p->palette.brightText().color();
}

QColor LimooColors::light() const
{
    return p->palette.light().color();
}

QColor LimooColors::midlight() const
{
    return p->palette.midlight().color();
}

QColor LimooColors::dark() const
{
    return p->palette.dark().color();
}

QColor LimooColors::mid() const
{
    return p->palette.mid().color();
}

QColor LimooColors::shadow() const
{
    return p->palette.shadow().color();
}

QColor LimooColors::highlight() const
{
    return p->palette.highlight().color();
}

QColor LimooColors::highlightedText() const
{
    return p->palette.highlightedText().color();
}

QColor LimooColors::link() const
{
    return p->palette.link().color();
}

QColor LimooColors::linkVisited() const
{
    return p->palette.linkVisited().color();
}

LimooColors::~LimooColors()
{
    delete p;
}

