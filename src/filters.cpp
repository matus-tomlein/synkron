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

void MainWindow::addFilter()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Set filter name"),
                                        tr("Filter name:"), QLineEdit::Normal,
                                        tr("filter_%1").arg(filter_list->count()+1), &ok);
    if (ok && !text.isEmpty()) {
        for (int i = 0; i < filter_list->count(); ++i) {
            if (filter_list->item(i)->text()==text) {
                QMessageBox::information(this, tr("Synkron"), tr("A filter with the same name is already in the list")); return;
            }
        }
        addFilter(text, QStringList());
        filter_list->setCurrentRow(filter_list->count()-1);
    }
}

void MainWindow::addFilter(QString text, QStringList exts)
{
    Filter * filter = new Filter(text);
    filter->extensions = exts;
    filter_list->addItem(filter);
    QAction * action;
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    while (i.hasNext()) { i.next();
        action = new QAction(text, i.value()->filters_menu);
        action->setCheckable(true);
        i.value()->filters_menu->addAction(action);
    }
    /*for (int m = 0; m < multi_tabWidget->count(); ++m) {
        item = new QListWidgetItem(text);
        item->setCheckState(Qt::Unchecked);
        ((MultisyncPage *)multi_tabWidget->widget(m))->lw_filters->addItem(item);
    }*/
}

void MainWindow::removeFilter()
{
    if (filter_list->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No filter selected.")); return; }
    Filter * filter = (Filter *) filter_list->currentItem(); QMapIterator<QWidget *, SyncPage *> i(tabs);
    filter_list->clearSelection(); setFiltersEnabled(false);
    //MultisyncPage * multi_page;
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected filter from the list?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        while (i.hasNext()) {
            i.next();
            QList<QAction *> actions = i.value()->filters_menu->actions();
            for (int i = 0; i < actions.count(); ++i) {
                if (actions.at(i)->text() == filter->text()) {
                    delete actions.at(i); break;
                }
            }
            /*}
                for (int n = 0; n < i.value()->lw_filters->count(); ++n) {
                    if (i.value()->lw_filters->item(n)->text()==filter->text()) {
                        delete i.value()->lw_filters->item(n); break;
                    }
            }*/
        }
        filter_extensions_list->clear();
        delete filter;
        break;
        case QMessageBox::No:
            filter_list->setCurrentItem(filter);
            break;
        default:
            break;
    }
}

void MainWindow::addFilterExtension()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Set extension"),
                                        tr("Filter extension name:"), QLineEdit::Normal,
                                        ".ext", &ok);
    if (ok && !text.isEmpty()) {
        text.remove(" ");
        QStringList exts = text.split(",");
        Filter * filter = (Filter *) filter_list->currentItem();
        for (int i = 0; i < exts.count(); ++i) {
            if (filter->extensions.contains(exts.at(i))) continue;
            filter->extensions << exts.at(i);
            filter_extensions_list->addItem(exts.at(i));
        }
    }
}

void MainWindow::removeFilterExtension()
{
    if (filter_extensions_list->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No extension selected.")); return; }
    Filter * filter = (Filter *) filter_list->currentItem();
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected extension from the list?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        if (filter->extensions.indexOf(filter_extensions_list->currentItem()->text())!=-1) {
            filter->extensions.removeAt(filter->extensions.indexOf(filter_extensions_list->currentItem()->text()));
        }
        delete filter_extensions_list->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void MainWindow::filterChanged()
{
    if (filter_list->currentItem()==0) return;
    setFiltersEnabled(true);
    Filter * filter = (Filter *) filter_list->currentItem();
    filter_extensions_list->clear();
    for (int i = 0; i < filter->extensions.count(); ++i) {
        filter_extensions_list->addItem(filter->extensions.at(i));
    }
}

void MainWindow::setFiltersEnabled(bool enable)
{
    filter_remove->setEnabled(enable);
    filter_add_extension->setEnabled(enable);
    filter_remove_extension->setEnabled(enable);
}
