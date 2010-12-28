#include "syncfile.h"

#include <QList>

SyncFile::SyncFile(const QString name)
{
    this->name = name;
    children = NULL;
    setDir(false);
    setBlacklisted(false);

    folders = new QList<FolderStatus>;
}

SyncFile::~SyncFile()
{
    if (children) {
        for (int i = childCount() - 1; i >= 0; --i) {
            delete children->takeAt(i);
        }
    }

    delete folders;
}

const QString & SyncFile::getName()
{
    return name;
}

void SyncFile::setName(const QString name)
{
    this->name = name;
}

/**
  * Creates a new SyncFile object. Appends it into the children list. Returns its pointer.
  */
SyncFile * SyncFile::addChild(const QString & child_name)
{
    SyncFile * child = new SyncFile(child_name);
    addSyncFile(child);
    return child;
}

void SyncFile::addSyncFile(SyncFile * sf)
{
    if (!children) children = new QList<SyncFile *>;

    children->append(sf);
}

/**
  * Returns number of child SyncFiles in the folder on one level.
  */
int SyncFile::childCount()
{
    if (children) return children->count();
    return 0;
}

/**
  * Returns number of children on every level under the item. By default counts self too.
  */
int SyncFile::count(bool include_self)
{
    int c = 0;
    if (children) {
        for (int i = 0; i < children->count(); ++i) {
            c += children->at(i)->count();
        }
    }

    if (include_self) c++;
    return c;
}

SyncFile * SyncFile::childAt(int i)
{
    if (children) return children->at(i);
    return NULL;
}

void SyncFile::addFolder(int i)
{
    FolderStatus fstat;
    fstat.file_stat = SyncFile::OK;
    fstat.folder_id = i;
    folders->append(fstat);
}

bool SyncFile::existsInFolder(int i)
{
    return fileStatusInFolder(i) != SyncFile::NotFound;
}

SyncFile::FileStatus SyncFile::fileStatusInFolder(int i)
{
    for (int n = 0; n < folders->count(); ++n) {
        if (folders->at(n).folder_id == i)
            return folders->at(n).file_stat;
    }
    return SyncFile::NotFound;
}

void SyncFile::setFileStatusInFolder(int i, FileStatus status)
{
    for (int n = 0; n < folders->count(); ++n) {
        if (folders->at(n).folder_id == i) {
            (*folders)[n].file_stat = status;
            return;
        }
    }
    FolderStatus fstat;
    fstat.file_stat = status;
    fstat.folder_id = i;
    folders->append(fstat);
}
