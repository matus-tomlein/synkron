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

#ifndef SYNCACTION_H
#define SYNCACTION_H

class Folders;
class FolderActionGroup;
class SyncFile;
class SyncExceptionBundle;
class FileCompare;
class SyncOutMessage;
class MTFile;

class QFileInfo;

#include <QThread>

class SyncAction : public QThread
{
    Q_OBJECT

public:
    SyncAction(FolderActionGroup *, SyncExceptionBundle *, SyncFile * = NULL);
    ~SyncAction();

    void run();

public slots:
    void start(Priority = InheritPriority);

private:
    FolderActionGroup * starting_fag;
    SyncFile * starting_sf;
    FileCompare * file_compare;
    SyncExceptionBundle * exception_bundle;

    int skipped_count;

    void createSyncFileFromFolders(SyncFile *, FolderActionGroup *);
    void sync(SyncFile *, FolderActionGroup *);
    virtual void copyFile(SyncFile *, FolderActionGroup *);
    virtual void updateFile(SyncFile *, FolderActionGroup *);
    virtual bool createFolder(SyncFile *, FolderActionGroup *);
    bool backupFile(MTFile *);

    virtual SyncFile * createRootSyncFile();
    virtual void finish(SyncFile *);

protected:
    QList<SyncFile *> sf_queue;

signals:
    void messageBox(const QString);
    void filesCounted(int);
    void finished();

    void fileCopied(const QString, const QString);
    void fileUpdated(const QString, const QString);
    void anotherItemChecked();

    void syncOutMessage(SyncOutMessage *);
};

#endif // SYNCACTION_H
