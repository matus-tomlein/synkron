/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2009 Matus Tomlein (matus.tomlein@gmail.com)

 Synkron is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Synkron is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Synkron; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#ifndef MTFILE_H
#define MTFILE_H

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QProcess>
#include <QDateTime>
#include <QApplication>
#include <QCheckBox>

class MTFile : public QFile
{
public:
    MTFile(QString);
    MTFile(QString, QObject *);
    bool copy(QString);
    bool touch(QApplication *);
    bool openAndTouch(QString);
};

class MTEvenDateTime : public QDateTime
{
public:
    MTEvenDateTime(): QDateTime() { /*makeEven();*/ };
    MTEvenDateTime(const QDate & date): QDateTime(date) { /*makeEven();*/ };
    MTEvenDateTime(const QDate & date, const QTime & time, Qt::TimeSpec spec = Qt::LocalTime): QDateTime(date, time, spec) { /*makeEven();*/ };
    MTEvenDateTime(const QDateTime & other): QDateTime(other) { /*makeEven();*/ };

    void setTime(const QTime & time) { QDateTime::setTime(time); /*makeEven();*/ };
    void setTime_t(uint seconds) { QDateTime::setTime_t(seconds); /*makeEven();*/ };
    int compareWith(MTEvenDateTime other, int ignored_secs = 1, bool DST = true) {
        QString this_str = QDateTime::toString("yyyyMMddhhmmss");
        QString other_str = other.toString("yyyyMMddhhmmss");
        if (this_str == other_str) {
            return 0;
        } else if ((DST) &&
                   ((QDateTime::addSecs(0 - 3600).toString("yyyyMMddhhmmss") > other_str) ||
                    (QDateTime::addSecs(3600).toString("yyyyMMddhhmmss") > other_str))) {
            // Ignore Daylight Saving Time (DST), which means time could be shifted by exactly 1 hour
            return 0;
        } else if (this_str > other_str) {
            if (QDateTime::addSecs(0 - ignored_secs).toString("yyyyMMddhhmmss") > other_str) {
                return 1;
            }
        } else if (this_str < other_str) {
            if (QDateTime::addSecs(ignored_secs).toString("yyyyMMddhhmmss") < other_str) {
                return -1;
            }
        }
        return 0;
    };
protected:
    void makeEven() {
        if (time().second() % 2 != 0) { setTime(time().addSecs(1)); }
    };
};

class MTFileInfo : public QFileInfo
{
public:
    MTFileInfo(): QFileInfo() {};
    MTFileInfo(const QString & file): QFileInfo(file) {};
    MTFileInfo(const QFile & file): QFileInfo(file) {};
    MTFileInfo(const QDir & dir, const QString & file): QFileInfo(dir, file) {};
    MTFileInfo(const QFileInfo & fileinfo): QFileInfo(fileinfo) {};

    MTEvenDateTime lastModified() const { return MTEvenDateTime(QFileInfo::lastModified()); };
    MTEvenDateTime lastRead() const { return MTEvenDateTime(QFileInfo::lastRead()); };
};

class MTCheckBoxGroup : public QWidget
{
    Q_OBJECT

public:
    MTCheckBoxGroup(QCheckBox * achb1, QCheckBox * achb2, QWidget * parent = NULL): QWidget(parent) { chb1 = achb1; chb2 = achb2; link(); };

protected:
    void link() {
        QObject::connect(chb1, SIGNAL(stateChanged(int)), this, SLOT(chb1StateChanged(int)));
        QObject::connect(chb2, SIGNAL(stateChanged(int)), this, SLOT(chb2StateChanged(int)));
        chb1->setChecked(false);
        chb2->setChecked(false);
    };

private slots:
    void chb1StateChanged(int state) { if (chb2->isChecked() && state) chb2->setChecked(false); };
    void chb2StateChanged(int state) { if (chb1->isChecked() && state) chb1->setChecked(false); };

private:
    QCheckBox * chb1;
    QCheckBox * chb2;
};

#endif // MTFILE_H
