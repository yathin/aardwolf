/**
    This file is part of Aardwolf.

    Aardwolf is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Aardwolf is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Aardwolf.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "loggerengine.h"
#include "mediator.h"

#include <QDate>
#include <QTime>
#include <QString>

LoggerEngine::LoggerEngine()
{
}

void LoggerEngine::initLoggerEngine()
{
    // Nothing to do
}

bool LoggerEngine::isInitialized() const
{
    return true;
}

void LoggerEngine::killLoggerEngine()
{
    // Nothing to do
}

void LoggerEngine::writeFormatted(QxtLogger::LogLevel level, const QList<QVariant> &messages)
{
    QTime t = QTime::currentTime();
    QDate d = QDate::currentDate();
    QString log = "[ " + d.toString("yyyy-MM-dd")+ " " + t.toString("hh:mm:ss") + " ]";
    if (level == QxtLogger::ErrorLevel) log += " [Error] ";
    else log += " [Debug] ";
    foreach (QVariant message, messages)
    {
        log += " " + message.toString();
    }
    Mediator::instance().appendLog(log);
}
