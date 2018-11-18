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

// QDesktopServices::openUrl(string);

void SyncPage::goToAnalyse()
{
    sync_log_label->setText(tr("<b>Analysis:</b>"));
    analyse_tree->clear();
    analyse_tree->setColumnCount(0);
    QStringList labels;
    labels << tr("File name");
    if (!sort_analysis_by_action->isChecked()) {
        for (int i = 0; i < sync_folders->count(); ++i) {
            labels << sync_folders->at(i)->label();
        }
    }
    analyse_tree->setHeaderLabels(labels);
    analyse_tree->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
analyse_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
analyse_tree->header()->setResizeMode(0, QHeaderView::Stretch);
#endif

    for (int i = 1; i < labels.count(); ++i) {
#if QT_VERSION >= 0x050000
analyse_tree->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
analyse_tree->header()->setResizeMode(i, QHeaderView::ResizeToContents);
#endif

    }

    logs_stw->setCurrentIndex(1);
    analyseFolders();
}

void MultisyncPage::aboutToShowAnalyseMenu()
{
    analyse_con_menu->clear();
    for (int i = 0; i < list_multi->count(); ++i) {
        analyse_con_menu->addAction(list_multi->item(i)->text());
    }
}

void MultisyncPage::goToAnalyse(QAction * action)
{
    analyse_tree->clear();
    analyse_tree->setColumnCount(0);
    QStringList labels;
    labels << tr("File name");
    labels << tr("Source");
    labels << tr("Destination");
    analyse_tree->setHeaderLabels(labels);
    analyse_tree->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
analyse_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
analyse_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#endif

    for (int i = 1; i < labels.count(); ++i) {
#if QT_VERSION >= 0x050000
analyse_tree->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
analyse_tree->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#endif

    }

    logs_stw->setCurrentIndex(1);
    analyse(action);
}

void MultisyncPage::analyse(QAction * action)
{
    if (syncing) return;
    QTreeWidgetItem * parent_item = new QTreeWidgetItem (analyse_tree);
    parent_item->setText(0, tr("Root directory"));
    QStringList data0; data0 << ""; data0 << "checked";
    parent_item->setData(0, Qt::UserRole, QVariant(data0));
    parent_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16.png"));
    parent_item->setCheckState(0, Qt::Checked);
    if (ignore_blacklist->isChecked()) parent_item->setDisabled(true);
    parent_item->setExpanded(true);

    bool do_return = false;
    MTStringSet folders_set;
    QDir destination(destination_multi->text());
    if (!destination.exists()) {
        parent_item->setText(2, tr("NOT FOUND"));
        parent_item->setForeground(2, QBrush(Qt::red));
    }
    QStringList pathlist = action->text().split("/");
    QString s = pathlist.at(0);
    pathlist[0] = s.remove(":");
    for (int v = 0; v < pathlist.count(); ++v) {
        if (!destination.cd(pathlist.at(v))) {
            do_return = true;
        }
    }
    QString path = action->text();
    QMapIterator<QString, QString> iter(vars_map);
    while (iter.hasNext()) {
        iter.next();
        if (path.contains(iter.key())) {
            path.replace(iter.key(), iter.value());
        }
    }
    path.replace("//", "/");
    QDir syncfolder(path);
    if (!syncfolder.exists()) {
        parent_item->setText(1, tr("NOT FOUND"));
        parent_item->setForeground(1, QBrush(Qt::red));
        return;
    }
    if (do_return) return;
    sync_folder_1 = syncfolder.path();
    sync_folder_2 = destination.path();
    folders_set << sync_folder_1;
    folders_set << sync_folder_2;
    parent_item->setData(1, Qt::UserRole, QVariant(sync_folder_1));
    parent_item->setData(2, Qt::UserRole, QVariant(sync_folder_2));

    dir_filters = QDir::NoDotAndDotDot | QDir::Files;
    if (sync_hidden->isChecked()) { dir_filters |= QDir::Hidden; }
    if (!sync_nosubdirs->isChecked()) { dir_filters |= QDir::AllDirs; }

    if (text_database) {
        if (propagate_deletions->isChecked() || alert_collisions->isChecked()) {
            folder_prop_list_map.clear();
            QString sync_folder; //QStringList prop_files_list;
            for (int i = 0; i < 2; ++i) {
                if (i == 0) sync_folder = syncfolder.absolutePath();
                else sync_folder = destination.absolutePath();
                //prop_files_list = getFolderDatabase(sync_folder);
                folder_prop_list_map.insert(sync_folder, getFolderDatabase(sync_folder));
            }
        }
    }
    update_time = (QDateTime::currentDateTime()).toString("yyyy.MM.dd-hh.mm.ss");
    syncing = true; synced_files = 0;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    subAnalyse(folders_set, parent_item);
    parent_item->setText(0, tr("Root directory") + " - " + tr("%1 files and folders need to be synchronised").arg(synced_files));
    QApplication::restoreOverrideCursor();
    syncing = false;
}

void SyncPage::leaveAnalyse()
{
    sync_log_label->setText(tr("<b>Sync log:</b>"));
    logs_stw->setCurrentIndex(0);
}

void SyncPage::analyseFolders()
{
    if (syncing) return;

    extensions.clear();
    QList<QAction *> actions = filters_menu->actions();
    for (int f = 0; f < actions.count(); ++f) {
        if (actions.at(f)->isChecked()) {
            for (int l = 0; l < mp_parent->filter_list->count(); ++l) {
                if (mp_parent->filter_list->item(l)->text() == actions.at(f)->text()) {
                    for (int e = 0; e < ((Filter *)mp_parent->filter_list->item(l))->extensions.count(); ++e) {
                        extensions << QString("*%1").arg(((Filter *)mp_parent->filter_list->item(l))->extensions.at(e));
                    } break;
                }
            }
        }
    }
    dir_filters = QDir::NoDotAndDotDot | QDir::Files;
    if (sync_hidden->isChecked()) { dir_filters |= QDir::Hidden; }
    if (!sync_nosubdirs->isChecked()) { dir_filters |= QDir::AllDirs; }
    if (text_database) {
        if (propagate_deletions->isChecked() || alert_collisions->isChecked()) {
            folder_prop_list_map.clear(); //QStringList prop_files_list;
            for (int i = 0; i < sync_folders->count(); ++i) {
                //prop_files_list = getFolderDatabase(sync_folders->at(i)->path());
                folder_prop_list_map.insert(sync_folders->at(i)->path(), getFolderDatabase(sync_folders->at(i)->path()));
            }
        }
    }
    syncing = true;
    synced_files = 0;

    if (sort_analysis_by_action->isChecked()) {
        analyseSeparately();
        return;
    }
    QTreeWidgetItem * parent_item = new QTreeWidgetItem (analyse_tree);
    parent_item->setText(0, tr("Root directory"));
    QStringList data0; data0 << ""; data0 << "checked";
    parent_item->setData(0, Qt::UserRole, QVariant(data0));
    MTStringSet folders_set; QDir dir;
    for (int i = 0; i < sync_folders->count(); ++i) {
        dir.setPath(sync_folders->at(i)->path());
        if (!dir.exists()) {
            parent_item->setText(i+1, tr("NOT FOUND"));
            parent_item->setForeground(i+1, QBrush(Qt::red));
        } else folders_set << dir.path();
        parent_item->setData(i+1, Qt::UserRole, QVariant(sync_folders->at(i)->path()));
    }
    parent_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16.png"));
    parent_item->setCheckState(0, Qt::Checked);
    if (ignore_blacklist->isChecked()) parent_item->setDisabled(true);
    parent_item->setExpanded(true);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    subAnalyse(folders_set, parent_item);
    //subCheckExpanded(parent_item);
    parent_item->setText(0, tr("Root directory") + " - " + tr("%1 files and folders need to be synchronised").arg(synced_files));
    QApplication::restoreOverrideCursor();

    synced_files = 0;
    syncing = false;
}

bool AbstractSyncPage::subAnalyse(const MTStringSet & folders_set, QTreeWidgetItem * parent_item/*, int num*/)
{
    MTStringSet file_names;
    QStringList entries;
    QDir dir;
    for (int i = 0; i < folders_set.count(); ++i) {
        dir = folders_set.at(i);
        if (!dir.exists()) continue;
        if (extensions.count()==0) {
            entries = dir.entryList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
        } else {
            entries = dir.entryList(extensions.toList(), dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
        }
        for (int n = 0; n < entries.count(); ++n) {
            //if (!entries.at(n).absoluteFilePath().startsWith(sync_folders_set.at(i))) continue;
            file_names << entries.at(n);
        }
    }

    bool special = false;
    QTreeWidgetItem * child_item = 0;
    MTFileInfo * file_info = new MTFileInfo;
    QStringList s_folders_list = currentSyncFoldersList();
    for (int n = 0; n < file_names.count(); ++n) {
        if (file_names.at(n).startsWith(".synkron.sync")) continue;
        int sub_special = 0;
        QString rel_path = QString("%1/%2").arg(parent_item->data(0, Qt::UserRole).toStringList().at(0)).arg(file_names.at(n));
        if (rel_path.isEmpty()) continue;
        if (rel_path.startsWith("/")) rel_path.remove(0, 1);
        bool blacklisted = parent_item->checkState(0) == Qt::Unchecked;
        if (!ignore_blacklist->isChecked()) {
            for (int e = 0; e < exts_blacklist.count(); ++e) {
                if (rel_path.endsWith(exts_blacklist.at(e))) {
                    blacklisted = true;
                    break;
                }
            }
        }
        MTStringSet child_folders_set;
        child_item = new QTreeWidgetItem (parent_item);
        MTEvenDateTime newest_datetime;
        QList<int> newest_indices;
        QList<int> conflicted_indices;
        QList<int> unexist_conflicted_indices;
        for (int i = 0; i < s_folders_list.count(); ++i) {
            //file_info->setFile(QString("%1%2").arg(s_folders_list.at(i)).arg(rel_path));
            file_info->setFile(QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
            if (!file_info->exists()) {
                child_item->setData(i+1, Qt::UserRole, QVariant(file_info->absoluteFilePath()));
                sub_special++;
                if (propagate_deletions->isChecked()) {
                    if (isInGroupDatabase(file_info->absoluteFilePath())) {
                        child_item->setText(i+1, tr("DELETED"));
                        child_item->setForeground(i+1, QBrush(Qt::darkMagenta));
                        continue;
                    }
                }
                child_item->setText(i+1, tr("NOT FOUND"));
                child_item->setForeground(i+1, QBrush(Qt::red));
                continue;
            }
            if (file_info->isDir() && !file_info->isSymLink()) {
                if (!ignore_blacklist->isChecked()) {
                    /*if (folders_blacklist.contains(file_info->absoluteFilePath(), Qt::CaseInsensitive)) {
                        blacklisted = true;
                    }*/
                    for (int b = 0; b < folders_blacklist.count(); ++b) {
                        if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                            blacklisted = true;
                        }
                    }
                }
                child_folders_set << file_info->absoluteFilePath();
            } else {
                if (!ignore_blacklist->isChecked()) {
                    /*if (files_blacklist.contains(file_info->absoluteFilePath(), Qt::CaseInsensitive)) {
                        blacklisted = true;
                    }*/
                    for (int b = 0; b < files_blacklist.count(); ++b) {
                        if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                            blacklisted = true;
                        }
                    }
                }
                child_item->setText(i+1, tr("OBSOLETE"));
                child_item->setForeground(i+1, QBrush(Qt::darkRed));
                int compared_dates = file_info->lastModified().compareWith(newest_datetime, allowed_difference, allow_DST->isChecked());
                if (compared_dates == 0) { // has the same date as the newest_datetime
                    newest_indices << i+1;
                } else if (compared_dates > 0) { // is newer than the newest_datetime
                    newest_datetime = file_info->lastModified();
                    newest_indices.clear();
                    newest_indices << i+1;
                }
                if (compared_dates != 0 && alert_collisions->isChecked()) {
                    int is_diff = fileIsDifferentFromDB(file_info->absoluteFilePath());
                    if (is_diff == 1) {
                        conflicted_indices << i+1;
                    } else if (is_diff == -1)  {
                        unexist_conflicted_indices << i+1;
                    }
                }
                child_item->setToolTip(i+1, file_info->lastModified().toString(Qt::ISODate));
            }
            child_item->setData(i+1, Qt::UserRole, QVariant(file_info->absoluteFilePath()));

            if (child_item->text(0).isEmpty()) {
                if (file_info->isDir() && !file_info->isSymLink()) {
                    child_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16.png"));
                } else {
                    child_item->setIcon(0, QIcon(":/new/prefix1/images/file.png"));
                }
                child_item->setText(0, file_info->fileName());
            }
        }
        if (newest_indices.count() != 0) { //If is not dir ---------------------
            if (!blacklisted && (newest_indices.count() != s_folders_list.count())) {
                sub_special++;
                child_item->setIcon(0, QIcon(":/new/prefix1/images/file_red.png"));
                synced_files += s_folders_list.count() - newest_indices.count();
            } else {
                if (analyse_special_only->isChecked()) {
                    delete child_item;
                    continue;
                }
            }
            for (int ind = 0; ind < newest_indices.count(); ++ind) {
                child_item->setForeground(newest_indices.at(ind), QBrush(Qt::darkGreen));
                child_item->setText(newest_indices.at(ind), tr("OK"));
            }
        } else if (sub_special && !blacklisted) {
            child_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16_red.png"));
            synced_files += sub_special;
        }
        if (conflicted_indices.count() > 1) {
            for (int ind = 0; ind < conflicted_indices.count(); ++ind) {
                child_item->setText(conflicted_indices.at(ind), tr("COLLISION"));
                child_item->setForeground(conflicted_indices.at(ind), QBrush(Qt::black));
            }
        }
        if (unexist_conflicted_indices.count() > 1) {
            for (int ind = 0; ind < unexist_conflicted_indices.count(); ++ind) {
                child_item->setText(unexist_conflicted_indices.at(ind), tr("COLLISION"));
                child_item->setForeground(unexist_conflicted_indices.at(ind), QBrush(Qt::black));
            }
        }
        child_item->setCheckState(0, blacklisted ? Qt::Unchecked : Qt::Checked);
        if (parent_item->checkState(0) == Qt::Unchecked || ignore_blacklist->isChecked()) child_item->setDisabled(true);
        QStringList data0;
        data0 << rel_path;
        if (child_item->checkState(0) == Qt::Checked) data0 << "checked";
        else data0 << "unchecked";
        child_item->setData(0, Qt::UserRole, QVariant(data0));
        if (blacklisted) sub_special = 0;
        //parent_item->addChild(child_item);
        if (child_folders_set.count() != 0) {
            if (fast_analyse->isChecked()) {
                QTreeWidgetItem * loading_item = new QTreeWidgetItem;
                loading_item->setText(0, tr("Loading..."));
                QStringList ld_data;
                ld_data << rel_path;
                ld_data << "unchecked";
                ld_data << "regular";
                loading_item->setData(0, Qt::UserRole, QVariant(ld_data));
                child_item->addChild(loading_item);
                sub_special = 0;
            } else {
                if (subAnalyse(child_folders_set, child_item)) sub_special = 1;
                else if (analyse_special_only->isChecked() && child_item->childCount() == 0 && !sub_special) {
                    delete child_item;
                    continue;
                }
            }
        }
        if (blacklisted) sub_special = 0;
        if (sub_special) {
            parent_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16_red.png"));
            //parent_item->setExpanded(true);
            special = true;
        }
        //child_item->setData(0, Qt::UserRole, QVariant(data0));
    }
    delete file_info;
    return special;
}

void SyncPage::analyseSeparately()
{
    files_to_be_copied.clear();
    collided_files.clear();
    skipped_files.clear();
    skipped_folders.clear();
    files_to_be_deleted.clear();
    folders_to_be_created.clear();
    folders_to_be_deleted.clear();
    files_to_be_overwritten.clear();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString rel_path;
    subSortedAnalyse(rel_path);
    if (files_to_be_copied.count())
        createChildItemsByFolder(tr("Files to copy (%1)").arg(files_to_be_copied.count()), files_to_be_copied.keys(), files_to_be_copied.values(), false, true);
    if (files_to_be_overwritten.count())
        createChildItemsByFolder(tr("Files to update (%1)").arg(files_to_be_overwritten.count()), files_to_be_overwritten.keys(), files_to_be_overwritten.values(), false, true);
    if (collided_files.count())
        createChildItemsByFolder(tr("Collided files (%1)").arg(collided_files.count()), collided_files, QStringList(), false, false);
    if (files_to_be_deleted.count())
        createChildItemsByFolder(tr("Files to delete (%1)").arg(files_to_be_deleted.count()), files_to_be_deleted, QStringList(), false, true);
    if (folders_to_be_created.count())
        createChildItemsByFolder(tr("Folders to create (%1)").arg(folders_to_be_created.count()), folders_to_be_created, QStringList(), true, true);
    if (folders_to_be_deleted.count())
        createChildItemsByFolder(tr("Folders to delete (%1)").arg(folders_to_be_deleted.count()), folders_to_be_deleted, QStringList(), true, true);
    if (!files_to_be_copied.count() && !files_to_be_overwritten.count() && !collided_files.count() && !files_to_be_deleted.count() && !folders_to_be_created.count() && !folders_to_be_deleted.count()) {
        createChildItemsByFolder(tr("There is nothing to be done"), QStringList(), QStringList(), false, false);
    }
    if (skipped_files.count())
        createChildItemsByFolder(tr("Skipped files (%1)").arg(skipped_files.count()), skipped_files, QStringList(), false, false, true);
    if (skipped_folders.count())
        createChildItemsByFolder(tr("Skipped folders (%1)").arg(skipped_folders.count()), skipped_folders, QStringList(), true, false, true);
    QApplication::restoreOverrideCursor();

    synced_files = 0;
    syncing = false;
}

void SyncPage::createChildItemsByFolder(QString name, QStringList keys, QStringList values, bool folder, bool sort, bool blacklisted)
{
    QTreeWidgetItem * parent_item = new QTreeWidgetItem (analyse_tree);
    parent_item->setText(0, name);
    QStringList data0; data0.clear(); data0 << "[container]"; data0 << "checked";
    parent_item->setData(0, Qt::UserRole, QVariant(data0));
    if (blacklisted) parent_item->setIcon(0, QIcon(":/new/prefix1/images/blacklist.png"));
    else if (folder) parent_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16.png"));
    else parent_item->setIcon(0, QIcon(":/new/prefix1/images/file.png"));
    if (ignore_blacklist->isChecked()) parent_item->setDisabled(true);

    QMap<QString, QTreeWidgetItem *> string_item_map;
    for (int i = 0; i < keys.count(); ++i) {
        if (sort) {
            int from = -1;
            int to = -1;
            for (int n = 0; n < sync_folders->count(); ++n) {
                if (keys.at(i).startsWith(sync_folders->at(n)->path())) {
                    from = n;
                } else if (values.count() && values.at(i).startsWith(sync_folders->at(n)->path())) {
                    to = n;
                }
            }
            QTreeWidgetItem * subparent_item = 0;
            if (from < 0) {
                createChildTreeItem(parent_item, keys.at(i), folder, false);
                continue;
            } else if (string_item_map.contains(QString("%1to%2").arg(from).arg(to))) {
                subparent_item = string_item_map.value(QString("%1to%2").arg(from).arg(to));
            } else {
                subparent_item = new QTreeWidgetItem(parent_item);
                if (values.count()) {
                    subparent_item->setText(0, tr("From %1 to %2").arg(sync_folders->at(from)->label()).arg(sync_folders->at(to)->label()));
                } else {
                    subparent_item->setText(0, sync_folders->at(from)->label());
                }
                subparent_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16.png"));
                subparent_item->setData(0, Qt::UserRole, QVariant(QStringList("[container]")));
                string_item_map[QString("%1to%2").arg(from).arg(to)] = subparent_item;
            }
            if (!subparent_item) continue;
            QString item_name = keys.at(i);
            createChildTreeItem(subparent_item, QDir(sync_folders->at(from)->path()).relativeFilePath(item_name), folder, blacklisted);
        } else {
            createChildTreeItem(parent_item, keys.at(i), folder, blacklisted);
        }
    }
}

void SyncPage::subSortedAnalyse(const QString & rel_path)
{
    MTStringSet file_names;
    QStringList entries;
    QDir dir;
    MTMap<QString, int> folders_set;
    for (int i = 0; i < sync_folders->count(); ++i) {
        folders_set.setValue(QDir(sync_folders->at(i)->path()).absoluteFilePath(rel_path), i);
    }
    for (int i = 0; i < folders_set.count(); ++i) {
        dir = folders_set.key(i);
        if (!dir.exists()) continue;
        if (extensions.count()==0) {
            entries = dir.entryList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
        } else {
            entries = dir.entryList(extensions.toList(), dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
        }
        for (int n = 0; n < entries.count(); ++n) {
            file_names << entries.at(n);
        }
    }

    MTFileInfo * file_info = new MTFileInfo;
    for (int n = 0; n < file_names.count(); ++n) {
        if (file_names.at(n).startsWith(".synkron.sync")) continue;
        QString sub_rel_path = QString("%1/%2").arg(rel_path).arg(file_names.at(n));
        if (sub_rel_path.isEmpty()) continue;
        if (sub_rel_path.startsWith("/")) sub_rel_path.remove(0, 1);
        bool blacklisted = false;
        if (!ignore_blacklist->isChecked()) {
            for (int e = 0; e < exts_blacklist.count(); ++e) {
                if (sub_rel_path.endsWith(exts_blacklist.at(e))) {
                    blacklisted = true;
                    break;
                }
            }
        }
        MTEvenDateTime newest_datetime;
        QList<int> newest_indices;
        MTMap<MTEvenDateTime, int> newest_slave_dates;
        QList<int> conflicted_indices;
        QList<int> unexist_conflicted_indices;
        QList<int> unexisting_indices;
        bool deleted = false;
        bool has_master_dirs = false;
        for (int i = 0; i < folders_set.count(); ++i) {
            file_info->setFile(QDir(folders_set.key(i)).absoluteFilePath(file_names.at(n)));
            //addTableItem( "", QString::fromUtf8(":/new/prefix1/images/file.png"));
            if (!file_info->exists()) {
                if (propagate_deletions->isChecked() && !isSlave(i)) {
                    if (isInGroupDatabase(file_info->absoluteFilePath())) {
                        deleted = true;
                    }
                }
                unexisting_indices << i;
                continue;
            }
            if (file_info->isDir() && !file_info->isSymLink()) {
                if (!isSlave(i)) has_master_dirs = true;
                if (!ignore_blacklist->isChecked()) {
                    for (int b = 0; b < folders_blacklist.count(); ++b) {
                        if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                            blacklisted = true;
                        }
                    }
                }
            } else {
                if (!ignore_blacklist->isChecked()) {
                    for (int b = 0; b < files_blacklist.count(); ++b) {
                        if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                            blacklisted = true;
                        }
                    }
                }
                int compared_dates = file_info->lastModified().compareWith(newest_datetime, allowed_difference, allow_DST->isChecked());
                if (compared_dates == 0) { // has the same date as the newest_datetime
                    newest_indices << i;
                } else if (compared_dates > 0) { // is newer than the newest_datetime
                    if (isSlave(i)) {
                        newest_slave_dates.setValue(file_info->lastModified(), i);
                    } else {
                        newest_datetime = file_info->lastModified();
                        newest_indices.clear();
                        newest_indices << i;
                    }
                }
                if (compared_dates != 0 && alert_collisions->isChecked()) {
                    int is_diff = fileIsDifferentFromDB(file_info->absoluteFilePath());
                    if (is_diff == 1) {
                        conflicted_indices << i;
                    } else if (is_diff == -1)  {
                        unexist_conflicted_indices << i;
                    }
                }
            }
        }
        if (newest_indices.count() || newest_slave_dates.count()) { // is file ---------------------
            if (!newest_indices.count()) deleted = true;
            if (blacklisted) {
                skipped_files << sub_rel_path;
            } else if (deleted) {
                for (int i = 0; i < folders_set.count(); ++i) {
                    if (unexisting_indices.contains(i)) continue;
                    if (dontModify(i) || updateOnly(i)) continue;
                    files_to_be_deleted << QDir(folders_set.key(i)).absoluteFilePath(file_names.at(n));
                }
            } else if (conflicted_indices.count() > 1 || unexist_conflicted_indices.count() > 1) {
                collided_files << sub_rel_path;
            } else if (newest_indices.count() != sync_folders->count()) {
                for (int s = newest_slave_dates.count() - 1; s >= 0; --s) {
                    if (newest_slave_dates.key(s).compareWith(newest_datetime, allowed_difference, allow_DST->isChecked()) != 0) {
                        newest_slave_dates.removeAt(s);
                    }
                }
                for (int i = 0; i < folders_set.count(); ++i) {
                    if (newest_indices.contains(i)) continue;
                    if (newest_slave_dates.values().contains(i)) continue;
                    if (dontModify(i)) continue;
                    if (unexisting_indices.contains(i)) {
                        if (updateOnly(i)) continue;
                        files_to_be_copied.insert(QDir(folders_set.key(newest_indices.first())).absoluteFilePath(file_names.at(n)), QDir(folders_set.key(i)).absoluteFilePath(file_names.at(n)));
                    } else {
                        files_to_be_overwritten.insert(QDir(folders_set.key(newest_indices.first())).absoluteFilePath(file_names.at(n)), QDir(folders_set.key(i)).absoluteFilePath(file_names.at(n)));
                    }
                }
            }
        } else { // is dir -------------------------------------------------------
            if (!has_master_dirs) deleted = true;
            if (blacklisted) {
                skipped_folders << sub_rel_path;
            } else if (deleted) {
                for (int i = 0; i < folders_set.count(); ++i) {
                    if (unexisting_indices.contains(i)) continue;
                    if (dontModify(i) || updateOnly(i)) continue;
                    folders_to_be_deleted << QString("%1/%2").arg(folders_set.key(i)).arg(file_names.at(n));
                }
            } else {
                for (int i = 0; i < unexisting_indices.count(); ++i) {
                    if (dontModify(unexisting_indices.at(i)) || updateOnly(unexisting_indices.at(i))) continue;
                    folders_to_be_created << QString("%1/%2").arg(folders_set.key(unexisting_indices.at(i))).arg(file_names.at(n));
                }
                subSortedAnalyse(sub_rel_path);
            }
        }
    }
    delete file_info;
}

void SyncPage::createChildTreeItem(QTreeWidgetItem * parent_item, QString text, bool folder, bool blacklisted)
{
    QTreeWidgetItem * child_item = new QTreeWidgetItem(parent_item);
    child_item->setText(0, text);
    QStringList data0;
    if (blacklisted) {
        data0 << text; data0 << "unchecked";
        child_item->setCheckState(0, Qt::Unchecked);
    } else {
        data0 << text; data0 << "checked";
        child_item->setCheckState(0, Qt::Checked);
    }
    child_item->setData(0, Qt::UserRole, QVariant(data0));
    /*MTStringSet folders_set; QDir dir;
    for (int i = 0; i < sync_folders->count(); ++i) {
        dir.setPath(sync_folders->at(i)->path());
        if (!dir.exists()) {
            parent_item->setText(i+1, tr("NOT FOUND"));
            parent_item->setForeground(i+1, QBrush(Qt::red));
        } else folders_set << dir.path();
        parent_item->setData(i+1, Qt::UserRole, QVariant(sync_folders->at(i)->path()));
    }*/
    if (folder) {
        child_item->setIcon(0, QIcon(":/new/prefix1/images/folder_16.png"));
    } else {
        child_item->setIcon(0, QIcon(":/new/prefix1/images/file.png"));
    }
    if (ignore_blacklist->isChecked()) child_item->setDisabled(true);
}

/*void SyncPage::subCheckExpanded(QTreeWidgetItem * parent_item)
{
    for (int i = 0; i < parent_item->childCount(); ++i) {
        if (parent_item->child(i)->childCount() != 0) subCheckExpanded(parent_item->child(i));
        if (parent_item->child(i)->data(0, Qt::UserRole).toStringList().at(2) == "special") parent_item->setExpanded(true);
    }
}*/

void AbstractSyncPage::analyseTreeItemExpanded(QTreeWidgetItem * item)
{
    if (fast_analyse->isChecked() && item->isExpanded() && item->childCount() == 1) {
        if (item->child(0)->text(0) == tr("Loading...")) {
            MTStringSet folders_set;
            QString rel_path = item->data(0, Qt::UserRole).toStringList().at(0);
            MTFileInfo * file_info = new MTFileInfo;
            QStringList s_folders_list = currentSyncFoldersList();
            for (int i = 0; i < s_folders_list.count(); ++i) {
                file_info->setFile(QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
                if (file_info->exists()) {
                    if (file_info->isDir() && !file_info->isSymLink()) {
                        folders_set << file_info->absoluteFilePath();
                    }
                }
            }
            release(file_info);
            if (folders_set.count() != 0) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                subAnalyse(folders_set, item);
                QApplication::restoreOverrideCursor();
            }
            delete item->child(0);
        }
    }
}

void AbstractSyncPage::analyseTreeItemClicked(QTreeWidgetItem * item, int column)
{
    if (analyseTree()->currentItem()->data(0, Qt::UserRole).toStringList().first() == "[container]") return;
    if (column != 0) return;
    if (ignore_blacklist->isChecked()) return;
    if (item->isDisabled()) return;
    bool was_checked = item->data(0, Qt::UserRole).toStringList().at(1) == "checked";
    bool is_checked = item->checkState(0) == Qt::Checked;
    if (was_checked == is_checked) return;
    QStringList data0;
    data0 << item->data(0, Qt::UserRole).toStringList().at(0);
    //data0 <<¬ (is_checked ? "checked" : "unchecked");
    if (is_checked) data0 << "checked";
    else data0 << "unchecked";
    item->setData(0, Qt::UserRole, QVariant(data0));
    QString rel_path = data0.at(0);
    QFileInfo * file_info = 0;
    QStringList s_folders_list = currentSyncFoldersList();
    if (is_checked) {
        for (int e = 0; e < exts_blacklist.count(); ++e) {
            if (rel_path.endsWith(exts_blacklist.at(e))) {
                removeExtFromBlacklist(exts_blacklist.at(e));
            }
        }
        for (int i = 0; i < s_folders_list.count(); ++i) {
            release(file_info);
            //file_info = new QFileInfo (item->data(i+1, Qt::UserRole).toString());
            file_info = new QFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
            if (!file_info->exists()) continue;
            if (file_info->isDir() && !file_info->isSymLink()) {
                /*if (folders_blacklist.contains(file_info->absoluteFilePath())) {
                    removeFolderFromBlacklist(file_info->absoluteFilePath());
                }*/
                for (int b = 0; b < folders_blacklist.count(); ++b) {
                    if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                        removeFolderFromBlacklist(folders_blacklist.at(b));
                    }
                }
            } else {
                /*if (files_blacklist.contains(file_info->absoluteFilePath())) {
                    removeFileFromBlacklist(file_info->absoluteFilePath());
                }*/
                for (int b = 0; b < files_blacklist.count(); ++b) {
                    if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                        removeFileFromBlacklist(files_blacklist.at(b));
                    }
                }
            }
            release(file_info);
        }
    } else {
        for (int i = 0; i < s_folders_list.count(); ++i) {
            release(file_info);
            //file_info = new QFileInfo (item->data(i+1, Qt::UserRole).toString());
            file_info = new QFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
            if (!file_info->exists()) continue;
            if (file_info->isDir() && !file_info->isSymLink()) {
                addFolderToBlacklist(file_info->absoluteFilePath());
            } else {
                addFileToBlacklist(file_info->absoluteFilePath());
            }
            break;
        }
        release(file_info);
    }
    checkAnalyseTree();
}

void AbstractSyncPage::checkAnalyseTree()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    for (int i = 0; i < analyseTree()->topLevelItemCount(); ++i) {
        subCheckAnalyseTree(analyseTree()->topLevelItem(i));
    }
    QApplication::restoreOverrideCursor();
}

void AbstractSyncPage::subCheckAnalyseTree(QTreeWidgetItem * parent_item)
{
    QTreeWidgetItem * child_item;
    QFileInfo * file_info = 0;
    QStringList data0;
    QStringList s_folders_list = currentSyncFoldersList();
    for (int i = 0; i < parent_item->childCount(); ++i) {
        child_item = parent_item->child(i);
        bool blacklisted = false;
        data0 = child_item->data(0, Qt::UserRole).toStringList();
        QString rel_path = data0.at(0);
        if (rel_path == "[container]") {
            subCheckAnalyseTree(child_item);
            continue;
        }
        for (int e = 0; e < exts_blacklist.count(); ++e) {
            if (rel_path.endsWith(exts_blacklist.at(e))) {
                blacklisted = true;
                break;
            }
        }
        if (!blacklisted) {
            for (int n = 0; n < s_folders_list.count(); ++n) {
                release(file_info);
                file_info = new QFileInfo (QDir(s_folders_list.at(n)).absoluteFilePath(rel_path));
                //file_info = new QFileInfo (child_item->data(n+1, Qt::UserRole).toString());
                if (!file_info->exists()) continue;
                if (file_info->isDir() && !file_info->isSymLink()) {
                    /*if (folders_blacklist.contains(file_info->absoluteFilePath())) {
                        blacklisted = true;
                    }*/
                    for (int b = 0; b < folders_blacklist.count(); ++b) {
                        if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                            blacklisted = true;
                        }
                    }
                } else {
                    /*if (files_blacklist.contains(file_info->absoluteFilePath())) {
                        blacklisted = true;
                    }*/
                    for (int b = 0; b < files_blacklist.count(); ++b) {
                        if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info->absoluteFilePath())) {
                            blacklisted = true;
                        }
                    }
                }
                if (blacklisted) break;
            }
            release(file_info);
        }
        if (parent_item->checkState(0) == Qt::Unchecked || ignore_blacklist->isChecked()) {
            if (parent_item->data(0, Qt::UserRole).toStringList().first() != "[container]") {
                blacklisted = true;
                child_item->setDisabled(true);
            }
        } else {
            child_item->setDisabled(false);
        }
        if (ignore_blacklist->isChecked()) {
            blacklisted = false;
        }
        child_item->setCheckState(0, blacklisted ? Qt::Unchecked : Qt::Checked);
        data0.replace(1, blacklisted ? QString("unchecked") : QString("checked"));
        child_item->setData(0, Qt::UserRole, data0);
        if (child_item->childCount() != 0) subCheckAnalyseTree(child_item);
    }
}

void AbstractSyncPage::searchAnalyseTree()
{
    if (logsStw()->currentIndex() != 1) return;
    subSearchAnalyseTree(searchLeText(), analyseTree()->topLevelItem(0));
}

bool AbstractSyncPage::subSearchAnalyseTree(QString text, QTreeWidgetItem * parent_item)
{
    QTreeWidgetItem * child_item;
    bool parent_found = false;
    for (int i = 0; i < parent_item->childCount(); ++i) {
        child_item = parent_item->child(i);
        bool child_found = false;
        if (child_item->childCount() > 0) {
            child_found = subSearchAnalyseTree(text, child_item);
            if (child_found) parent_found = true;
            child_item->setExpanded(child_found);
        }
        if (!child_found && child_item->text(0).contains(text, Qt::CaseInsensitive)) {
            parent_found = true;
        }
    }
    parent_item->setExpanded(parent_found);
    return parent_found;
}

void AbstractSyncPage::analyseTreeItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    if (column == 0) return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(item->data(column, Qt::UserRole).toString()));
}

void AbstractSyncPage::analyseTreeConMenu(QPoint pos)
{
    if (analyseTree()->currentItem()->data(0, Qt::UserRole).toStringList().first() == "[container]") return;
    QMenu * contextMenu = new QMenu(this);
    QAction * open_analyse_action;
    if (analyseTree()->currentColumn() != 0) {
        open_analyse_action = new QAction (tr("Open"), this);
        connect(open_analyse_action, SIGNAL(triggered()), this, SLOT(openAnalyseTreeItem()));
        contextMenu->addAction(open_analyse_action);
        contextMenu->addSeparator();
    }

    if (!is_multisync) {
        QAction * sync_analyse_action;
        if (analyseTree()->currentItem()->checkState(0) == Qt::Checked) {
            sync_analyse_action = new QAction (tr("Sync"), this);
            connect(sync_analyse_action, SIGNAL(triggered()), this, SLOT(syncCurrentAnalyseItem()));
            contextMenu->addAction(sync_analyse_action);
        }
    }

    QAction * del_analyse_action = new QAction (tr("Delete"), this);
    connect(del_analyse_action, SIGNAL(triggered()), this, SLOT(deleteCurrentAnalyseItem()));
    contextMenu->addAction(del_analyse_action);

    QAction * rename_analyse_action = new QAction (tr("Rename"), this);
    connect(rename_analyse_action, SIGNAL(triggered()), this, SLOT(renameCurrentAnalyseItem()));
    contextMenu->addAction(rename_analyse_action);

    QAction * bl_analyse_action = new QAction (this);
    if (analyseTree()->currentItem()->checkState(0) == Qt::Checked) {
        bl_analyse_action->setText(tr("Add to blacklist"));
    } else {
        bl_analyse_action->setText(tr("Remove from blacklist"));
    }
    connect(bl_analyse_action, SIGNAL(triggered()), this, SLOT(blacklistCurrentAnalyseItem()));
    contextMenu->addAction(bl_analyse_action);

    if (analyseTree()->currentItem()->checkState(0) == Qt::Checked) {
        QAction * bl_ext_analyse_action = new QAction (tr("Add extension to blacklist"), this);
        connect(bl_ext_analyse_action, SIGNAL(triggered()), this, SLOT(blExtCurrentAnalyseItem()));
        contextMenu->addAction(bl_ext_analyse_action);
    }

    contextMenu->move(pos);
    contextMenu->show();
}

void AbstractSyncPage::openAnalyseTreeItem()
{
    if (analyseTree()->currentColumn() == 0) return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(analyseTree()->currentItem()
        ->data(analyseTree()->currentColumn(), Qt::UserRole).toString()));
}

void SyncPage::syncCurrentAnalyseItem()
{
    QTreeWidgetItem * item = analyse_tree->currentItem();
    MTMap<QString, int> folders;
    MTStringSet rel_paths;
    MTFileInfo * file_info = 0;
    leaveAnalyse();
    bool is_dir = false;
    QString rel_path = item->data(0, Qt::UserRole).toStringList().first();
    QStringList s_folders_list = currentSyncFoldersList();
    for (int i = 0; i < s_folders_list.count(); ++i) {
        release(file_info);
        //file_info = new MTFileInfo (item->data(i, Qt::UserRole).toString());
        file_info = new MTFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
        if (file_info->exists()) {
            if (file_info->isDir() && !file_info->isSymLink()) {
                is_dir = true; break;
            }
        }
    }
    for (int i = 0; i < s_folders_list.count(); ++i) {
        release(file_info);
        //file_info = new MTFileInfo (item->data(i, Qt::UserRole).toString());
        file_info = new MTFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
        if (!file_info->exists()) {
            if (is_dir) {
                if (!QDir().mkpath(file_info->absoluteFilePath())) {
                    addTableItem(tr("%1 Failed to create directory %2").arg(QTime().currentTime().toString("hh:mm:ss")).arg(file_info->absoluteFilePath()), "", "", QBrush(Qt::red), QBrush(Qt::white));
                    continue;
                } else {
                    addTableItem(tr("%1 Directory %2 created").arg(QTime().currentTime().toString("hh:mm:ss")).arg(file_info->absoluteFilePath()), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
                }
            } else {
                QDir dir = file_info->dir();
                if (!dir.exists()) {
                    if (!QDir().mkpath(dir.absolutePath())) {
                        addTableItem(tr("%1 Failed to create directory %2").arg(QTime().currentTime().toString("hh:mm:ss")).arg(dir.absolutePath()), "", "", QBrush(Qt::red), QBrush(Qt::white));
                        continue;
                    } else {
                        addTableItem(tr("%1 Directory %2 created").arg(QTime().currentTime().toString("hh:mm:ss")).arg(dir.absolutePath()), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
                    }
                }
            }
        }
        if (is_dir) {
            //folders << file_info->absoluteFilePath();
            folders.setValue(file_info->absoluteFilePath(), i);
        } else {
            rel_paths << file_info->fileName();
            //folders << file_info->dir().path();
            folders.setValue(file_info->dir().path(), i);
        }
    }
    release(file_info);
    if (folders.count() != 0) {
        if (rel_paths.count() == 0) {
            sync(folders);
        } else {
            setSyncEnabled(false);
                subGroupSync(folders, rel_paths);
            setSyncEnabled(true);
            if (propagate_deletions->isChecked()) saveAllFolderDatabases();
            addTableItem(tr("%1	Synchronisation complete: %2 file(s) %3").arg(QTime().currentTime().toString("hh:mm:ss")).arg(synced_files).arg(tr("synchronised")), "", "", QBrush(Qt::green));
            mp_parent->showTrayMessage(tr("Synchronisation complete"), tr("%1 files %2").arg(synced_files).arg(tr("synchronised")));
            synced_files = 0;
        }
    }
}

void AbstractSyncPage::deleteCurrentAnalyseItem()
{
    QTreeWidgetItem * item = analyseTree()->currentItem();
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove \"%1\" from every synced location?").arg(item->text(0)));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QFileInfo * file_info = 0;
    bool backup = true;
    bool sub_backup = true;
    QStringList s_folders_list = currentSyncFoldersList();
    QString rel_path = item->data(0, Qt::UserRole).toStringList().first();

    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        leaveAnalyse();
        update_time = (QDateTime::currentDateTime()).toString("yyyy.MM.dd-hh.mm.ss");
        if (backupFolders()) backup = false;
        for (int i = 0; i < s_folders_list.count(); ++i) {
            sub_backup = backup;
            release(file_info);
            //file_info = new QFileInfo (item->data(i, Qt::UserRole).toString());
            file_info = new MTFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
            if (sub_backup && s_folders_list.count() < 3) {
                if (backupFolder(i)) {
                    sub_backup = false;
                }
            }
            if (!file_info->exists()) continue;
            if (file_info->isDir() && !file_info->isSymLink()) {
                syncing = true;
                backupAndRemoveDir(file_info->absoluteFilePath(), sub_backup);
                syncing = false;
            } else {
                backupAndRemoveFile(*file_info, sub_backup);
            }
        }
        release(file_info);
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

void AbstractSyncPage::renameCurrentAnalyseItem()
{
    QTreeWidgetItem * item = analyseTree()->currentItem();
    bool ok = false;
    QStringList s_folders_list = currentSyncFoldersList();
    QString rel_path = item->data(0, Qt::UserRole).toStringList().first();
    QFileInfo * file_info = 0;
    for (int i = 0; i < s_folders_list.count(); ++i) {
        //file_info = new QFileInfo (item->data(i+1, Qt::UserRole).toString());
        file_info = new MTFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));
        if (file_info->exists()) break;
        else release(file_info);
    }
    if (!file_info->exists()) return;
    QString new_name = QInputDialog::getText(this, tr("Synkron - Rename file"), tr("Type a new name for \"%1\":").arg(file_info->fileName()), QLineEdit::Normal, file_info->fileName(), &ok);
    if (!ok) return;
    leaveAnalyse();
    for (int i = 0; i < s_folders_list.count(); ++i) {
        release(file_info);
        //file_info = new QFileInfo (item->data(i, Qt::UserRole).toString());
        file_info = new MTFileInfo (QDir(s_folders_list.at(i)).absoluteFilePath(rel_path));

        if (!file_info->exists()) continue;
        if (file_info->isDir() && !file_info->isSymLink()) {
            QDir dir (file_info->absoluteFilePath());
            QString dirname = dir.dirName();
            dir.cdUp();
            if (!dir.rename(dirname, new_name)) {
                addTableItem(tr("Error renaming directory %1").arg(file_info->absoluteFilePath()), "", "", QBrush(Qt::red), QBrush(Qt::white));
            } else {
                addTableItem(tr("File %1 renamed").arg(file_info->absoluteFilePath()), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
            }
        } else {
            QFile file (file_info->absoluteFilePath());
            if (!file.rename(QString("%1/%2").arg(file_info->dir().path()).arg(new_name))) {
                addTableItem(tr("Error renaming file %1").arg(file_info->absoluteFilePath()), "", "", QBrush(Qt::red), QBrush(Qt::white));
            } else {
                addTableItem(tr("File %1 renamed").arg(file_info->absoluteFilePath()), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
            }
        }
    }
    release(file_info);
}

void AbstractSyncPage::blacklistCurrentAnalyseItem()
{
    analyseTree()->currentItem()->setCheckState(0, analyseTree()->currentItem()->checkState(0) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
    analyseTreeItemClicked(analyseTree()->currentItem(), 0);
}

void AbstractSyncPage::blExtCurrentAnalyseItem()
{
    addExtToBlacklist(analyseTree()->currentItem()->text(0).split(".").last().split(" ").last());
    checkAnalyseTree();
}
