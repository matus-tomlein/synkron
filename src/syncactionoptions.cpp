#include "syncactionoptions.h"

SyncActionOptions::SyncActionOptions(QMap<QString, QVariant> * settings)
{
    this->settings = settings;
}

SyncActionOptions::~SyncActionOptions()
{
    delete settings;
}

const QVariant SyncActionOptions::value(const QString & key)
{
    return settings->value(key);
}

bool SyncActionOptions::boolValue(const QString & key)
{
    return settings->value(key).toBool();
}

bool SyncActionOptions::createEmptyFolders()
{
    return !settings->value("no_empty_folders").toBool();
}

bool SyncActionOptions::updateOnly()
{
    return settings->value("update_only").toBool();
}
