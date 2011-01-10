#ifndef SYNCACTIONGENERALOPTIONS_H
#define SYNCACTIONGENERALOPTIONS_H

#include "syncactionoptions.h"

class SyncActionFolderOptions;

class SyncActionGeneralOptions : public SyncActionOptions
{
public:
    SyncActionGeneralOptions(QMap<QString, QVariant> *);

    bool syncHidden();
    bool syncSubdirs();

    bool createEmptyFolders(int);
    bool canCopy(int);
    bool canUpdate(int);

    void insertFolderOptions(int, SyncActionFolderOptions *);

private:
    QMap<int, SyncActionFolderOptions *> folder_options;
};

#endif // SYNCACTIONGENERALOPTIONS_H
