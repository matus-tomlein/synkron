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

#include "analyseaction.h"

#include "analysefile.h"
#include "folderactiongroup.h"

AnalyseAction::AnalyseAction(FolderActionGroup * fag, SyncExceptionBundle * bundle, SyncFile * sf)
    : SyncAction(fag, bundle, sf)
{
}

void AnalyseAction::copyFile(SyncFile * sf, FolderActionGroup * fag)
{
    sf->setFileStatusInFolder(fag->lastId(), SyncFile::NotFound);
    ((AnalyseFile *) sf)->increaseNumNotFound();

    for (int i = 0; i < sf_queue.count(); ++i) {
        ((AnalyseFile *) sf_queue.at(i))->increaseNumNotFound();
    }

    delete fag;
}

void AnalyseAction::updateFile(SyncFile * sf, FolderActionGroup * fag)
{
    sf->setFileStatusInFolder(fag->lastId(), SyncFile::Obsolete);
    ((AnalyseFile *) sf)->increaseNumObsolete();

    for (int i = 0; i < sf_queue.count(); ++i) {
        ((AnalyseFile *) sf_queue.at(i))->increaseNumObsolete();
    }

    delete fag;
}

bool AnalyseAction::createFolder(SyncFile * sf, FolderActionGroup * fag)
{
    sf->setFileStatusInFolder(fag->lastId(), SyncFile::NotFound);
    ((AnalyseFile *) sf)->increaseNumNotFound();

    for (int i = 0; i < sf_queue.count(); ++i) {
        ((AnalyseFile *) sf_queue.at(i))->increaseNumNotFound();
    }

    delete fag;
    return true;
}

void AnalyseAction::finish(SyncFile * sf)
{
    emit finished((AnalyseFile *) sf);
}

SyncFile * AnalyseAction::createRootSyncFile()
{
    return new AnalyseFile(tr("Root"));
}
