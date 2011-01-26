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

#ifndef SYNCDATABASE_H
#define SYNCDATABASE_H

class SyncFile;

class QString;
class QSqlDatabase;

template<class T>
class QList;

class SyncDatabase
{
public:
    SyncDatabase(int, QString *);
    ~SyncDatabase();

    void setupRootSyncFile(SyncFile *);

private:
    bool createDatabase();
    void setupSyncFile(SyncFile *);

    int page_id;
    QString * temp_path;
    QSqlDatabase * db;
};

#endif // SYNCDATABASE_H
