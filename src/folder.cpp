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

#include "folder.h"
#include "settings.h"

Folder::Folder(int id)
{
    this->id = id;

    settings_map = new QMap<QString, QVariant>;
    setLabel(QObject::tr("Folder #%1").arg(index()));
}

const QString Folder::path()
{
    return settings_map->value("path").toString();
}

void Folder::setPath(const QString & p)
{
    settings_map->insert("path", p);
}

const QString Folder::label()
{
    return settings_map->value("label").toString();
}

void Folder::setLabel(const QString & l)
{
    settings_map->insert("label", l);
}

void Folder::save(Settings * settings)
{
    settings->beginGroup(QVariant(index()).toString());
    settings->remove("");

    QMapIterator<QString, QVariant> i(*settings_map);
    while (i.hasNext()) {
        i.next();
        settings->setValue(i.key(), i.value());
    }

    settings->endGroup();
}

void Folder::load(Settings * settings)
{
    settings->beginGroup(QVariant(index()).toString());

    QStringList keys = settings->allKeys();
    for (int i = 0; i < keys.count(); ++i) {
        settings_map->insert(keys.at(i), settings->value(keys.at(i)));
    }

    settings->endGroup();
}

/**
  * Inserts a record into the settings_map
  */
void Folder::setValue(const QString & key, const QVariant & value)
{
    settings_map->insert(key, value);
}

/**
  * Returns value for the given key from the settings_map
  */
const QVariant Folder::value(const QString & key)
{
    return settings_map->value(key);
}
