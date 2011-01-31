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

#include "syncaction.h"

#include "filecompare.h"
#include "folderactiongroup.h"
#include "syncfile.h"
#include "syncexceptionbundle.h"
#include "syncoutmessage.h"
#include "exceptiongroup.h"
#include "mtfile.h"
#include "syncactiongeneraloptions.h"
#include "backupaction.h"
#include "syncdatabase.h"

#include <QSet>
#include <QElapsedTimer>

SyncAction::SyncAction(FolderActionGroup * fag, SyncExceptionBundle * bundle, SyncActionGeneralOptions * opts, BackupAction * ba, SyncDatabase * syncdb, SyncFile * sf)
{
    starting_fag = fag;
    exception_bundle = bundle;
    options = opts;
    backup_action = ba;
    starting_sf = sf;
    skipped_count = 0;
    this->syncdb = syncdb;
    file_compare = new FileCompare();
}

SyncAction::~SyncAction()
{
    delete starting_fag;
    delete exception_bundle;
    delete options;
    delete backup_action;
}

void SyncAction::start()
{
    skipped_count = 0;
    changed_count = 0;

    QElapsedTimer timer;
    timer.start();

    dir_filters = QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs;
    if (options->syncHidden())
        dir_filters |= QDir::Hidden;

    exception_bundle->updateRootFolder(starting_fag);
    if (!starting_sf) {
        starting_sf = createRootSyncFile();
        starting_sf->setDir(true);
        for (int i = 0; i < starting_fag->count(); ++i) {
            starting_sf->addFolder(starting_fag->idAt(i));
        }
        syncdb->setupRootSyncFile(starting_sf);

        createSyncFileFromFolders(starting_sf, starting_fag);
    }

    emit filesCounted(starting_sf->count());
    sync(starting_sf, starting_fag);

    emit messageBox(QString("TIME %1").arg(timer.elapsed()));

    finish(starting_sf);
}

/**
  * Fills up the parent sync file with all the content of folders in the folder action group.
  * Recursively calls itself when finds a folder.
  */
void SyncAction::createSyncFileFromFolders(SyncFile * parent, FolderActionGroup * fag)
{
    QSet<QString> entries;

    for (int i = 0; i < fag->count(); ++i) {
        //Creating a set of all file names ---------------------------------

        QDir sync_dir(fag->at(i));
        if (!sync_dir.exists()) continue;

        entries.unite(sync_dir.entryList(exception_bundle->filters(), static_cast<QDir::Filter>(dir_filters), (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase)).toSet());
    }

    QFileInfo fi;
    FolderActionGroup * child_dirs_fag = NULL;
    SyncFile * sf = NULL;

    foreach (QString entry, entries) {

        if (exception_bundle->cdAndCheck(entry)) { // +++ Blacklisted +++
            skipped_count++;
        }
        else { // +++ Not in blacklist +++
            sf = parent->addChild(entry);

            for (int n = 0; n < fag->count(); ++n) {
                // Obtaining absolute paths for the file names ---------------------
                fi.setFile(QDir(fag->at(n)).absoluteFilePath(entry));

                if (fi.exists()) {
                    if (fi.isDir()) {
                        sf->setDir(true);
                        if (!child_dirs_fag) child_dirs_fag = new FolderActionGroup;
                        child_dirs_fag->insert(fag->idAt(n), fi.absoluteFilePath());
                    }

                    sf->addFolder(fag->idAt(n));
                }
            }

            //sf->setBlacklisted(exception_bundle->isInBlacklist());
            if (child_dirs_fag) {
                createSyncFileFromFolders(sf, child_dirs_fag);

                delete child_dirs_fag; child_dirs_fag = NULL;
            }
        }

        exception_bundle->cdUp();
    }

    fag = NULL;
}

void SyncAction::sync(SyncFile * parent, FolderActionGroup * fag)
{
    if (!parent)
        return;

    SyncFile * sf = NULL;
    QFileInfo fi;
    QDir dir;

    int latest_index_arr[fag->count()];
    int latest_index = 0, c;
    MTFileInfo * newest_fi = NULL;
    FolderActionGroup * sub_fag = NULL;

    sf_queue << parent;

    for (int i = 0; i < parent->childCount(); ++i) {
        sf = parent->childAt(i);
        latest_index = 0;
        if (newest_fi) {
            delete newest_fi;
            newest_fi = NULL;
        }

        if (sf->isInDatabase() && sf->numFoundFolders() < fag->count()) { // was deleted
            deleteFileOrFolder(sf, fag);
        }
        else if (sf->isDir()) {
            sub_fag = new FolderActionGroup;
            for (int n = 0; n < fag->count(); ++n) {
                dir.setPath(fag->at(n));
                if (!sf->existsInFolder(fag->idAt(n))) {
                    if (options->canCopy(fag->idAt(n))
                        && (sf->childCount() || options->createEmptyFolders(fag->idAt(n)))) {

                        if (createFolder(sf, new FolderActionGroup(fag->idAt(n), dir.absoluteFilePath(sf->getName()))))
                            sub_fag->insert(fag->idAt(n), dir.absoluteFilePath(sf->getName()));

                    }
                } else {
                    sub_fag->insert(fag->idAt(n), dir.absoluteFilePath(sf->getName()));
                }
            }
            if (options->syncSubdirs())
                sync(sf, sub_fag);
            delete sub_fag;
        }
        else { // is not dir
            for (int n = 0; n < fag->count(); ++n) {
                if (sf->fileStatusInFolder(fag->idAt(n)) == SyncFile::Obsolete) { // Found, checked and obsolete
                    latest_index_arr[n] = latest_index - 1;
                }
                else if (sf->existsInFolder(fag->idAt(n))) { // Found but not checked yet
                    dir.setPath(fag->at(n));
                    fi.setFile(dir.absoluteFilePath(sf->getName()));
                    if (!newest_fi) {
                        newest_fi = new MTFileInfo(fi, fag->idAt(n));
                        latest_index_arr[n] = latest_index;
                    }
                    else {
                        if (sf->fileStatusInFolder(fag->idAt(n)) == SyncFile::OK || // checked before and is newest
                            (c = file_compare->compareTwoFiles(&fi, newest_fi)) == 0) { // equal to the newest
                            latest_index_arr[n] = latest_index;
                        }
                        else if (c > 0) { // fi is newer
                            newest_fi->setFile(fi.absoluteFilePath());
                            newest_fi->setFolderId(fag->idAt(n));
                            latest_index_arr[n] = ++latest_index;
                        }
                        else { // fi is older
                            latest_index_arr[n] = latest_index - 1;
                        }
                    }
                }
                else { // Not found
                    latest_index_arr[n] = -2;
                }
            }

            if (latest_index < 0) // probably never happens...
                continue;

            for (int n = 0; n < fag->count(); ++n) {
                dir.setPath(fag->at(n));
                if (latest_index_arr[n] == -2) { // does not exist
                    if (options->canCopy(fag->idAt(n)))
                        copyFile(sf, new FolderActionGroup(newest_fi->folderId(), newest_fi->absoluteFilePath(),
                                                     fag->idAt(n), dir.absoluteFilePath(sf->getName())));
                }
                else if (latest_index_arr[n] < latest_index) { // is obsolete
                    if (options->canUpdate(fag->idAt(n)))
                        updateFile(sf, new FolderActionGroup(newest_fi->folderId(), newest_fi->absoluteFilePath(),
                                                     fag->idAt(n), dir.absoluteFilePath(sf->getName())));
                } else { // is fine
                    sf->setFileStatusInFolder(fag->idAt(n), SyncFile::OK);
                }
            }
            if (newest_fi) {
                sf->setLastModified(newest_fi->lastModified());
            }
        }
        emit this->anotherItemChecked();
    }

    sf_queue.takeLast();
}

void SyncAction::copyFile(SyncFile *, FolderActionGroup * fag)
{
    MTFile source_file(fag->first());
    if (source_file.copy(fag->last())) {
        emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FileCopied, fag));
    } else {
        emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FileCopied, fag, true, source_file.errorString()));
        changed_count++;
    }
}

void SyncAction::updateFile(SyncFile *, FolderActionGroup * fag)
{
    MTFile file(fag->last());

    if (!backup_action->backupFile(&file)) {
        return;
    }

    if (!file.remove()) {
        emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FileUpdated, fag, true, file.errorString()));
        return;
    }

    file.setFileName(fag->first());
    if (!file.copy(fag->last())) {
        emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FileUpdated, fag, true, file.errorString()));
    } else {
        emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FileUpdated, fag));
        changed_count++;
    }
}

bool SyncAction::createFolder(SyncFile *, FolderActionGroup * fag)
{
    if (!QDir().mkpath(fag->first())) {
        emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FolderCreated, fag, true));
        return false;
    }
    emit this->syncOutMessage(new SyncOutMessage(SyncOutMessage::FolderCreated, fag));
    changed_count++;
    return true;
}

bool SyncAction::deleteFileOrFolder(SyncFile *, FolderActionGroup *)
{

    return true;
}

void SyncAction::finish(SyncFile * sf)
{
    if (syncdb) syncdb->saveSyncFile(sf);
    delete sf;
    emit finished(changed_count, skipped_count);
}

SyncFile * SyncAction::createRootSyncFile()
{
    return new SyncFile(tr("Root"));
}
