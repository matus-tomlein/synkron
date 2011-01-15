/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2011 Matus Tomlein (matus.tomlein@gmail.com)

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

#ifndef BACKUPACTION_H
#define BACKUPACTION_H

#include <QObject>

class QString;
class QDir;
class MTFile;

class BackupAction : public QObject
{
    Q_OBJECT

public:
    BackupAction(QString *);
    ~BackupAction();

    bool backupFile(MTFile *);

private:
    QDir * temp;
    QString * time_str;

    int num_backed_up;

signals:
    void fileBackedUp(const QString &, int, const QString &);
    void commit();
};

#endif // BACKUPACTION_H
