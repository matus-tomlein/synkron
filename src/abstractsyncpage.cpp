/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2011 Matus Tomlein (matus.tomlein@gmail.com)

 Synkron is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Synkron is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Synkron; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#include "abstractsyncpage.h"
#include "folders.h"
#include "settings.h"
#include "exceptionbundle.h"
#include "syncexceptionbundle.h"
#include "syncactiongeneraloptions.h"
#include "exceptions.h"
#include "analyseaction.h"
#include "analysefile.h"
#include "folderactiongroup.h"
#include "backuphandler.h"
#include "syncthread.h"
#include "syncdatabase.h"

#include <QMessageBox>

AbstractSyncPage::AbstractSyncPage(int id, Exceptions * exceptions, BackupHandler * backup_handler)
{
    this->id = id;
    this->exceptions = exceptions;
    this->backup_handler = backup_handler;

    settings_map = new QMap<QString, QVariant>;
    exception_bundle_ids_map = new QMap<int, bool>;

    setValue("title", QObject::tr("Sync #%1").arg(index()));

    syncdb = new SyncDatabase(id, backup_handler->tempPath());
}

Folder * AbstractSyncPage::addFolder(int f_id)
{
    return folders->addFolder(f_id);
}

void AbstractSyncPage::closeFolder(int f_id)
{
    folders->closeFolder(f_id);
}

/**
  * Saves the tab settings in a group with the name of the tab id inside the group syncs in the settings.
  * Calls folders to save themselves.
  */
void AbstractSyncPage::save(Settings * settings)
{
    settings->beginGroup(QVariant(index()).toString());
    settings->remove("");

    // Settings +++
    QMapIterator<QString, QVariant> i(*settings_map);
    while (i.hasNext()) { i.next();
        settings->setValue(i.key(), i.value());
    }
    // Settings ---

    // Exceptions +++
    settings->beginGroup("exceptions");
    settings->remove("");
    QMapIterator<int, bool> e(*exception_bundle_ids_map);
    while (e.hasNext()) { e.next();
        settings->setValue(QVariant(e.key()).toString(), e.value());
    }
    settings->endGroup();
    // Exceptions ---

    folders->save(settings);

    settings->endGroup();
}

/**
  * Loads the tab settings from a group with the name of the tab id inside the group syncs in the settings.
  * Calls folders to load themselves.
  */
void AbstractSyncPage::load(Settings * settings)
{
    settings->beginGroup(QVariant(index()).toString());

    QStringList keys = settings->childKeys();
    for (int i = 0; i < keys.count(); ++i) {
        settings_map->insert(keys.at(i), settings->value(keys.at(i)));
    }

    // Exceptions +++
    settings->beginGroup("exceptions");
    keys = settings->childKeys();
    for (int i = 0; i < keys.count(); ++i) {
        exception_bundle_ids_map->insert(keys.at(i).toInt(), settings->value(keys.at(i)).toBool());
    }
    settings->endGroup();
    // Exceptions ---

    folders->load(settings);

    settings->endGroup();
}

/**
  * Inserts a record into the settings_map
  */
void AbstractSyncPage::setValue(const QString & key, const QVariant & value)
{
    settings_map->insert(key, value);
}

/**
  * Returns value for the given key from the settings_map
  */
const QVariant AbstractSyncPage::value(const QString & key)
{
    return settings_map->value(key);
}

void AbstractSyncPage::addExceptionBundle(ExceptionBundle * bundle)
{
    if (!exception_bundle_ids_map->contains(bundle->index()))
        exception_bundle_ids_map->insert(bundle->index(), false);

    emit exceptionBundleAdded(bundle);
}

void AbstractSyncPage::removeExceptionBundle(int i)
{
    exception_bundle_ids_map->remove(i);

    emit exceptionBundleRemoved(i);
}

bool AbstractSyncPage::exceptionBundleChecked(int i)
{
    return exception_bundle_ids_map->value(i);
}

ExceptionBundle * AbstractSyncPage::exceptionBundleById(int i)
{
    return exceptions->bundleById(i);
}

ExceptionBundle * AbstractSyncPage::exceptionBundleAt(int i)
{
    return exceptions->at(i);
}

int AbstractSyncPage::exceptionBundleCount()
{
    return exceptions->count();
}

void AbstractSyncPage::checkExceptionBundle(int i, bool b)
{
    exception_bundle_ids_map->insert(i, b);
}

/**
  * Returns a SyncExceptionBundle object, which contains all data in all groups of all used bundles in the tab.
  * SyncExceptionBundles are used in syncs.
  */
SyncExceptionBundle * AbstractSyncPage::syncExceptionBundle()
{
    SyncExceptionBundle * bundle = new SyncExceptionBundle;

    QMapIterator<int, bool> i(*exception_bundle_ids_map);
     while (i.hasNext()) { i.next();
         if (i.value()) {
             ExceptionBundle * b = exceptionBundleById(i.key());
             if (b)
                 bundle->importBundle(b);
         }
     }

     return bundle;
}

QMap<QString, QVariant> * AbstractSyncPage::getCopyOfSettings()
{
    return new QMap<QString, QVariant>(*settings_map);
}

SyncActionGeneralOptions * AbstractSyncPage::syncOptions()
{
    SyncActionGeneralOptions * sago = new SyncActionGeneralOptions(getCopyOfSettings());
    folders->insertFolderOptions(sago);
    return sago;
}

BackupHandler * AbstractSyncPage::backupHandler()
{
    return backup_handler;
}

void AbstractSyncPage::startSync()
{
    startSync(NULL);
}

void AbstractSyncPage::startSync(SyncAction * sa)
{
    emit syncStarted();

    if (!sa)
        sa = new SyncAction(folders->folderActionGroup(), syncExceptionBundle(), syncOptions(), backup_handler->backupAction(), syncdb);

    QObject::connect(sa, SIGNAL(messageBox(QString)), this, SIGNAL(messageBox(QString)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(filesCounted(int)), this, SIGNAL(setProgressBarMaximum(int)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(anotherItemChecked()), this, SIGNAL(increaseProgressBarValue()), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(finished(int, int)), this, SIGNAL(syncFinished(int,int)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(syncOutMessage(SyncOutMessage*)), this, SIGNAL(messageFromSync(SyncOutMessage*)), Qt::QueuedConnection);

    new SyncThread(sa);
}

void AbstractSyncPage::startSync(SyncFile * sf, FolderActionGroup * fag)
{
    startSync(new SyncAction(fag, syncExceptionBundle(), syncOptions(), backup_handler->backupAction(), syncdb, sf));
}

void AbstractSyncPage::startAnalysis()
{
    emit analysisStarted();

    AnalyseAction * aa = new AnalyseAction(folders->folderActionGroup(), syncExceptionBundle(), syncOptions(), backup_handler->backupAction(), syncdb);

    QObject::connect(aa, SIGNAL(finished(AnalyseFile*)), this, SIGNAL(analysisFinished(AnalyseFile*)), Qt::QueuedConnection);

    new SyncThread(aa);
}
