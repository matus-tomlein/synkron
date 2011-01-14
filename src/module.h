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

#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QVariant>

class Settings;
class AbstractSyncPage;
class SyncPage;
class Folder;
class Exceptions;
class BackupHandler;

template<class Key, class T>
class QMap;

class Module : public QObject
{
    Q_OBJECT

public:
    Module(const QString &);
    ~Module();

    QVariant value(const QString &);
    void setValue(const QString &, const QVariant &);

    QMapIterator<int, AbstractSyncPage *> * syncIterator();

    Exceptions * getExceptions() { return exceptions; }

    void save();
    void load();

public slots:
    SyncPage * addSync();
    SyncPage * addSync(int);
    void closeSync(int);
    Folder * addSyncFolder(int, int);
    void closeSyncFolder(int, int);

private:
    void setupSyncSettings(const QString &);

    Settings * sync_settings;
    Exceptions * exceptions;
    BackupHandler * backup_handler;

    QMap<QString, QVariant> * settings_map;
    QMap<int, AbstractSyncPage *> * syncs;
};

#endif // MODULE_H
