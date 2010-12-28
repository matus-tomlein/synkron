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

#include "exceptions.h"
#include "exceptionbundle.h"
#include "settings.h"

#include <QStringList>

Exceptions::Exceptions()
{
    bundles = new QList<ExceptionBundle *>;
}

ExceptionBundle * Exceptions::at(int i)
{
    return bundles->at(i);
}

int Exceptions::count()
{
    return bundles->count();
}

/**
  * Calls each bundle to save its settings in the exceptions group.
  */
void Exceptions::save(Settings * settings)
{
    settings->beginGroup("exceptions");
    settings->remove("");

    for (int i = 0; i < count(); ++i) {
        at(i)->save(settings);
    }

    settings->endGroup();
}

/**
  * Creates bundles by calling addBundle with the bundle id and the settings. Run at start up.
  */
void Exceptions::load(Settings * settings)
{
    settings->beginGroup("exceptions");

    QStringList exception_bundles = settings->childGroups();
    for (int i = 0; i < exception_bundles.count(); ++i) {
        int e_id = exception_bundles.at(i).toInt();

        ExceptionBundle * bundle = addBundle(e_id, false);
        bundle->load(settings);

        emit bundleAddedSig(bundle);
    }

    settings->endGroup();

    if (!bundles->count()) {
        ExceptionBundle * bundle = addBundle(1, false);
        bundle->setValue("default", true);
        bundle->setValue("name", QObject::tr("General"));

        emit bundleAddedSig(bundle);
    }
}

/**
  * Adds a new bundle with the index. Emits signal if sig is true (default).
  */
ExceptionBundle * Exceptions::addBundle(int b_id, bool sig)
{
    ExceptionBundle * bundle = new ExceptionBundle(b_id);
    bundles->append(bundle);
    QObject::connect(bundle, SIGNAL(changed(ExceptionBundle*)), this, SIGNAL(bundleChangedSig(ExceptionBundle*)));

    if (sig) emit bundleAddedSig(bundle);
    return bundle;
}

/**
  * Adds a new bundle. Assigns it the next highest index.
  */
ExceptionBundle * Exceptions::addBundle()
{
    return addBundle(getHighestBundleIndex() + 1);
}

/**
  * Returns the highest available index of all bundles.
  */
int Exceptions::getHighestBundleIndex()
{
    int n = 0;
    for (int i = 0; i < bundles->count(); ++i) {
        if (at(i)->index() > n) n = bundles->at(i)->index();
    }
    return n;
}

void Exceptions::removeBundle(ExceptionBundle * bundle)
{
    int i = bundle->index();
    delete bundles->takeAt(bundles->indexOf(bundle));

    emit bundleRemovedSig(i);
}

ExceptionBundle * Exceptions::bundleById(int b_id)
{
    for (int i = 0; i < bundles->count(); ++i) {
        if (bundles->at(i)->index() == b_id) return bundles->at(i);
    }
    return NULL;
}
