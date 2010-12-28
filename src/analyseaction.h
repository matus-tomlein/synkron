#ifndef ANALYSEACTION_H
#define ANALYSEACTION_H

#include "syncaction.h"

class AnalyseFile;

class AnalyseAction : public SyncAction
{
    Q_OBJECT

public:
    AnalyseAction(FolderActionGroup *, SyncExceptionBundle *, SyncFile * sf = NULL);

private:
    void copyFile(SyncFile *, FolderActionGroup *);
    void updateFile(SyncFile *, FolderActionGroup *);
    bool createFolder(SyncFile *, FolderActionGroup *);

    SyncFile * createRootSyncFile();
    void finish(SyncFile *);

signals:
    void finished(AnalyseFile *);
};

#endif // ANALYSEACTION_H
