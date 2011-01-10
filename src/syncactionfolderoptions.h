#ifndef SYNCACTIONFOLDEROPTIONS_H
#define SYNCACTIONFOLDEROPTIONS_H

#include "syncactionoptions.h"

class SyncActionFolderOptions : public SyncActionOptions
{
public:
    SyncActionFolderOptions(QMap<QString, QVariant> *);

    bool canCopy();
    bool canUpdate();
};

#endif // SYNCACTIONFOLDEROPTIONS_H
