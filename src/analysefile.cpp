#include "analysefile.h"

AnalyseFile::AnalyseFile(const QString name)
    : SyncFile(name)
{
    this->num_deleted = 0;
    this->num_not_found = 0;
    this->num_obsolete = 0;
}

SyncFile * AnalyseFile::addChild(const QString & child_name)
{
    SyncFile * child = new AnalyseFile(child_name);
    addSyncFile(child);
    return child;
}

void AnalyseFile::increaseNumDeleted()
{
    this->num_deleted++;
}

void AnalyseFile::increaseNumNotFound()
{
    this->num_not_found++;
}

void AnalyseFile::increaseNumObsolete()
{
    this->num_obsolete++;
}

int AnalyseFile::numDeleted()
{
    return this->num_deleted;
}

int AnalyseFile::numNotFound()
{
    return this->num_not_found;
}

int AnalyseFile::numObsolete()
{
    return this->num_obsolete;
}

int AnalyseFile::numNotSynced()
{
    return this->num_deleted + this->num_not_found + this->num_obsolete;
}
