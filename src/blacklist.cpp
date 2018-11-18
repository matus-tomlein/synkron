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

void MainWindow::toBlacklist()
{
    blacklist_fileslist->clear(); blacklist_folderslist->clear(); blacklist_extslist->clear();
    QListWidgetItem * item;
    for (int i = 0; i < files_blacklist.count(); ++i) {
        item = new QListWidgetItem(files_blacklist.at(i));
        blacklist_fileslist->addItem(item);
    }
    for (int i = 0; i < folders_blacklist.count(); ++i) {
        item = new QListWidgetItem(folders_blacklist.at(i));
        blacklist_folderslist->addItem(item);
    }
    for (int i = 0; i < exts_blacklist.count(); ++i) {
        item = new QListWidgetItem(exts_blacklist.at(i));
        blacklist_extslist->addItem(item);
    }
}

void MainWindow::addFileToBlacklist()
{
    MTPathDialogue * dialogue = new MTPathDialogue;
    QString file = dialogue->getPath(false);
    if (file.isEmpty() || file == ".") { return; }
    files_blacklist << file;
    blacklist_fileslist->addItem(file);
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    while (i.hasNext()) { i.next();
        if (!i.value()->files_blacklist.contains(file))
            i.value()->files_blacklist << file;
    }
    for (int m = 0; m < multi_tabWidget->count(); ++m) {
        if (!((MultisyncPage *)multi_tabWidget->widget(m))->files_blacklist.contains(file))
            ((MultisyncPage *)multi_tabWidget->widget(m))->files_blacklist << file;
    }
}

void MainWindow::removeFileFromBlacklist()
{
    if (blacklist_fileslist->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No file selected.")); return; }
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected file from blacklist?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QString text;
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        text = blacklist_fileslist->currentItem()->text();
        if (files_blacklist.contains(text)) {
            files_blacklist.removeAt(files_blacklist.indexOf(text));
        }
        while (i.hasNext()) { i.next();
            if (i.value()->files_blacklist.contains(text))
                i.value()->files_blacklist.removeAt(i.value()->files_blacklist.indexOf(text));
        }
        for (int m = 0; m < multi_tabWidget->count(); ++m) {
            if (((MultisyncPage *)multi_tabWidget->widget(m))->files_blacklist.contains(text))
                ((MultisyncPage *)multi_tabWidget->widget(m))->files_blacklist.removeAt(((MultisyncPage *)multi_tabWidget->widget(m))->files_blacklist.indexOf(text));
        }
        delete blacklist_fileslist->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void MainWindow::addFolderToBlacklist()
{
    //QString folder = getPathDialogue(true);
    MTPathDialogue * dialogue = new MTPathDialogue;
    QString folder = dialogue->getPath(true);
    if (folder.isEmpty() || folder == ".") { return; }
    QDir dir(folder); folder = dir.path();
    folders_blacklist << folder;
    blacklist_folderslist->addItem(folder);
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    while (i.hasNext()) { i.next();
        if (!i.value()->folders_blacklist.contains(folder))
            i.value()->folders_blacklist << folder;
    }
    for (int m = 0; m < multi_tabWidget->count(); ++m) {
        if (!((MultisyncPage *)multi_tabWidget->widget(m))->folders_blacklist.contains(folder))
            ((MultisyncPage *)multi_tabWidget->widget(m))->folders_blacklist << folder;
    }
}

void MainWindow::removeFolderFromBlacklist()
{
    if (blacklist_folderslist->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No folder selected.")); return; }
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected folder from blacklist?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QString text;
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        text = blacklist_folderslist->currentItem()->text();
        if (folders_blacklist.contains(text)) {
                folders_blacklist.removeAt(folders_blacklist.indexOf(text));
        }
        while (i.hasNext()) { i.next();
            if (i.value()->folders_blacklist.contains(text))
                i.value()->folders_blacklist.removeAt(i.value()->folders_blacklist.indexOf(text));
        }
        for (int m = 0; m < multi_tabWidget->count(); ++m) {
            if (((MultisyncPage *)multi_tabWidget->widget(m))->folders_blacklist.contains(text))
                ((MultisyncPage *)multi_tabWidget->widget(m))->folders_blacklist.removeAt(((MultisyncPage *)multi_tabWidget->widget(m))->folders_blacklist.indexOf(text));
        }
        delete blacklist_folderslist->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void MainWindow::addExtToBlacklist()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Set extension"),
                                        tr("Extension name:"), QLineEdit::Normal,
                                        ".ext", &ok);
    if (ok && !text.isEmpty()) {
        text.remove(" ");
        QStringList exts = text.split(",");
        for (int i = 0; i < exts.count(); ++i) {
            if (exts_blacklist.contains(exts.at(i))) continue;
            exts_blacklist << exts.at(i);
            blacklist_extslist->addItem(exts.at(i));
            QMapIterator<QWidget *, SyncPage *> t(tabs);
            while (t.hasNext()) { t.next();
            if (!t.value()->exts_blacklist.contains(exts.at(i)))
                t.value()->exts_blacklist << exts.at(i);
            }
            for (int m = 0; m < multi_tabWidget->count(); ++m) {
                if (!((MultisyncPage *)multi_tabWidget->widget(m))->exts_blacklist.contains(exts.at(i)))
                    ((MultisyncPage *)multi_tabWidget->widget(m))->exts_blacklist << exts.at(i);
            }
        }
    }
}

void MainWindow::removeExtFromBlacklist()
{
    if (blacklist_extslist->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No extension selected.")); return; }
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected extension from blacklist?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QString text;
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        text = blacklist_extslist->currentItem()->text();
        if (exts_blacklist.contains(text)) {
            exts_blacklist.removeAt(exts_blacklist.indexOf(text));
        }
        while (i.hasNext()) { i.next();
            if (i.value()->exts_blacklist.contains(text))
                i.value()->exts_blacklist.removeAt(i.value()->exts_blacklist.indexOf(text));
        }
        for (int m = 0; m < multi_tabWidget->count(); ++m) {
            if (((MultisyncPage *)multi_tabWidget->widget(m))->exts_blacklist.contains(text))
                ((MultisyncPage *)multi_tabWidget->widget(m))->exts_blacklist.removeAt(((MultisyncPage *)multi_tabWidget->widget(m))->exts_blacklist.indexOf(text));
        }
        delete blacklist_extslist->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void AbstractSyncPage::addFileToBlacklist()
{
    //QString file = mp_parent->getPathDialogue(false);
    MTPathDialogue * dialogue = new MTPathDialogue;
    QString file = dialogue->getPath(false);
    addFileToBlacklist(file);
}

void AbstractSyncPage::addFileToBlacklist(QString file)
{
    if (file.isEmpty() || file == ".") { return; }
    files_blacklist << file;
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(file);
    blacklist_fileslist->addItem(item);
}

void AbstractSyncPage::removeFileFromBlacklist()
{
    if (blacklist_fileslist->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No file selected.")); return; }
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected file from blacklist?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        if (files_blacklist.contains(blacklist_fileslist->currentItem()->text())) {
                files_blacklist.removeAt(files_blacklist.indexOf(blacklist_fileslist->currentItem()->text()));
        }
        delete blacklist_fileslist->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void AbstractSyncPage::removeFileFromBlacklist(QString file_name)
{
    if (files_blacklist.contains(file_name)) {
        files_blacklist.removeAt(files_blacklist.indexOf(file_name));
    }
}

void AbstractSyncPage::addFolderToBlacklist()
{
    //QString folder = mp_parent->getPathDialogue(true);
    MTPathDialogue * dialogue = new MTPathDialogue;
    QString folder = dialogue->getPath(true);
    addFolderToBlacklist(folder);
}

void AbstractSyncPage::addFolderToBlacklist(QString folder)
{
    if (folder.isEmpty() || folder == ".") { return; }
    QDir dir(folder); folder = dir.path();
    folders_blacklist << folder;
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(folder);
    blacklist_folderslist->addItem(item);
}

void AbstractSyncPage::removeFolderFromBlacklist()
{
    if (blacklist_folderslist->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No folder selected.")); return; }
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected folder from blacklist?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        if (folders_blacklist.contains(blacklist_folderslist->currentItem()->text())) {
                folders_blacklist.removeAt(folders_blacklist.indexOf(blacklist_folderslist->currentItem()->text()));
        }
        delete blacklist_folderslist->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void AbstractSyncPage::removeFolderFromBlacklist(QString folder_name)
{
    if (folders_blacklist.contains(folder_name)) {
        folders_blacklist.removeAt(folders_blacklist.indexOf(folder_name));
    }
}

void AbstractSyncPage::addExtToBlacklist()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Set extension"),
                                        tr("Extension name:"), QLineEdit::Normal,
                                        ".ext", &ok);
    if (ok) {
        addExtToBlacklist(text);
    }
}

void AbstractSyncPage::addExtToBlacklist(QString text)
{
    if (!text.isEmpty()) {
        text.remove(" ");
        QStringList exts = text.split(",");
        for (int i = 0; i < exts.count(); ++i) {
            if (exts_blacklist.contains(exts.at(i))) continue;
            exts_blacklist << exts.at(i);
            blacklist_extslist->addItem(exts.at(i));
        }
    }
}

void AbstractSyncPage::removeExtFromBlacklist()
{
    if (blacklist_extslist->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No extension selected.")); return; }
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected extension from blacklist?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        if (exts_blacklist.contains(blacklist_extslist->currentItem()->text())) {
            exts_blacklist.removeAt(exts_blacklist.indexOf(blacklist_extslist->currentItem()->text()));
        }
        delete blacklist_extslist->currentItem();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void AbstractSyncPage::removeExtFromBlacklist(QString ext)
{
    if (exts_blacklist.contains(ext)) {
        exts_blacklist.removeAt(exts_blacklist.indexOf(ext));
    }
    for (int i = 0; i < blacklist_extslist->count(); ++i) {
        if (blacklist_extslist->item(i)->text() == ext) {
            delete blacklist_extslist->item(i);
        }
    }
}

void AbstractSyncPage::editBlacklist()
{
    blacklist_fileslist->clear();
    blacklist_folderslist->clear();
    blacklist_extslist->clear();
    for (int i = 0; i < files_blacklist.count(); ++i) {
        blacklist_fileslist->addItem(files_blacklist.at(i));
    }
    for (int i = 0; i < folders_blacklist.count(); ++i) {
        blacklist_folderslist->addItem(folders_blacklist.at(i));
    }
    for (int i = 0; i < exts_blacklist.count(); ++i) {
        blacklist_extslist->addItem(exts_blacklist.at(i));
    }
    blacklistStwChangeIndex(1);
}
