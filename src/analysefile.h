#ifndef ANALYSEFILE_H
#define ANALYSEFILE_H

#include "syncfile.h"

class AnalyseFile : public SyncFile
{
public:
    AnalyseFile(const QString);

    SyncFile * addChild(const QString &);

    void increaseNumObsolete();
    void increaseNumDeleted();
    void increaseNumNotFound();

    int numObsolete();
    int numDeleted();
    int numNotFound();
    int numNotSynced();

private:
    int num_obsolete;
    int num_deleted;
    int num_not_found;
};

#endif // ANALYSEFILE_H
