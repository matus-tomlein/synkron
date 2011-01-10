#include "syncactionfolderoptions.h"

SyncActionFolderOptions::SyncActionFolderOptions(QMap<QString, QVariant> * opts)
    : SyncActionOptions(opts)
{
}

bool SyncActionFolderOptions::canCopy()
{
    return !settings->value("dont_modify").toBool() && !settings->value("update_only").toBool();
}

bool SyncActionFolderOptions::canUpdate()
{
    return !settings->value("dont_modify").toBool();
}
