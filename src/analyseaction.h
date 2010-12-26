#ifndef ANALYSEACTION_H
#define ANALYSEACTION_H

#include "syncaction.h"

class AnalyseAction : public SyncAction
{
    Q_OBJECT

public:
    AnalyseAction(Folders *, SyncExceptionBundle *);

private:
    void copyFile(SyncFile *, FolderActionGroup *);
    void updateFile(SyncFile *, FolderActionGroup *);
    bool createFolder(SyncFile *, FolderActionGroup *);

    void finish(SyncFile *);

signals:
    void finished(SyncFile *);
};

#endif // ANALYSEACTION_H
