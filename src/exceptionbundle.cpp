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

#include "exceptionbundle.h"
#include "settings.h"
#include "fileblacklistgroup.h"
#include "folderblacklistgroup.h"
#include "filtergroup.h"

#include <QStringList>

ExceptionBundle::ExceptionBundle(int id) : AbstractExceptionBundle()
{
    this->id = id;

    //items = new QList<ExceptionItem *>;
    settings_map = new QMap<QString, QVariant>;

    setValue("name", QObject::tr("Bundle #%1").arg(index()));
    setValue("default", false);
}

/**
  * Saves settings at quit. Triggered from outside.
  */
void ExceptionBundle::save(Settings * settings)
{
    settings->beginGroup(QVariant(index()).toString());
    settings->remove("");

    QMapIterator<QString, QVariant> i(*settings_map);
    while (i.hasNext()) {
        i.next();
        settings->setValue(i.key(), i.value());
    }

    for (int i = 0; i < exception_groups->count(); ++i) {
        exception_groups->at(i)->save(settings);
    }

    settings->endGroup();
}

/**
  * Loads settings. Run at start up, triggered from outside.
  */
void ExceptionBundle::load(Settings * settings)
{
    settings->beginGroup(QVariant(index()).toString());

    QStringList keys = settings->childKeys();
    for (int i = 0; i < keys.count(); ++i) {
        settings_map->insert(keys.at(i), settings->value(keys.at(i)));
    }

    for (int i = 0; i < exception_groups->count(); ++i) {
        exception_groups->at(i)->load(settings);
    }

    settings->endGroup();
}

/**
  * Inserts the key and value to settings_map.
  */
void ExceptionBundle::setValue(const QString & key, const QVariant & value)
{
    settings_map->insert(key, value);

    emit changed(this);
}

/**
  * Returns value for the key from the settings_map.
  */
const QVariant ExceptionBundle::value(const QString & key)
{
    return settings_map->value(key);
}

const QString ExceptionBundle::name()
{
    return value("name").toString();
}
