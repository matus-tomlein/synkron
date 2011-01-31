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

#ifndef SYNCFILE_H
#define SYNCFILE_H

#include <QString>

class MTEvenDateTime;

namespace sf_namespace {
    extern int highest_sf_id;
}

class SyncFile
{
public:
    enum FileStatus {
        UncheckedButFound,
        NotFound,
        OK,
        Obsolete,
        Deleted
    };
    struct FolderStatus {
        SyncFile::FileStatus file_stat;
        int folder_id;
    };

    SyncFile(const QString);
    ~SyncFile();

    SyncFile * addChild(const QString &, int = -1);
    void addSyncFile(SyncFile *);

    void setName(const QString);
    const QString & getName();

    int childCount();
    int count(bool = true);
    SyncFile * childAt(int);

    void addFolder(int);
    bool existsInFolder(int);
    void setFileStatusInFolder(int, FileStatus);
    SyncFile::FileStatus fileStatusInFolder(int);
    int numFoundFolders();

    bool isDir() { return is_dir; }
    void setDir(bool d) { is_dir = d; }

    bool isBlacklisted() { return blacklisted; }
    void setBlacklisted(bool b) { blacklisted = b; }

    int index() { return id; }
    void setIndex(int);

    QString * lastModified() { return last_modified; }
    const QString lastModifiedString();
    void setLastModified(const MTEvenDateTime &);
    void setLastModified(const QString &);

    bool wasModified() { return modified; }
    bool isInDatabase() { return !not_in_db; }

    SyncFile * childByIndex(int);

private:
    int indexOfFolder(int);
    virtual SyncFile * addNewChild(const QString &, int = -1);

    QList<SyncFile *> * children;
    QList<SyncFile::FolderStatus> * folders;

    QString name;
    bool is_dir;
    bool blacklisted;
    bool not_in_db;
    bool modified;
    int id;
    QString * last_modified;
};

#endif // SYNCFILE_H
