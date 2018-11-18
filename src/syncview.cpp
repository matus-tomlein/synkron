/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2009 Matus Tomlein (matus.tomlein@gmail.com)

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

#include "mainwindow.h"

SyncViewItem::SyncViewItem(AbstractSyncPage * page)
{
    setupUi(this);

    connect(start_btn, SIGNAL(released()), this, SLOT(startSync()));
    connect(stop_btn, SIGNAL(released()), page, SLOT(stopSync()));
    connect(edit_btn, SIGNAL(released()), page, SLOT(showThisPage()));

    stop_btn->setEnabled(false);

    parent_page = page;
}

void MainWindow::toSyncView()
{
    refreshSyncs();
    refreshMultisyncs();
}

void MainWindow::refreshSyncs()
{
    for (int i = 0; i < syncs_syncview->rowCount();) {
        delete syncs_syncview->takeItem(i, 0);
        syncs_syncview->removeRow(i);
    }

    SyncViewItem * item;
    SyncPage * page;
    for (int m = 0; m < tabWidget->count(); ++m) {
        page = tabs.value(tabWidget->widget(m));
        item = new SyncViewItem (page);
        item->setName(page->tab_name->text());
        multisyncs_syncview->setRowHeight(m, item->sizeHint().height());
        syncs_syncview->setRowCount(syncs_syncview->rowCount() + 1);
        syncs_syncview->setCellWidget(syncs_syncview->rowCount() - 1, 0, item);
    }
}

void MainWindow::refreshMultisyncs()
{
    for (int i = 0; i < multisyncs_syncview->rowCount();) {
        delete multisyncs_syncview->takeItem(i, 0);
        multisyncs_syncview->removeRow(i);
    }

    SyncViewItem * item;
    MultisyncPage * page;
    for (int m = 0; m < multi_tabWidget->count(); ++m) {
        page = (MultisyncPage *) multi_tabWidget->widget(m);
        item = new SyncViewItem (page);
        item->setName(page->tab_name->text());
        multisyncs_syncview->setRowHeight(m, item->sizeHint().height());
        multisyncs_syncview->insertRow(multisyncs_syncview->rowCount());
        multisyncs_syncview->setCellWidget(multisyncs_syncview->rowCount() - 1, 0, item);
    }
}

void SyncViewItem::startSync()
{
    start_btn->setEnabled(false);
    stop_btn->setEnabled(true);

    int synced_files = parent_page->sync();
    sync_info_lbl->setText(QString("<span style=\"color: rgb(128, 128, 128)\">&nbsp;%1 %2</span>")
                .arg(synced_files).arg(tr("file(s) synchronised")));

    start_btn->setEnabled(true);
    stop_btn->setEnabled(false);
    parent_page->mp_parent->actionClose_sync->setEnabled(false);
}
