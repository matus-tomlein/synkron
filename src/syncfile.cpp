#include "syncfile.h"

#include <QList>

SyncFile::SyncFile(const QString name)
{
    this->name = name;
    children = NULL;
    setDir(false);
    setBlacklisted(false);

    folders = new QList<int>;
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
    if (!children) children = new QList<SyncFile *>;

    SyncFile * child = new SyncFile(child_name);
    children->append(child);
    return child;
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
    folders->append(i);
}

bool SyncFile::existsInFolder(int i)
{
    for (int n = 0; n < folders->count(); ++n) {
        if (folders->at(n) == i)
            return true;
    }
    return false;
}
