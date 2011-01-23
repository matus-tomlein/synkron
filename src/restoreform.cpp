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

    QObject::connect(ui->items_tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(itemExpanded(QTreeWidgetItem*)));
    QObject::connect(ui->items_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));
    QObject::connect(ui->select_latest_btn, SIGNAL(clicked()), this, SLOT(selectLatest()));
    QObject::connect(ui->restore_btn, SIGNAL(clicked()), this, SLOT(restoreSelected()));
    QObject::connect(ui->delete_btn, SIGNAL(clicked()), this, SLOT(deleteSelected()));

    QObject::connect(backup_handler, SIGNAL(actionFinished(BackupDatabaseRecord*)), this, SLOT(recordActionFinished(BackupDatabaseRecord*)));
    QObject::connect(backup_handler, SIGNAL(actionFailed(BackupDatabaseRecord*)), this, SLOT(recordActionFailed(BackupDatabaseRecord*)));
    QObject::connect(backup_handler, SIGNAL(restoreActionsFinished()), this, SLOT(allActionsFinished()));

    ui->items_tree->header()->setStretchLastSection(false);
    ui->items_tree->header()->setResizeMode(PathCol, QHeaderView::Stretch);
    ui->items_tree->header()->setResizeMode(RestoreCol, QHeaderView::ResizeToContents);
    ui->items_tree->header()->setResizeMode(DeleteCol, QHeaderView::ResizeToContents);

    items_in_action = new QList<RestoreTreeWidgetItem *>;
}

RestoreForm::~RestoreForm()
{
    delete ui;
}

void RestoreForm::reload()
{
    QStringList * new_dates = backup_handler->newDates();

    RestoreTreeWidgetItem * item;
    for (int i = 0; i < new_dates->count(); ++i) {
        item = new RestoreTreeWidgetItem(ui->items_tree);
        item->setText(0, QDateTime::fromString(new_dates->at(i), "yyyy.MM.dd-hh.mm.ss").toString(Qt::DefaultLocaleShortDate));
        item->setData(0, Qt::UserRole, new_dates->at(i));
        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        ui->items_tree->addTopLevelItem(item);
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

void RestoreForm::itemClicked(QTreeWidgetItem * qitem, int column)
{
    RestoreTreeWidgetItem * item = (RestoreTreeWidgetItem *) qitem;
    if (!item->databaseRecord()) return;

    switch (column) {
    case RestoreCol:
        items_in_action->append(item);
        backup_handler->restoreRecord(item->databaseRecord());
        break;

    case DeleteCol:
        items_in_action->append(item);
        backup_handler->removeRecord(item->databaseRecord());
        break;
    }
}

void RestoreForm::recordActionFinished(BackupDatabaseRecord * record)
{
    for (int i = 0; i < items_in_action->count(); ++i) {
        if (items_in_action->at(i)->databaseRecord() == record) {
            delete items_in_action->takeAt(i);
            break;
        }
    }
}

void RestoreForm::recordActionFailed(BackupDatabaseRecord * record)
{
    for (int i = 0; i < items_in_action->count(); ++i) {
        if (items_in_action->at(i)->databaseRecord() == record) {
            items_in_action->takeAt(i)->restoreActionFailed();
            break;
        }
    }
}

void RestoreForm::allActionsFinished()
{
    items_in_action->clear();
}

void RestoreForm::selectLatest()
{
    if (!ui->items_tree->topLevelItemCount()) return;

    RestoreTreeWidgetItem * parent = (RestoreTreeWidgetItem *) ui->items_tree->topLevelItem(ui->items_tree->topLevelItemCount() - 1);

    for (int i = 0; i < parent->childCount(); ++i) {
        parent->child(i)->setCheckState(0, Qt::Checked);
    }
}

QList<BackupDatabaseRecord *> * RestoreForm::getSelectedRecords()
{
    QList<BackupDatabaseRecord *> * bdrs = new QList<BackupDatabaseRecord *>;
    QTreeWidgetItem * parent;
    RestoreTreeWidgetItem * item;

    for (int i = 0; i < ui->items_tree->topLevelItemCount(); ++i) {
        parent = ui->items_tree->topLevelItem(i);

        for (int n = 0; n < parent->childCount(); ++n) {
            if (parent->child(n)->checkState(0) == Qt::Checked) {
                item = (RestoreTreeWidgetItem *) parent->child(n);
                bdrs->append(item->databaseRecord());
                items_in_action->append(item);
            }
        }
    }

    return bdrs;
}

void RestoreForm::restoreSelected()
{
    QList<BackupDatabaseRecord *> * bdrs = getSelectedRecords();

    if (!bdrs->count()) {
        delete bdrs;
        return;
    }

    backup_handler->restoreRecord(bdrs);
}

void RestoreForm::deleteSelected()
{
    QList<BackupDatabaseRecord *> * bdrs = getSelectedRecords();

    if (!bdrs->count()) {
        delete bdrs;
        return;
    }

    backup_handler->removeRecord(bdrs);
}
