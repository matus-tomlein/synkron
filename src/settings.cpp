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

void MainWindow::saveSettings()
{
    QString lang = sync_settings->value("lang", "English").toString();
    bool dont_ask_on_quit = sync_settings->value("dont_ask_on_quit").toBool();
    bool minimise_on_quit = sync_settings->value("minimise_on_quit").toBool();
    sync_settings->clear();
    sync_settings->setValue("lang", lang);
    sync_settings->setValue("dont_ask_on_quit", dont_ask_on_quit);
    sync_settings->setValue("minimise_on_quit", minimise_on_quit);
    QStringList tabs_list;
    SyncPage * page;
    for (int i = 0; i < tabWidget->count(); ++i) {
        tabs_list << tabWidget->tabText(i);
        page = tabs.value(tabWidget->widget(i));
        QStringList paths_list;
        QString path;
        for (int n = 0; n < page->sync_folders->count(); ++n) {
            path = page->sync_folders->at(n)->path();
            paths_list << path;
            sync_settings->setValue(QString("tab_%1_%2/folder_%3/label").arg(tabWidget->tabText(i)).arg(i).arg(path), page->sync_folders->at(n)->label());
            sync_settings->setValue(QString("tab_%1_%2/folder_%3/dont_update").arg(tabWidget->tabText(i)).arg(i).arg(path), page->sync_folders->at(n)->dont_update_act->isChecked() ? "checked" : "unchecked");
            sync_settings->setValue(QString("tab_%1_%2/folder_%3/update_only").arg(tabWidget->tabText(i)).arg(i).arg(path), page->sync_folders->at(n)->update_only_act->isChecked() ? "checked" : "unchecked");
            sync_settings->setValue(QString("tab_%1_%2/folder_%3/backup_folder").arg(tabWidget->tabText(i)).arg(i).arg(path), page->sync_folders->at(n)->backup_folder_act->isChecked() ? "checked" : "unchecked");
            sync_settings->setValue(QString("tab_%1_%2/folder_%3/no_empty_folders").arg(tabWidget->tabText(i)).arg(i).arg(path), page->sync_folders->at(n)->no_empty_folders_act->isChecked() ? "checked" : "unchecked");
            sync_settings->setValue(QString("tab_%1_%2/folder_%3/slave").arg(tabWidget->tabText(i)).arg(i).arg(path), page->sync_folders->at(n)->slave_act->isChecked() ? "checked" : "unchecked");
        }
        sync_settings->setValue(QString("tab_%1_%2/folders").arg(tabWidget->tabText(i)).arg(i), paths_list);
        sync_settings->setValue(QString("tab_%1_%2/last_sync").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->last_sync);
        sync_settings->setValue(QString("tab_%1_%2/allowed_difference").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->allowed_difference);
        sync_settings->setValue(QString("tab_%1_%2/sync_hidden").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->sync_hidden->isChecked() ? "checked" : "unchecked");
        //sync_settings->setValue(QString("tab_%1_%2/advanced").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->advanced->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/sync_nosubdirs").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->sync_nosubdirs->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/ignore_blacklist").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->ignore_blacklist->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/move").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->move->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/propagate_deletions").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->propagate_deletions->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/backup_folders").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->backup_folders->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/update_only").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->update_only->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/fast_analyse").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->fast_analyse->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/analyse_special_only").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->analyse_special_only->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/sort_analysis_by_action").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->sort_analysis_by_action->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/alert_collisions").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->alert_collisions->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/text_database").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->text_database_action->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/no_empty_folders").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->no_empty_folders->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("tab_%1_%2/allow_dst").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->allow_DST->isChecked() ? "checked" : "unchecked");
#ifndef Q_WS_WIN
        sync_settings->setValue(QString("tab_%1_%2/symlinks").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->symlinks->isChecked() ? "checked" : "unchecked");
#endif
        //sync_settings->setValue(QString("tab_%1_%2/filters_gb").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->filters->isChecked() ? "checked" : "unchecked");
        QStringList flist;
        QList<QAction *> actions = tabs.value(tabWidget->widget(i))->filters_menu->actions();
        for (int n = 0; n < actions.count(); ++n) {
            if (actions.at(n)->isChecked()) {
                flist << actions.at(n)->text();
            }
        }
        sync_settings->setValue(QString("tab_%1_%2/flist").arg(tabWidget->tabText(i)).arg(i), flist);
        sync_settings->setValue(QString("tab_%1_%2/files_blacklist").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->files_blacklist);
        sync_settings->setValue(QString("tab_%1_%2/folders_blacklist").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->folders_blacklist);
        sync_settings->setValue(QString("tab_%1_%2/exts_blacklist").arg(tabWidget->tabText(i)).arg(i), tabs.value(tabWidget->widget(i))->exts_blacklist);
    }
    sync_settings->setValue("tabs", tabs_list);
    QStringList filters;
    for (int i = 0; i < filter_list->count(); ++i) {
        filters << filter_list->item(i)->text();
        sync_settings->setValue(QString("filter_%1_%2/extensions").arg(filter_list->item(i)->text()).arg(i), ( static_cast<Filter *> (filter_list->item(i))->extensions));
    }
    sync_settings->setValue("filters", filters);
    QStringList multitabs_list; MultisyncPage * multi_page;
    for (int i = 0; i < multi_tabWidget->count(); ++i) {
        multi_page = static_cast<MultisyncPage *> (multi_tabWidget->widget(i));
        multitabs_list << multi_tabWidget->tabText(i);
        QStringList sources;
        for (int s = 0; s < multi_page->list_multi->count(); ++s) {
            sources << multi_page->list_multi->item(s)->text();
            if (multi_page->list_multi->item(s)->checkState() == Qt::Checked) sources << "checked";
            else sources << "unchecked";
        }
        sync_settings->setValue(QString("multitab_%1_%2/sources").arg(multi_tabWidget->tabText(i)).arg(i), sources);
        sync_settings->setValue(QString("multitab_%1_%2/destination").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->destination_multi->text());
        sync_settings->setValue(QString("multitab_%1_%2/last_sync").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->last_sync);
        sync_settings->setValue(QString("multitab_%1_%2/allowed_difference").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->allowed_difference);
        //sync_settings->setValue(QString("multitab_%1_%2/advanced").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->advanced->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/sync_hidden").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->sync_hidden->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/backup_folder_1").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->backup_folder_1->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/backup_folder_2").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->backup_folder_2->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/update_only_1").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->update_only_1->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/update_only_2").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->update_only_2->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/sync_nosubdirs").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->sync_nosubdirs->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/ignore_blacklist").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->ignore_blacklist->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/move").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->move->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/clone_folder1").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->clone_folder1->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/propagate_deletions").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->propagate_deletions->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/fast_analyse").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->fast_analyse->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/analyse_special_only").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->analyse_special_only->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/alert_collisions").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->alert_collisions->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/no_empty_folders").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->no_empty_folders->isChecked() ? "checked" : "unchecked");
        sync_settings->setValue(QString("multitab_%1_%2/allow_dst").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->allow_DST->isChecked() ? "checked" : "unchecked");
#ifndef Q_WS_WIN
        sync_settings->setValue(QString("multitab_%1_%2/symlinks").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->symlinks->isChecked() ? "checked" : "unchecked");
#endif
        sync_settings->setValue(QString("multitab_%1_%2/files_blacklist").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->files_blacklist);
        sync_settings->setValue(QString("multitab_%1_%2/folders_blacklist").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->folders_blacklist);
        sync_settings->setValue(QString("multitab_%1_%2/exts_blacklist").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->exts_blacklist);
        sync_settings->setValue(QString("multitab_%1_%2/vars_map").arg(multi_tabWidget->tabText(i)).arg(i), multi_page->variablesToString());
    }
    sync_settings->setValue("multitabs", multitabs_list);
    //sync_settings->setValue("synchronised", synchronised);
    sync_settings->setValue("restore_clean_selected", restore_clean_selected->isChecked());
    sync_settings->setValue("restore_clean_by_date", restore_clean_by_date->isChecked());
    sync_settings->setValue("restore_clean_repeated", restore_clean_repeated->isChecked());
    sync_settings->setValue("restore_clean_date", restore_clean_date->value());
    sync_settings->setValue("restore_clean_files", restore_clean_files->value());
    sync_settings->setValue("restore_clean_auto_gb", restore_clean_auto_gb->isChecked());
    sync_settings->setValue("restore_clean_auto_files", restore_clean_auto_files->value());
    sync_settings->setValue("files_blacklist", files_blacklist);
    sync_settings->setValue("folders_blacklist", folders_blacklist);
    sync_settings->setValue("exts_blacklist", exts_blacklist);
    sync_settings->setValue("run_hidden", run_hidden);
    sync_settings->setValue("current_synctab", tabWidget->currentIndex());
    sync_settings->setValue("current_multitab", multi_tabWidget->currentIndex());
    sync_settings->setValue("disable_tray_messages", actionDisable_tray_messages->isChecked());
    sync_settings->setValue("sync_at_launch", actionSync_at_launch->isChecked());
    sync_settings->setValue("temp_path", raw_temp_path);
#ifdef Q_WS_WIN
    sync_settings->setValue("shut_down_after_sync", actionShut_down_after_sync->isChecked());
#endif
    sync_settings->setValue("quit_after_sync", actionQuit_after_sync->isChecked());
    QStringList schedules; SyncSchedule * schedule;
    for (int i = 0; i < tw_schedules->rowCount(); ++i) {
        schedule = item_sched_map.value(tw_schedules->item(i, 0));
        schedules << tw_schedules->item(i, 0)->text();
        if (schedule->scheduling) schedules << "on";
        else schedules << "off";
        sync_settings->setValue(QString("sched_%1_%2_tabs").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_tab_list);
        sync_settings->setValue(QString("sched_%1_%2_multitabs").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_multitab_list);
        sync_settings->setValue(QString("sched_%1_%2_times").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_time_list);
        sync_settings->setValue(QString("sched_%1_%2_checkedtimes").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_checked_time_list);
        sync_settings->setValue(QString("sched_%1_%2_periodical_interval").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->periodical_interval);
        sync_settings->setValue(QString("sched_%1_%2_tab_index").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->timing_tab_index);
        sync_settings->setValue(QString("sched_%1_%2_dates").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_date_list);
        sync_settings->setValue(QString("sched_%1_%2_checkeddates").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_checked_date_list);
        sync_settings->setValue(QString("sched_%1_%2_uncheckeddays").arg(tw_schedules->item(i, 0)->text()).arg(i), schedule->sched_unchecked_days_list);
    }
    sync_settings->setValue("schedules", schedules);
    sync_settings->setValue("pos", this->pos());
    sync_settings->setValue("size", this->size());
    sync_settings->setValue("ver", QVariant(f_ver).toString());
#ifdef Q_WS_MAC
    sync_settings->setValue("macx_brushedmetalstyle", actionBrushedMetalStyle->isChecked());
#endif
}

void MainWindow::readSettings()
{
    QSettings settings ("Matus Tomlein", "Synkron");
#ifdef Q_WS_MAC
    actionBrushedMetalStyle->setChecked(sync_settings->value("macx_brushedmetalstyle", settings.value("macx_brushedmetalstyle", false)).toBool());
    if (actionBrushedMetalStyle->isChecked()) this->setAttribute(Qt::WA_MacBrushedMetal);
    this->setUnifiedTitleAndToolBarOnMac(!actionBrushedMetalStyle->isChecked());
#endif
    QStringList filters = sync_settings->value("filters").toStringList();
    for (int i = 0; i < filters.count(); ++i) {
        addFilter(filters.at(i), sync_settings->value(QString("filter_%1_%2/extensions").arg(filters.at(i)).arg(i)).toStringList());
    }
    SyncPage * page = nullptr;
    QStringList tabs_list = sync_settings->value("tabs").toStringList();
    if (tabs_list.count()==0) {
        QStringList recentsyncs = sync_settings->value("recentsyncs").toStringList();
        if (recentsyncs.count() != 0) {
            bool okey; recentsyncs.last().toInt(&okey);
            if ((recentsyncs.count()-1) % 6 == 5 && okey) {
                for (int i = 0; i < recentsyncs.count(); ++i) {
                    if (i % 6 == 0 || i == 0) { page = addSyncTab();
                        page->sync_folders->removeAllFolders();
                        QDir dirpath(recentsyncs.at(i));
                        page->tab_name->setText(dirpath.path());
                        tabWidget->setTabText(tabWidget->indexOf(page->tab_stw), dirpath.path()); }
                    if (i % 6 == 1) {
                        page->sync_folders->addFolder()->setPath(recentsyncs.at(i));
                        page->sync_folders->addToFolders(2);
                    }
                    if (i % 6 == 2) page->sync_folders->addFolder()->setPath(recentsyncs.at(i));
                    if (i % 6 == 4) page->sync_hidden->setChecked(recentsyncs.at(i)=="checked");
                }
            }
            else {
                for (int i = 0; i < recentsyncs.count(); ++i) {
                    if (i % 12 == 0 || i == 0) { page = addSyncTab();
                        page->sync_folders->removeAllFolders();
                        QDir dirpath(recentsyncs.at(i));
                        page->tab_name->setText(dirpath.path());
                        tabWidget->setTabText(tabWidget->indexOf(page->tab_stw), dirpath.path()); }
                    if (i % 12 == 1) {
                        page->sync_folders->addFolder()->setPath(recentsyncs.at(i));
                        page->sync_folders->addToFolders(2);
                    }
                    if (i % 12 == 2) page->sync_folders->addFolder()->setPath(recentsyncs.at(i));
                    if (i % 12 == 4) page->sync_hidden->setChecked(recentsyncs.at(i)=="checked");
                }
            }
        } else {
            recentsyncs = settings.value("recentsyncs").toStringList();
            for (int i = 0; i < recentsyncs.count(); ++i) {
                if (i % 3 == 0 || i == 0) { page = addSyncTab();
                    page->sync_folders->removeAllFolders();
                    QDir dirpath(recentsyncs.at(i));
                    page->tab_name->setText(dirpath.path());
                    tabWidget->setTabText(tabWidget->indexOf(page->tab_stw), dirpath.path()); }
                if (i % 3 == 1) page->sync_folders->addFolder()->setPath(recentsyncs.at(i));
                if (i % 3 == 2) {
                    page->sync_folders->addFolder()->setPath(recentsyncs.at(i));
                    page->sync_folders->addToFolders(2);
                }
            }
        }
    }
    for (int i = 0; i < tabs_list.count(); ++i) {
        page = addSyncTab();
        page->loading = true;
        page->tab_name->setText(tabs_list.at(i));
        tabWidget->setTabText(tabWidget->indexOf(page->tab_stw), tabs_list.at(i));
        QStringList folders = sync_settings->value(QString("tab_%1_%2/folders").arg(tabs_list.at(i)).arg(i)).toStringList();
        if (folders.count()==0) {
            folders << sync_settings->value(QString("tab_%1_%2/folder1").arg(tabs_list.at(i)).arg(i)).toString();
            folders << sync_settings->value(QString("tab_%1_%2/folder2").arg(tabs_list.at(i)).arg(i)).toString();
        }
        page->sync_folders->removeAllFolders();
        //page->sync_folders->loadPathsAndLabelsFromList(folders);
        SyncFolder * folder;
        QString str;
        for (int n = 0; n < folders.count(); ++n) {
            folder = page->sync_folders->addFolder();
            folder->setPath(folders.at(n));
            str = sync_settings->value(QString("tab_%1_%2/folder_%3/label").arg(tabs_list.at(i)).arg(i).arg(folders.at(n))).toString();
            if (!str.isEmpty()) folder->setLabel(str);
            folder->dont_update_act->setChecked(sync_settings->value(QString("tab_%1_%2/folder_%3/dont_update").arg(tabs_list.at(i)).arg(i).arg(folders.at(n))).toString() == "checked");
            folder->update_only_act->setChecked(sync_settings->value(QString("tab_%1_%2/folder_%3/update_only").arg(tabs_list.at(i)).arg(i).arg(folders.at(n))).toString() == "checked");
            folder->backup_folder_act->setChecked(sync_settings->value(QString("tab_%1_%2/folder_%3/backup_folder").arg(tabs_list.at(i)).arg(i).arg(folders.at(n))).toString() == "checked");
            folder->no_empty_folders_act->setChecked(sync_settings->value(QString("tab_%1_%2/folder_%3/no_empty_folders").arg(tabs_list.at(i)).arg(i).arg(folders.at(n))).toString() == "checked");
            folder->slave_act->setChecked(sync_settings->value(QString("tab_%1_%2/folder_%3/slave").arg(tabs_list.at(i)).arg(i).arg(folders.at(n))).toString() == "checked");
        }
        page->syncFoldersEdited();
        page->last_sync = sync_settings->value(QString("tab_%1_%2/last_sync").arg(tabs_list.at(i)).arg(i)).toString();
        if (!page->last_sync.isEmpty()) page->status_table_item->setText(tr("Last synced on %1").arg(page->last_sync));
        page->allowed_difference = sync_settings->value(QString("tab_%1_%2/allowed_difference").arg(tabs_list.at(i)).arg(i), 2).toInt();
        //page->sync_folder_1->setText(sync_settings->value(QString("tab_%1_%2/folder1").arg(tabs_list.at(i)).arg(i)).toString());
        //page->sync_folder_2->setText(sync_settings->value(QString("tab_%1_%2/folder2").arg(tabs_list.at(i)).arg(i)).toString());
        //showAdvancedGroupBox(sync_settings->value(QString("tab_%1_%2/advanced").arg(tabs_list.at(i)).arg(i)).toString()=="checked", page);
        page->sync_hidden->setChecked(sync_settings->value(QString("tab_%1_%2/sync_hidden").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->sync_nosubdirs->setChecked(sync_settings->value(QString("tab_%1_%2/sync_nosubdirs").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->ignore_blacklist->setChecked(sync_settings->value(QString("tab_%1_%2/ignore_blacklist").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->fast_analyse->setChecked(sync_settings->value(QString("tab_%1_%2/fast_analyse").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->analyse_special_only->setChecked(sync_settings->value(QString("tab_%1_%2/analyse_special_only").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->sort_analysis_by_action->setChecked(sync_settings->value(QString("tab_%1_%2/sort_analysis_by_action").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        if (sync_settings->value(QString("tab_%1_%2/backup_folder_1").arg(tabs_list.at(i)).arg(i)).toString()=="checked") page->setBackupFolder(0, true);
        if (sync_settings->value(QString("tab_%1_%2/backup_folder_2").arg(tabs_list.at(i)).arg(i)).toString()=="checked") page->setBackupFolder(1, true);
        if (sync_settings->value(QString("tab_%1_%2/update_only_1").arg(tabs_list.at(i)).arg(i)).toString()=="checked") page->sync_folders->at(0)->update_only_act->setChecked(true);
        if (sync_settings->value(QString("tab_%1_%2/update_only_2").arg(tabs_list.at(i)).arg(i)).toString()=="checked") page->sync_folders->at(1)->update_only_act->setChecked(true);
        if (sync_settings->value(QString("tab_%1_%2/move").arg(tabs_list.at(i)).arg(i)).toString()=="checked") page->move->setChecked(true);
        if (sync_settings->value(QString("tab_%1_%2/clone_folder1").arg(tabs_list.at(i)).arg(i)).toString()=="checked") page->sync_folders->at(1)->slave_act->setChecked(true);
        if (sync_settings->value(QString("tab_%1_%2/propagate_deletions").arg(tabs_list.at(i)).arg(i)).toString()=="checked") {
            page->propagate_deletions->setChecked(true); page->propagatedStateChanged(true); }
        if (sync_settings->value(QString("tab_%1_%2/alert_collisions").arg(tabs_list.at(i)).arg(i)).toString()=="checked") {
            page->alert_collisions->setChecked(true); }
        page->backup_folders->setChecked(sync_settings->value(QString("tab_%1_%2/backup_folders").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->update_only->setChecked(sync_settings->value(QString("tab_%1_%2/update_only").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->text_database_action->setChecked(sync_settings->value(QString("tab_%1_%2/text_database").arg(tabs_list.at(i)).arg(i), "checked").toString()=="checked");
        page->no_empty_folders->setChecked(sync_settings->value(QString("tab_%1_%2/no_empty_folders").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        page->allow_DST->setChecked(sync_settings->value(QString("tab_%1_%2/allow_dst").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        if (sync_settings->value(QString("tab_%1_%2/periodical").arg(tabs_list.at(i)).arg(i)).toString()=="checked") {
            SyncSchedule * sync_sched = addSchedule(tabs_list.at(i));
            sync_sched->periodical_interval = sync_settings->value(QString("tab_%1_%2/sync_interval").arg(tabs_list.at(i)).arg(i), 1).toInt();
            sync_sched->timing_tab_index = 1;
            sync_sched->sched_tab_list << tabs_list.at(i);
            if (sync_settings->value(QString("tab_%1_%2/periodical_stop").arg(tabs_list.at(i)).arg(i)).toString()=="on") {
                startSchedule(item_sched_map.key(sync_sched));
            }
        }
#ifndef Q_WS_WIN
        page->symlinks->setChecked(sync_settings->value(QString("tab_%1_%2/symlinks").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
#endif
        //page->filters->setChecked(sync_settings->value(QString("tab_%1_%2/filters_gb").arg(tabs_list.at(i)).arg(i)).toString()=="checked");
        QStringList flist = sync_settings->value(QString("tab_%1_%2/flist").arg(tabs_list.at(i)).arg(i)).toStringList();
        QList<QAction *> actions = page->filters_menu->actions();
        for (int n = 0; n < flist.count(); ++n) {
            for (int f = 0; f < actions.count(); ++f) {
                if (actions.at(f)->text() == flist.at(n)) {
                    actions.at(f)->setChecked(true);
                }
            }
        }
        page->files_blacklist = sync_settings->value(QString("tab_%1_%2/files_blacklist").arg(tabs_list.at(i)).arg(i), sync_settings->value("files_blacklist")).toStringList();
        page->folders_blacklist = sync_settings->value(QString("tab_%1_%2/folders_blacklist").arg(tabs_list.at(i)).arg(i), sync_settings->value("folders_blacklist")).toStringList();
        page->exts_blacklist = sync_settings->value(QString("tab_%1_%2/exts_blacklist").arg(tabs_list.at(i)).arg(i), sync_settings->value("exts_blacklist")).toStringList();
        page->loading = false;
    }
    if (sync_settings->value("synchronised").toStringList().count() != 0) {
        convertOldTempSettings(sync_settings->value("synchronised").toStringList());
    }
    restore_clean_selected->setChecked(sync_settings->value("restore_clean_selected", true).toBool());
    restore_clean_by_date->setChecked(sync_settings->value("restore_clean_by_date").toBool());
    restore_clean_repeated->setChecked(sync_settings->value("restore_clean_repeated").toBool());
    restore_clean_date->setValue(sync_settings->value("restore_clean_date", 7).toInt());
    restore_clean_files->setValue(sync_settings->value("restore_clean_files", 3).toInt());
    restore_clean_auto_gb->setChecked(sync_settings->value("restore_clean_auto_gb", true).toBool());
    restore_clean_auto_files->setValue(sync_settings->value("restore_clean_auto_files", 3).toInt());
    files_blacklist = sync_settings->value("files_blacklist").toStringList();
    folders_blacklist = sync_settings->value("folders_blacklist").toStringList();
    exts_blacklist = sync_settings->value("exts_blacklist").toStringList();
    QStringList multitabs_list = sync_settings->value("multitabs").toStringList();
    MultisyncPage * multi_page;
    if (multitabs_list.count()==0) {
        QStringList multisyncs_items = sync_settings->value("multisyncs_items").toStringList();
        QListWidgetItem * item = nullptr;
        multi_page = addMultiTab();
        for (int i = 0; i < multisyncs_items.count(); ++i) {
            if (i % 2 == 0) {
                item = new QListWidgetItem();
                item->setText(multisyncs_items.at(i));
            }
            else if (i % 2 == 1) {
                if (multisyncs_items.at(i) == "unchecked") item->setCheckState(Qt::Unchecked);
                else item->setCheckState(Qt::Checked);
                multi_page->list_multi->addItem(item);
            }
        }
        multi_page->destination_multi->setText(sync_settings->value("destination_multi").toString());
        multi_page->destinationTextChanged();
    }
    for (int i = 0; i < multitabs_list.count(); ++i) {
        multi_page = addMultiTab();
        multi_page->loading = true;
        multi_page->tab_name->setText(multitabs_list.at(i));
        multi_tabWidget->setTabText(multi_tabWidget->indexOf(multi_page), multitabs_list.at(i));
        QStringList sources = sync_settings->value(QString("multitab_%1_%2/sources").arg(multitabs_list.at(i)).arg(i)).toStringList();
        QListWidgetItem * item = nullptr;
        for (int s = 0; s < sources.count(); ++s) {
            if (s % 2 == 0) {
                item = new QListWidgetItem();
                item->setText(sources.at(s));
            }
            else if (s % 2 == 1) {
                if (sources.at(s) == "unchecked") item->setCheckState(Qt::Unchecked);
                else item->setCheckState(Qt::Checked);
                multi_page->list_multi->addItem(item);
            }
        }
        multi_page->destination_multi->setText(sync_settings->value(QString("multitab_%1_%2/destination").arg(multitabs_list.at(i)).arg(i)).toString());
        multi_page->foldersChanged();
        multi_page->last_sync = sync_settings->value(QString("multitab_%1_%2/last_sync").arg(multitabs_list.at(i)).arg(i)).toString();
        if (!multi_page->last_sync.isEmpty()) multi_page->status_table_item->setText(tr("Last synced on %1").arg(multi_page->last_sync));
        multi_page->allowed_difference = sync_settings->value(QString("multitab_%1_%2/allowed_difference").arg(multitabs_list.at(i)).arg(i), 2).toInt();
        //multi_page->showAdvancedGroupBox(sync_settings->value(QString("multitab_%1_%2/advanced").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->sync_hidden->setChecked(!(sync_settings->value(QString("multitab_%1_%2/sync_hidden").arg(multitabs_list.at(i)).arg(i)).toString()=="unchecked"));
        multi_page->backup_folder_1->setChecked(sync_settings->value(QString("multitab_%1_%2/backup_folder_1").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->backup_folder_2->setChecked(sync_settings->value(QString("multitab_%1_%2/backup_folder_2").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->update_only_1->setChecked(sync_settings->value(QString("multitab_%1_%2/update_only_1").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->update_only_2->setChecked(sync_settings->value(QString("multitab_%1_%2/update_only_2").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->sync_nosubdirs->setChecked(sync_settings->value(QString("multitab_%1_%2/sync_nosubdirs").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->ignore_blacklist->setChecked(sync_settings->value(QString("multitab_%1_%2/ignore_blacklist").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->move->setChecked(sync_settings->value(QString("multitab_%1_%2/move").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->clone_folder1->setChecked(sync_settings->value(QString("multitab_%1_%2/clone_folder1").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->propagate_deletions->setChecked(sync_settings->value(QString("multitab_%1_%2/propagate_deletions").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->fast_analyse->setChecked(sync_settings->value(QString("multitab_%1_%2/fast_analyse").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->analyse_special_only->setChecked(sync_settings->value(QString("multitab_%1_%2/analyse_special_only").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->alert_collisions->setChecked(sync_settings->value(QString("multitab_%1_%2/alert_collisions").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->no_empty_folders->setChecked(sync_settings->value(QString("multitab_%1_%2/no_empty_folders").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        multi_page->allow_DST->setChecked(!(sync_settings->value(QString("multitab_%1_%2/allow_dst").arg(multitabs_list.at(i)).arg(i)).toString()=="unchecked"));
        //multi_page->backup_folders->setChecked(sync_settings->value(QString("multitab_%1_%2/backup_folders").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
        //multi_page->update_only->setChecked(sync_settings->value(QString("multitab_%1_%2/update_only").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
#ifndef Q_WS_WIN
        multi_page->symlinks->setChecked(sync_settings->value(QString("multitab_%1_%2/symlinks").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
#endif
        /*
        multi_page->filters->setChecked(sync_settings->value(QString("multitab_%1_%2/filters_gb").arg(multitabs_list.at(i)).arg(i)).toString()=="checked");
                QStringList flist = sync_settings->value(QString("multitab_%1_%2/flist").arg(multitabs_list.at(i)).arg(i)).toStringList();
                for (int n = 0; n < flist.count(); ++n) {
                        for (int f = 0; f < multi_page->lw_filters->count(); ++f) {
                                if (multi_page->lw_filters->item(f)->text() == flist.at(n)) {
                                        multi_page->lw_filters->item(f)->setCheckState(Qt::Checked);
                                }
                        }
                }*/
        multi_page->files_blacklist = sync_settings->value(QString("multitab_%1_%2/files_blacklist").arg(multitabs_list.at(i)).arg(i), sync_settings->value("files_blacklist")).toStringList();
        multi_page->folders_blacklist = sync_settings->value(QString("multitab_%1_%2/folders_blacklist").arg(multitabs_list.at(i)).arg(i), sync_settings->value("folders_blacklist")).toStringList();
        multi_page->exts_blacklist = sync_settings->value(QString("multitab_%1_%2/exts_blacklist").arg(multitabs_list.at(i)).arg(i), sync_settings->value("exts_blacklist")).toStringList();
        if (sync_settings->value(QString("multitab_%1_%2/vars_map").arg(multitabs_list.at(i)).arg(i), "None").toString() != "None") {
            multi_page->vars_map.clear();
            /*QStringList vars = sync_settings->value(QString("multitab_%1_%2/vars_map").arg(multitabs_list.at(i)).arg(i), "").toString().split("<:?:>");
            for (int i = 0; i < vars.count(); ++i) {
                if (vars.at(i).split(";").count() >= 2) {
                    multi_page->vars_map.insert(vars.at(i).split(";").first(), vars.at(i).split(";").at(1));
                }
            }*/
            multi_page->variablesFromString(sync_settings->value(QString("multitab_%1_%2/vars_map").arg(multitabs_list.at(i)).arg(i), "").toString());
        }
        multi_page->loading = false;
    }
    run_hidden = sync_settings->value("run_hidden", false).toBool();
    actionDisable_tray_messages->setChecked(sync_settings->value("disable_tray_messages").toBool());
    actionSync_at_launch->setChecked(sync_settings->value("sync_at_launch").toBool());
#ifdef Q_WS_WIN
    actionShut_down_after_sync->setChecked(sync_settings->value("shut_down_after_sync").toBool());
#endif
    actionQuit_after_sync->setChecked(sync_settings->value("quit_after_sync").toBool());
    raw_temp_path = sync_settings->value("temp_path", temp_path).toString();
    QDir dir (raw_temp_path);
    temp_path = dir.absolutePath();
    QStringList schedules = sync_settings->value("schedules").toStringList(); QString schedule;
    SyncSchedule * sync_sched = nullptr;
    for (int i = 0; i < schedules.count(); ++i) {
        if (i % 2 == 0) {
            sync_sched = addSchedule(schedules.at(i));
            sync_sched->sched_tab_list = sync_settings->value(QString("sched_%1_%2_tabs").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
            sync_sched->sched_multitab_list = sync_settings->value(QString("sched_%1_%2_multitabs").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
            sync_sched->sched_time_list = sync_settings->value(QString("sched_%1_%2_times").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
            sync_sched->sched_checked_time_list = sync_settings->value(QString("sched_%1_%2_checkedtimes").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
            sync_sched->periodical_interval = sync_settings->value(QString("sched_%1_%2_periodical_interval").arg(schedules.at(i)).arg(i/2), 1).toInt();
            sync_sched->timing_tab_index = sync_settings->value(QString("sched_%1_%2_tab_index").arg(schedules.at(i)).arg(i/2), 0).toInt();
            sync_sched->sched_date_list = sync_settings->value(QString("sched_%1_%2_dates").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
            sync_sched->sched_checked_date_list = sync_settings->value(QString("sched_%1_%2_checkeddates").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
            sync_sched->sched_unchecked_days_list = sync_settings->value(QString("sched_%1_%2_uncheckeddays").arg(schedules.at(i)).arg(i/2), QStringList()).toStringList();
        }
        if (i % 2 == 1) {
            if (schedules.at(i)=="on") {
                startSchedule(item_sched_map.key(sync_sched));
            }
        }
    }
    tabWidget->setCurrentIndex(sync_settings->value("current_synctab", 0).toInt());
    multi_tabWidget->setCurrentIndex(sync_settings->value("current_multitab", 0).toInt());
    this->move(sync_settings->value("pos", (settings.value("pos", this->pos()))).toPoint());
    this->resize(sync_settings->value("size", (settings.value("size", this->size()))).toSize());
    actionRun_hidden->setChecked(run_hidden);
    trayIconVisible(!run_hidden);
}
