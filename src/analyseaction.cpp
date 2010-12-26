#include "analyseaction.h"

#include "syncfile.h"
#include "folderactiongroup.h"

AnalyseAction::AnalyseAction(Folders * folders, SyncExceptionBundle * bundle)
    : SyncAction(folders, bundle)
{
}

void AnalyseAction::copyFile(SyncFile * sf, FolderActionGroup * fag)
{
    sf->setFileStatusInFolder(fag->lastId(), SyncFile::NotFound);
    delete fag;
}

void AnalyseAction::updateFile(SyncFile * sf, FolderActionGroup * fag)
{
    sf->setFileStatusInFolder(fag->lastId(), SyncFile::Obsolete);
    delete fag;
}

bool AnalyseAction::createFolder(SyncFile * sf, FolderActionGroup * fag)
{
    sf->setFileStatusInFolder(fag->lastId(), SyncFile::NotFound);
    delete fag;
    return true;
}

void AnalyseAction::finish(SyncFile * sf)
{
    emit finished(sf);
}
