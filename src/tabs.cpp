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

//+++ Shared +++

void AbstractSyncPage::save()
{
    if (slist_path == "")
    {
        saveAs();
        return;
    }
    saveAs(slist_path);
}

void AbstractSyncPage::saveAs()
{
    qApp->processEvents();
    QString file_name = QFileDialog::getSaveFileName(mp_parent,
                tr("Synkron - Save tab"),
                QString("%1/%2.slist").arg(QDir::homePath()).arg(tabNameText()),
                tr("Synkron Tabs (*.slist)"));
    saveAs(file_name);
}

void AbstractSyncPage::sharedSave(QDomDocument & domdoc, QDomElement & el_sync)
{
    //TODO: I think it need to write unit tests to save function

    el_sync.setAttribute("last_sync", last_sync);
    el_sync.setAttribute("allowed_difference", allowed_difference);
//*** Blacklist ***
    QDomElement el_blacklist = domdoc.createElement("blacklist");
    el_blacklist.setAttribute("files", files_blacklist.join(";"));
    el_blacklist.setAttribute("folders", folders_blacklist.join(";"));
    el_blacklist.setAttribute("extensions", exts_blacklist.join(";"));
    el_sync.appendChild(el_blacklist);

//*** Advanced settings ***
    QDomElement el_advanced = domdoc.createElement("advanced_options");
    el_sync.appendChild(el_advanced);
    QDomElement el_adv_global = domdoc.createElement("global");
    el_adv_global.setAttribute("sync_hidden",
        sync_hidden->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("propagate_deletions",
        propagate_deletions->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("alert_collisions",
        alert_collisions->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("sync_nosubdirs",
        sync_nosubdirs->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("ignore_blacklist",
        ignore_blacklist->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("symlinks",
        symlinks->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("no_empty_folders",
        no_empty_folders->isChecked() ? QString("1") : QString("0"));
    el_advanced.appendChild(el_adv_global);
    QDomElement el_adv_analysis = domdoc.createElement("analysis");
    el_adv_analysis.setAttribute("fast_analyse",
        fast_analyse->isChecked() ? QString("1") : QString("0"));
    el_adv_analysis.setAttribute("analyse_special_only",
        analyse_special_only->isChecked() ? QString("1") : QString("0"));
    el_advanced.appendChild(el_adv_analysis);
}

void AbstractSyncPage::sharedLoad(QDomElement & el_sync)
{
    //TODO: I think it need to write unit tests to load function
    loading = true;
    allowed_difference = el_sync.attribute("allowed_difference").toInt();
    last_sync = el_sync.attribute("last_sync").toInt();
//*** Blacklist ***
    QDomElement el_blacklist = el_sync.firstChildElement("blacklist");
    files_blacklist = el_blacklist.attribute("files").split(";");
    folders_blacklist = el_blacklist.attribute("folders").split(";");
    exts_blacklist = el_blacklist.attribute("extensions").split(";");
    files_blacklist.removeAll("");
    folders_blacklist.removeAll("");
    exts_blacklist.removeAll("");

//*** Advanced ***
    QDomElement el_advanced = el_sync.firstChildElement("advanced_options");
    QDomElement el_adv_global = el_advanced.firstChildElement("global");
    sync_hidden->setChecked(el_adv_global.attribute("sync_hidden").toInt());
    propagate_deletions->setChecked(el_adv_global.attribute("propagate_deletions").toInt());
    alert_collisions->setChecked(el_adv_global.attribute("alert_collisions").toInt());
    sync_nosubdirs->setChecked(el_adv_global.attribute("sync_nosubdirs").toInt());
    ignore_blacklist->setChecked(el_adv_global.attribute("ignore_blacklist").toInt());
    no_empty_folders->setChecked(el_adv_global.attribute("no_empty_folders").toInt());
#ifndef Q_WS_WIN
    symlinks->setChecked(el_adv_global.attribute("symlinks").toInt());
#endif
    QDomElement el_adv_analysis = el_advanced.firstChildElement("analysis");
    fast_analyse->setChecked(el_adv_analysis.attribute("fast_analyse").toInt());
    analyse_special_only->setChecked(el_adv_analysis.attribute("analyse_special_only").toInt());
    loading = false;
}

//+++ Sync +++

void SyncPage::saveAs(QString file_name)
{
    if (file_name.isEmpty()) { return; }
    QDomDocument domdoc("SynkronSync");
    QDomElement el_sync = domdoc.createElement("sync");
    el_sync.setAttribute("type", "sync");
    domdoc.appendChild(el_sync);
//*** Name ***
    QDomElement el_name = domdoc.createElement("name");
    el_name.setAttribute("data", tab_name->text());
    el_sync.appendChild(el_name);
//*** Folders ***
    QDomElement el_folders = domdoc.createElement("folders");
    el_folders.setAttribute("paths", syncFoldersList().join(";"));
    el_sync.appendChild(el_folders);
    for (int i = 0; i < sync_folders->count(); ++i)
    {
        QDomElement el_folder = domdoc.createElement("folder");
        el_folder.setAttribute("path", sync_folders->at(i)->path());
        el_folder.setAttribute("label", sync_folders->at(i)->label());
        QDomElement el_folder_adv = domdoc.createElement("advanced");
        el_folder_adv.setAttribute("update_only",
            sync_folders->at(i)->update_only_act->isChecked() ? QString("1") : QString("0"));
        el_folder_adv.setAttribute("dont_update",
            sync_folders->at(i)->dont_update_act->isChecked() ? QString("1") : QString("0"));
        el_folder_adv.setAttribute("backup_folder",
            sync_folders->at(i)->backup_folder_act->isChecked() ? QString("1") : QString("0"));
        el_folder_adv.setAttribute("slave",
            sync_folders->at(i)->slave_act->isChecked() ? QString("1") : QString("0"));
        el_folder_adv.setAttribute("no_empty_folders",
            sync_folders->at(i)->no_empty_folders_act->isChecked() ? QString("1") : QString("0"));
        el_folder.appendChild(el_folder_adv);
        el_folders.appendChild(el_folder);
    }
//*** Shared ***
    sharedSave(domdoc, el_sync);
//*** Advanced ***
    QDomElement el_advanced = el_sync.firstChildElement("advanced_options");
//*** Filters ***
    QDomElement el_adv_filters = domdoc.createElement("filters");
    //el_adv_filters.setAttribute("filters_gb", filters->isChecked() ? QString("1") : QString("0"));
    QStringList checked;
    QList<QAction *> actions = filters_menu->actions();
    for (int n = 0; n < actions.count(); ++n) {
        if (actions.at(n)->isChecked()) {
            checked << actions.at(n)->text();
        }
    }
    el_adv_filters.setAttribute("checked", checked.join(";"));
    el_advanced.appendChild(el_adv_filters);
//*** Analysis ***
    QDomElement el_adv_analysis = el_advanced.firstChildElement("analysis");
    el_adv_analysis.setAttribute("sort_analysis_by_action",
        sort_analysis_by_action->isChecked() ? QString("1") : QString("0"));
//*** Global ***
    QDomElement el_adv_global = el_advanced.firstChildElement("global");
    el_adv_global.setAttribute("text_database",
        text_database_action->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("update_only",
        update_only->isChecked() ? QString("1") : QString("0"));
    el_adv_global.setAttribute("backup_folders",
        backup_folders->isChecked() ? QString("1") : QString("0"));

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Synkron"), tr("Cannot write file %1:\n%2.").arg(file_name).arg(file.errorString()));
        return;
    }
    QTextStream out(&file);
    out << domdoc.toString();
    slist_path = file_name;
}

void SyncPage::load(QDomDocument & domdoc, QString file_name)
{
    QDomElement el_sync = domdoc.firstChildElement("sync");
    QDomElement el_folders = el_sync.firstChildElement("folders");
//*** Folders ***
    QDomNodeList folders = el_folders.elementsByTagName("folder");
    sync_folders->removeAllFolders();
    SyncFolder * folder;
    QDomElement e;
    for (int f = 0; f < folders.count(); ++f)
    {
        if (folders.at(f).isElement())
        {
            e = folders.at(f).toElement();
            folder = sync_folders->addFolder();
            folder->setPath(e.attribute("path"));
            folder->setLabel(e.attribute("label"));
            QDomElement el_folder_adv = e.firstChildElement("advanced");
            folder->update_only_act->setChecked(el_folder_adv.attribute("update_only") == "1");
            folder->dont_update_act->setChecked(el_folder_adv.attribute("dont_update") == "1");
            folder->backup_folder_act->setChecked(el_folder_adv.attribute("backup_folder") == "1");
            folder->slave_act->setChecked(el_folder_adv.attribute("slave") == "1");
            folder->no_empty_folders_act->setChecked(el_folder_adv.attribute("no_empty_folders") == "1");
        }
    }
    sync_folders->addToFolders(2);
    QDomElement el_name = el_sync.firstChildElement("name");
    if (el_name.attribute("data")!="")
    {
        tab_name->setText(el_name.attribute("data"));
        mp_parent->tabNameChanged();
    }
    last_sync = el_sync.attribute("last_sync");
    if (!last_sync.isEmpty()) status_table_item->setText(tr("Last synced on %1").arg(last_sync));

    sharedLoad(el_sync);
    QDomElement el_advanced = el_sync.firstChildElement("advanced_options");
//*** Filters ***
    QDomElement el_adv_filters = el_advanced.firstChildElement("filters");
    //filters->setChecked(el_adv_filters.attribute("filters_gb").toInt());
    QStringList flist = el_adv_filters.attribute("checked").split(";");
    QList<QAction *> actions = filters_menu->actions();
    for (int n = 0; n < flist.count(); ++n)
    {
        for (int f = 0; f < actions.count(); ++f)
        {
            if (actions.at(f)->text() == flist.at(n))
            {
                actions.at(f)->setChecked(true);
            }
        }
    }
// *** Global ***
    QDomElement el_adv_global = el_advanced.firstChildElement("global");
    text_database_action->setChecked(el_adv_global.attribute("text_database") == "1");
    update_only->setChecked(el_adv_global.attribute("update_only") == "1");
    backup_folders->setChecked(el_adv_global.attribute("backup_folders") == "1");
//*** Analysis ***
    QDomElement el_adv_analysis = el_advanced.firstChildElement("analysis");
    sort_analysis_by_action->setChecked(el_adv_analysis.attribute("sort_analysis_by_action") == "1");

    slist_path = file_name;
}

//+++ Multisync +++

void MultisyncPage::saveAs(QString file_name)
{
    if (file_name.isEmpty()) { return; }
    QDomDocument domdoc("SynkronMultisync");
    QDomElement el_sync = domdoc.createElement("sync");
    el_sync.setAttribute("type", "multisync");
    domdoc.appendChild(el_sync);
    QStringList source_list; QString checkstates;
    for (int i = 0; i < list_multi->count(); ++i) {
        source_list << list_multi->item(i)->text();
        checkstates.append(list_multi->item(i)->checkState() == Qt::Checked ? "+" : "-");
    }
    QString dest = destination_multi->text();
    QDomElement el_sources = domdoc.createElement("sources");
    el_sources.setAttribute("paths", source_list.join("|"));
    el_sources.setAttribute("checkstates", checkstates);
    el_sync.appendChild(el_sources);
    QDomElement el_destination = domdoc.createElement("destination");
    el_destination.setAttribute("data", dest);
    el_sync.appendChild(el_destination);
    QDomElement el_name = domdoc.createElement("name");
    el_name.setAttribute("data", tab_name->text());
    el_sync.appendChild(el_name);
    QDomElement el_variables = domdoc.createElement("variables");
    el_variables.setAttribute("data", variablesToString());
    el_sync.appendChild(el_variables);

    sharedSave(domdoc, el_sync);
//*** Advanced folder settings ***
    QDomElement el_advanced = el_sync.firstChildElement("advanced_options");
    QDomElement el_adv_folder_1 = domdoc.createElement("folder_1");
    el_adv_folder_1.setAttribute("backup_folder_1", backupFolder(0) ? QString("1") : QString("0"));
    el_adv_folder_1.setAttribute("update_only_1", updateOnly(0) ? QString("1") : QString("0"));
    el_adv_folder_1.setAttribute("move", move->isChecked() ? QString("1") : QString("0"));
    el_advanced.appendChild(el_adv_folder_1);
    QDomElement el_adv_folder_2 = domdoc.createElement("folder_2");
    el_adv_folder_2.setAttribute("backup_folder_2", backupFolder(1) ? QString("1") : QString("0"));
    el_adv_folder_2.setAttribute("update_only_2", updateOnly(1) ? QString("1") : QString("0"));
    el_adv_folder_2.setAttribute("clone_folder1", clone_folder1->isChecked() ? QString("1") : QString("0"));
    el_advanced.appendChild(el_adv_folder_2);

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Synkron"), tr("Cannot write file %1:\n%2.").arg(file_name).arg(file.errorString()));
        return;
    }
    QTextStream out(&file);
    out << domdoc.toString();
    slist_path = file_name;
}

void MultisyncPage::load(QDomDocument & domdoc, QString file_name)
{
    QDomElement el_sync = domdoc.firstChildElement("sync");
    QDomElement el_sources = el_sync.firstChildElement("sources");
    QStringList source_list = el_sources.attribute("paths").split('|');
    list_multi->clear();
    QListWidgetItem * item;
    for (int i = 0; i < source_list.count(); ++i) {
        if (source_list.at(i) == "") continue;
        item = new QListWidgetItem (source_list.at(i));
        item->setCheckState(el_sources.attribute("checkstates").at(i) == '+' ? Qt::Checked : Qt::Unchecked);
        list_multi->addItem(item);
    }
    QDomElement el_destination = el_sync.firstChildElement("destination");
    if (el_destination.attribute("data") != "") {
        destination_multi->setText(el_destination.attribute("data"));
    }
    QDomElement el_name = el_sync.firstChildElement("name");
    if (el_name.attribute("data") != "") {
        tab_name->setText(el_name.attribute("data"));
        multitabNameChanged();
    }
    QDomElement el_variables = el_sync.firstChildElement("variables");
    if (!el_variables.attribute("data").isEmpty()) {
        variablesFromString(el_variables.attribute("data"));
    }
    last_sync = el_sync.attribute("last_sync");
    if (!last_sync.isEmpty()) status_table_item->setText(tr("Last synced on %1").arg(last_sync));

    sharedLoad(el_sync);
//*** Advanced folder settings ***
    QDomElement el_advanced = el_sync.firstChildElement("advanced_options");
    QDomElement el_adv_folder_1 = el_advanced.firstChildElement("folder_1");
    setBackupFolder(0, el_adv_folder_1.attribute("backup_folder_1").toInt());
    setUpdateOnly(0, el_adv_folder_1.attribute("update_only_1").toInt());
    move->setChecked(el_adv_folder_1.attribute("move").toInt());
    QDomElement el_adv_folder_2 = el_advanced.firstChildElement("folder_2");
    setBackupFolder(1, el_adv_folder_2.attribute("backup_folder_2").toInt());
    setUpdateOnly(1, el_adv_folder_2.attribute("update_only_2").toInt());
    clone_folder1->setChecked(el_adv_folder_2.attribute("clone_folder1").toInt());

    slist_path = file_name;
}

