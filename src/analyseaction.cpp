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
