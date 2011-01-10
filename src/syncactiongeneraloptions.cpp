#include "syncactiongeneraloptions.h"

#include "syncactionfolderoptions.h"

SyncActionGeneralOptions::SyncActionGeneralOptions(QMap<QString, QVariant> * opts)
    : SyncActionOptions(opts)
{
}

bool SyncActionGeneralOptions::syncSubdirs()
{
    return !settings->value("sync_no_subdirs").toBool();
}

void SyncActionGeneralOptions::insertFolderOptions(int f_id, SyncActionFolderOptions * safo)
{
    folder_options.insert(f_id, safo);
}

bool SyncActionGeneralOptions::syncHidden()
{
    return settings->value("sync_hidden").toBool();
}

bool SyncActionGeneralOptions::createEmptyFolders(int f_id)
{
    return SyncActionOptions::createEmptyFolders() && folder_options.value(f_id)->createEmptyFolders();
}

bool SyncActionGeneralOptions::canCopy(int f_id)
{
    return !settings->value("update_only").toBool() && folder_options.value(f_id)->canCopy();
}

bool SyncActionGeneralOptions::canUpdate(int f_id)
{
    return folder_options.value(f_id)->canUpdate();
}
