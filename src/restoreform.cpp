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

#include "restoreform.h"
#include "ui_restoreform.h"

#include "backuphandler.h"
#include "backupdatabaserecord.h"
#include "restoretreewidgetitem.h"

#include <QDateTime>

RestoreForm::RestoreForm(BackupHandler * backup_handler, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RestoreForm)
{
    ui->setupUi(this);

    this->backup_handler = backup_handler;

    QObject::connect(ui->tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(itemExpanded(QTreeWidgetItem*)));
}

RestoreForm::~RestoreForm()
{
    delete ui;
}

void RestoreForm::reload()
{
    QStringList * new_dates = backup_handler->newDates();

    QTreeWidgetItem * item;
    for (int i = 0; i < new_dates->count(); ++i) {
        item = new QTreeWidgetItem(ui->tree);
        item->setText(0, QDateTime::fromString(new_dates->at(i), "yyyy.MM.dd-hh.mm.ss").toString(Qt::DefaultLocaleShortDate));
        item->setData(0, Qt::UserRole, new_dates->at(i));
        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        ui->tree->addTopLevelItem(item);
    }

    delete new_dates;
}

void RestoreForm::itemExpanded(QTreeWidgetItem * parent_item)
{
    if (parent_item->childCount() || parent_item->data(0, Qt::UserRole).isNull()) return;

    QList<BackupDatabaseRecord *> * records = backup_handler->recordsByTime(parent_item->data(0, Qt::UserRole).toString());

    if (!records) return;

    for (int i = 0; i < records->count(); ++i) {
        parent_item->addChild(new RestoreTreeWidgetItem(records->at(i), parent_item));
    }
}
