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

void MainWindow::toRestorePage()
{
    date_of_sync->setDateTime(QDateTime(QDate(2000, 01, 01), QTime(00, 00, 00)));
    path_of_syncfile->clear();
    QListWidgetItem * item;
    restore_list->clear();
    //QString last_time = "";
    //QString last_colour = "grey";
    QStringList time_groups = temp_settings->childGroups();
    for (int i = 0; i < time_groups.count(); ++i) {
        temp_settings->beginGroup(time_groups.at(i));
        QStringList file_groups = temp_settings->childGroups();
        for (int n = 0; n < file_groups.count(); ++n) {
            temp_settings->beginGroup(file_groups.at(n));
            item = new QListWidgetItem (temp_settings->value("file_name").toString());
            item->setCheckState(Qt::Unchecked);
            if (i % 2 == 1) {
                item->setBackground(QBrush(QColor(226, 226, 226)));
            }
            QStringList list;
            list << temp_settings->value("file_name").toString();
            list << time_groups.at(i);
            list << temp_settings->value("old_file_path").toString();
            list << temp_settings->value("temp_file_path").toString();
            item->setData(Qt::UserRole, list);
            item->setToolTip(temp_settings->value("old_file_path").toString());
            restore_list->addItem(item);
            temp_settings->endGroup();
        }
        temp_settings->endGroup();
    }
    to_black_list->setCheckState(Qt::Unchecked);
}

void MainWindow::restoreItemChanged(QListWidgetItem * item, QListWidgetItem *) //yyyy.MM.dd-hh.mm
{
     if (!item) { return; }
     QStringList item_list = item->data(Qt::UserRole).toStringList();
     bool found = false;
     for (int i = 0; i < files_blacklist.count(); ++i) {
        if (files_blacklist.at(i) == item_list.at(2)) {
            to_black_list->setCheckState(Qt::Checked); found = true;
        }
     }
     if (!found) {
            to_black_list->setCheckState(Qt::Unchecked);
        }
     QStringList date_time = item_list.at(1).split("-");
     QStringList date_str = date_time.at(0).split(".");
     QStringList time_str = date_time.at(1).split(".");
     QDate date (date_str.at(0).toInt(), date_str.at(1).toInt(), date_str.at(2).toInt());
     QTime time (time_str.at(0).toInt(), time_str.at(1).toInt(), time_str.at(2).toInt());
     date_of_sync->setDate(date);
     date_of_sync->setTime(time);
     restore_select_dt_edit->setDate(date);
     restore_select_dt_edit->setTime(time);
     path_of_syncfile->setText(item_list.at(2));
}

void MainWindow::restoreFiles()
{
    //MTFile * file; MTFile * old_file;
    int restored_items = 0; int errors = 0;// int restored [restore_list->count()];
    for (int c = restore_list->count()-1; c >= 0; --c) {
        if (restore_list->item(c)->checkState() == Qt::Checked) {
            if (restoreItem(restore_list->item(c))) restored_items++;
            else errors++;
        }
    }
    /*for (int l = restored_items; l > 0; --l) {
        delete restore_list->item(restored[l-1]);
    }*/
    if (restored_items == 0 && errors == 0) {
        QMessageBox::warning(this, tr("Synkron"), tr("No files selected."));
    } else {
        QMessageBox::information(this, tr("Synkron"), tr("%1 files restored.").arg(restored_items));
    }
}

void MainWindow::restoreCurrentItem()
{
    if (restore_list->currentItem()!=0) restoreItem(restore_list->currentItem());
}

bool MainWindow::restoreItem(QListWidgetItem * item)
{
    QStringList item_list = item->data(Qt::UserRole).toStringList();
    if (!restoreFile(item_list.at(3), item_list.at(2), item_list.at(1))) {
        QMessageBox::critical(this, tr("Synkron"), tr("Unknown error restoring file: %1").arg(item_list.at(0)));
        return false;
    }
    delete item;
    return true;
}

//file_path - temp-file path, old_file_path - destination path
bool MainWindow::restoreFile(QString file_path, QString old_file_path, QString group_name)
{
    MTFile * file = new MTFile (file_path);
    if (!file->exists()) { delete file; return false; }
    MTFile * old_file = new MTFile (old_file_path);
    if (old_file->exists()) {
        if (!file->copy(QString("%1.res").arg(old_file_path))) { delete file; delete old_file; return false; }
        QString file_name = old_file->fileName();
        if (!old_file->remove()) { delete file; delete old_file; return false; }
        delete file; file = new MTFile (QString("%1.res").arg(old_file_path));
        if (!file->rename(file_name)) { delete file; delete old_file; return false; }
        delete file; file = new MTFile (file_path);
    } else if (!QFileInfo(old_file_path).dir().exists()) {
        if (QDir().mkpath(QFileInfo(old_file_path).dir().path())) {
            if (!file->copy(old_file_path)) { delete file; delete old_file; return false; }
        } else {
            delete file; delete old_file; return false;
        }
     } else {
        if (!file->copy(old_file_path)) { delete file; delete old_file; return false; }
    }
    file->remove(); delete file;
    file = new MTFile (old_file_path);
    if (mainStackedWidget->currentIndex() == 1) {
        file->touch(qApp);
    }
    delete file; delete old_file;
    QFileInfo info(file_path);
        QDir dir = info.dir();
        QString dirname = dir.dirName();
    dir.cdUp();
    dir.rmdir(dirname);
    //restored[restored_items] = restore_list->row(item);
    temp_settings->beginGroup(group_name);
    temp_settings->remove(file_path.split("/").last());
    if (temp_settings->childGroups().count() == 0) {
        temp_settings->remove("");
    }
    temp_settings->endGroup();
    /*for (int o = 0; o < synchronised.count(); ++o) {
        if (synchronised.at(o) == file_path) {
            synchronised.removeAt(o);
            synchronised.removeAt(o-1);
            synchronised.removeAt(o-2);
            synchronised.removeAt(o-3);
            break;
        }
    }*/
    return true;
}

void MainWindow::addToBlackList(int state)
{
    if (!restore_list->currentItem()) { to_black_list->setCheckState(Qt::Unchecked); return; }
    QStringList item_list = restore_list->currentItem()->data(Qt::UserRole).toStringList();
    if (item_list.count() != 0) {
        if (state ==  Qt::Checked) {
            bool found = false;
            for (int i = 0; i < files_blacklist.count(); ++i) {
                if (files_blacklist.at(i) == item_list.at(2)) {
                    found = true;
                }
            }
            if (!found) {
                files_blacklist << item_list.at(2);
            }
        }
        else if (state == Qt::Unchecked) {
            for (int i = files_blacklist.count(); i > 0; --i) {
                if (files_blacklist.at(i-1) == item_list.at(2)) {
                    files_blacklist.removeAt(i-1);
                }
            }
        }
    }
}

void MainWindow::selTmpAll()
{
    for (int i = 0; i < restore_list->count(); ++i) {
        if (!restore_list->item(i)->isHidden()) {
            restore_list->item(i)->setCheckState(Qt::Checked);
        }
    }
}

void MainWindow::setCleanGB()
{
    restore_clean_gb->setTitle(tr("Clean temporary files"));
    restore_clean_selected = new QCheckBox(restore_clean_gb);
    restore_clean_selected->setChecked(true);
    restore_clean_selected->setText(tr("Delete selected files only"));
    restore_clean_selected->setStatusTip(tr("Delete selected files only"));
    restore_clean_gb->addWidget(restore_clean_selected, 0, 0);

    QHBoxLayout * clean_date_layout = new QHBoxLayout;
    restore_clean_by_date = new QCheckBox(restore_clean_gb);
    restore_clean_by_date->setChecked(false);
    restore_clean_by_date->setText(tr("Delete files older than "));
    restore_clean_by_date->setStatusTip(tr("Delete files older than"));
    clean_date_layout->addWidget(restore_clean_by_date);
    restore_clean_date = new QSpinBox (restore_clean_gb);
    //page->sync_interval->setStatusTip(tr("Set"));
    restore_clean_date->setMaximum(1440);
    restore_clean_date->setValue(30);
    restore_clean_date->setEnabled(false);
    clean_date_layout->addWidget(restore_clean_date);
    connect(restore_clean_by_date, SIGNAL(toggled(bool)), restore_clean_date, SLOT(setEnabled(bool)));
    QLabel * days_lbl = new QLabel (restore_clean_gb);
    days_lbl->setText(tr(" day(s)"));
    clean_date_layout->addWidget(days_lbl);
    clean_date_layout->addStretch();
    restore_clean_gb->addLayout(clean_date_layout, 1, 0);

    QHBoxLayout * clean_repeated_layout = new QHBoxLayout;
    restore_clean_repeated = new QCheckBox(restore_clean_gb);
    restore_clean_repeated->setChecked(false);
    restore_clean_repeated->setText(tr("Delete older versions of files, keep "));
    restore_clean_repeated->setStatusTip(tr("Delete older versions of files"));
    clean_repeated_layout->addWidget(restore_clean_repeated);
    restore_clean_files = new QSpinBox (restore_clean_gb);
    //page->sync_interval->setStatusTip(tr("Set"));
    restore_clean_files->setMaximum(1440);
    restore_clean_files->setValue(30);
    restore_clean_files->setEnabled(false);
    connect(restore_clean_repeated, SIGNAL(toggled(bool)), restore_clean_files, SLOT(setEnabled(bool)));
    clean_repeated_layout->addWidget(restore_clean_files);
    QLabel * versions_lbl = new QLabel (restore_clean_gb);
    versions_lbl->setText(tr(" latest versions"));
    clean_repeated_layout->addWidget(versions_lbl);
    clean_repeated_layout->addStretch();
    restore_clean_gb->addLayout(clean_repeated_layout, 2, 0);

    QHBoxLayout * clean_layout = new QHBoxLayout;
    clean_layout->addStretch();
    restore_clean = new QPushButton(restore_clean_gb);
    restore_clean->setText(tr("Clean"));
    restore_clean->setStatusTip(tr("Clean temporary files"));
    restore_clean->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/clean_temp_16.png")));
    connect(restore_clean, SIGNAL(released()), this, SLOT(cleanTemporary()));
    clean_layout->addWidget(restore_clean);
    restore_clean_gb->addLayout(clean_layout, 3, 0);
}

void MainWindow::cleanTemporary()
{
    if (restore_clean_selected->isChecked()) {
        bool a = false;
        for (int i = restore_list->count()-1; i>=0; --i) {
            if (restore_list->item(i)->checkState() == Qt::Checked) {
                a = true;
            }
        }
        if (!a) { QMessageBox::information(this, tr("Synkron"), tr("No files selected.")); return; }
    }
    QMap<QString, int> repeated_map; int deleted_files = 0;
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to delete temporary files?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        for (int i = restore_list->count()-1; i>=0; --i) {
            if (restore_clean_selected->isChecked() && restore_list->item(i)->checkState() == Qt::Unchecked) continue;
            bool straight_delete = false; bool repeated_only = false;
            if (restore_clean_by_date->isChecked()) {
                QStringList date = restore_list->item(i)->data(Qt::UserRole).toStringList().at(1).split("-")
                                                                            .at(0).split(".");
                int days_to = QDate(date.at(0).toInt(), date.at(1).toInt(), date.at(2).toInt())
                                                                .daysTo(QDate::currentDate());
                if (days_to < restore_clean_date->value()) repeated_only = true;
                else straight_delete = true;
            }
            if (restore_clean_repeated->isChecked() && !straight_delete) {
                if (repeated_map.value(restore_list->item(i)->data(Qt::UserRole).toStringList().at(2)) >= restore_clean_files->value()) {
                    deleteTempFile(restore_list->item(i)->data(Qt::UserRole).toStringList().at(3));
                    delete restore_list->item(i);
                    deleted_files++;
                } else {
                    repeated_map.insert(restore_list->item(i)->data(Qt::UserRole).toStringList().at(2), repeated_map.value(restore_list->item(i)->data(Qt::UserRole).toStringList().at(2))+1);
                }
                continue;
            }
            if (!repeated_only) {
                deleteTempFile(restore_list->item(i)->data(Qt::UserRole).toStringList().at(3));
                delete restore_list->item(i);
                deleted_files++;
            }
        }
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, tr("Synkron"), tr("%1 files deleted.").arg(deleted_files));
        break;
    case QMessageBox::No:
        return;
    default:
        break;
    }
}

void MainWindow::deleteTempFile(QString path)
{
    MTFile * file = new MTFile (path);
    if (file->exists()) {
        file->remove();
        QFileInfo info(path);
        QDir dir = info.dir();
        QString dirname = dir.dirName();
        dir.cdUp();
        dir.rmdir(dirname);
    }
    QStringList path_split = path.split("/");
    QString time = path_split.at(path_split.count()-2);
    temp_settings->beginGroup(time);
    temp_settings->remove(path_split.last());
    if (temp_settings->childGroups().count() == 0) {
        temp_settings->remove("");
    }
    temp_settings->endGroup();
    /*if (synchronised.contains(path)) {
        int n = synchronised.indexOf(path);
        synchronised.removeAt(n); synchronised.removeAt(n-1); synchronised.removeAt(n-2); synchronised.removeAt(n-3);
    }*/
}

void MainWindow::deleteRestoreItem()
{
    if (restore_list->currentItem()==0) return;
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to delete the selected temporary file?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        deleteTempFile(restore_list->currentItem()->data(Qt::UserRole).toStringList().at(3));
        delete restore_list->currentItem();
        break;
    case QMessageBox::No:
        return;
    default:
        break;
    }
}

void MainWindow::checkRestoreItem()
{
    if (restore_list->currentItem()==0) return;
    restore_list->currentItem()->setCheckState(restore_list->currentItem()->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

void MainWindow::blacklistRestoreItem()
{
    if (restore_list->currentItem()==0) return;
    to_black_list->setCheckState(to_black_list->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

void MainWindow::restoreListConMenu(QPoint pos)
{
    if (restore_list->currentItem()==0) return;
    QMenu * contextMenu = new QMenu(this);
    contextMenu->addAction(checkRestoreItemAction);
    contextMenu->addAction(restoreAction);
    contextMenu->addAction(deleteRestoreItemAction);
    if (!files_blacklist.contains(restore_list->currentItem()->data(Qt::UserRole).toStringList().at(2))) {
        blacklistRestoreItemAction->setText(tr("Add to blacklist"));
    } else {
        blacklistRestoreItemAction->setText(tr("Remove from blacklist"));
    }
    contextMenu->addAction(blacklistRestoreItemAction);
    contextMenu->move(pos);
    contextMenu->show();
}

void AbstractSyncPage::backupAndRemove(const QFileInfo * fi1, const QFileInfo * fi2, bool dont_backup)
{
    if (!fi2->absoluteDir().exists())
        return;

    if (fi1->isDir() && !fi1->isSymLink()) {
        backupAndRemoveDir(fi1->absoluteFilePath(), !dont_backup);
    } else {
        backupAndRemoveFile(*fi1, !dont_backup);
    }
}

void AbstractSyncPage::backupAndRemoveDir(QString dir_path, bool backup, bool add_table_item)
{
    int former_files = synced_files;
    QDir dir (dir_path);
    QFileInfoList entries;
    if (extensions.count()==0) {
        entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Hidden | QDir::Files | QDir::AllDirs, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    } else {
        entries = dir.entryInfoList(extensions.toList(), QDir::NoDotAndDotDot | QDir::Hidden | QDir::Files | QDir::AllDirs, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    }
    //deleted_files += entries.count();
    for (int i = 0; i < entries.count(); ++i) {
        if (!syncing) {
            if (add_table_item && synced_files!=former_files) {
                addTableItem(tr("%1 files deleted").arg(synced_files - former_files), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
            }
            return;
        }
        if (entries.at(i).isDir() && !entries.at(i).isSymLink()) {
            backupAndRemoveDir(entries.at(i).absoluteFilePath(), backup, false);
        } else {
            backupAndRemoveFile(entries.at(i), backup, false);
            qApp->processEvents();
        }
    }
    if (!syncing) {
        if (add_table_item && synced_files!=former_files) {
            addTableItem(tr("%1 files deleted").arg(synced_files - former_files), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
        }
        return;
    }
    QString dirname = dir.dirName();
    dir.cdUp();
    if (dir.rmdir(dirname)) {
        if (add_table_item) addTableItem(tr("Folder %1 with %2 files deleted").arg(getLabeledPath(dir_path)).arg(synced_files - former_files), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
        synced_files++;
    } else {
        if (add_table_item) {
            addTableItem(tr("Unknown error removing folder: %1").arg(getLabeledPath(dir_path)), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
        }
    }
    return;
}

void AbstractSyncPage::backupAndRemoveFile(QFileInfo file_info, bool backup, bool add_table_item)
{
    MTFile file (file_info.absoluteFilePath());
    QString previous_status_text = status_table_item->text();
    status_table_item->setText(tr("Removing file %1").arg(file_info.absoluteFilePath())); qApp->processEvents();
    if (backup) {
        QDir().mkpath(QString("%1/%2").arg(mp_parent->temp_path).arg(update_time));
        if (!file.copy(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(file_info.fileName()).arg(synced_files))) {
            errorCopyingFile(file_info.absoluteFilePath(), file.errorString(), true);
            return;
        }
        saveBackedUpFile(file_info);
        //mp_parent->synchronised << file_info.fileName() << update_time << file_info.absoluteFilePath();
        //mp_parent->synchronised << QString("%1/.Synkron/%2/%3.%4").arg(QDir::QDir::homePath()).arg(update_time).arg(file_info.fileName()).arg(synced_files);
    }
    if (!file.remove()) {
        if (add_table_item) errorRemovingFile(file_info.absoluteFilePath(), file.errorString());
        return;
    }
    if (add_table_item) addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_info.absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
    status_table_item->setText(previous_status_text); qApp->processEvents();
    synced_files++;
}

void MainWindow::setSelectGB()
{
    restore_select_gb->setTitle(tr("Advanced selection"));

    restore_select_last_sync = new QToolButton (restore_select_gb);
    restore_select_last_sync->setStatusTip(tr("Select files from the last synchronisation"));
    restore_select_last_sync->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/ok_16.png")));
    connect(restore_select_last_sync, SIGNAL(released()), this, SLOT(restoreSelLastSync()));
    restore_select_gb->addWidget(restore_select_last_sync, 0, 0);
    QLabel * sel_last_sync_lbl = new QLabel (restore_select_gb);
    sel_last_sync_lbl->setText(tr("Select files from the last synchronisation"));
    restore_select_gb->addWidget(sel_last_sync_lbl, 0, 1);

    restore_select_common_date = new QToolButton (restore_select_gb);
    restore_select_common_date->setStatusTip(tr("Select files with common date and time of synchronisation"));
    restore_select_common_date->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/ok_16.png")));
    connect(restore_select_common_date, SIGNAL(released()), this, SLOT(restoreSelCommonDate()));
    restore_select_gb->addWidget(restore_select_common_date, 1, 0);
    QHBoxLayout * select_common_layout = new QHBoxLayout;
    QLabel * select_common_lbl_1 = new QLabel (restore_select_gb);
    select_common_lbl_1->setText(tr("Select files synchronised on "));
    select_common_layout->addWidget(select_common_lbl_1);
    restore_select_dt_edit = new QDateTimeEdit(restore_select_gb);
    restore_select_dt_edit->setDisplayFormat("yyyy.MM.dd-hh:mm:ss");
    select_common_layout->addWidget(restore_select_dt_edit);
    select_common_layout->addStretch();
    restore_select_gb->addLayout(select_common_layout, 1, 1);
}

void MainWindow::restoreSelLastSync()
{
    QString last_date = restore_list->item(restore_list->count()-1)->data(Qt::UserRole).toStringList().at(1);
    for (int i = restore_list->count()-1; i >= 0; --i) {
        if (restore_list->item(i)->data(Qt::UserRole).toStringList().at(1) == last_date) {
            restore_list->item(i)->setCheckState(Qt::Checked);
        } else return;
    }
}

void MainWindow::restoreSelCommonDate()
{
    QString date_time = restore_select_dt_edit->dateTime().toString("yyyy.MM.dd-hh.mm.ss");
    for (int i = restore_list->count()-1; i >= 0; --i) {
        if (date_time == restore_list->item(i)->data(Qt::UserRole).toStringList().at(1)) {
            restore_list->item(i)->setCheckState(Qt::Checked);
        }
    }
}

void MainWindow::convertOldTempSettings(QStringList old_settings)
{
    QString temp_file_name;
    for (int i = 0; i < old_settings.count(); i+=4) {
        //at(i) - file name
        //at(i+1) - sync time
        //at(i+2) - old file path
        //at(i+3) - temp file path
        temp_settings->beginGroup(old_settings.at(i+1));
        temp_file_name = old_settings.at(i+3).split("/").last();
        temp_settings->beginGroup(temp_file_name);
        temp_settings->setValue("file_name", old_settings.at(i));
        temp_settings->setValue("old_file_path", old_settings.at(i+2));
        temp_settings->setValue("temp_file_path", old_settings.at(i+3));
        temp_settings->endGroup();
        temp_settings->endGroup();
    }
}

void MainWindow::loadTempSettings()
{
    QDir dir (temp_path);
    dir.mkpath(temp_path);
    /*dir.mkdir(".Synkron");
    dir.cd(".Synkron");*/
    QFile file(QString("%1/%2").arg(dir.absolutePath()).arg(".backup.syncdb"));
    if (!file.open(QFile::Append | QFile::Text)) {
        QMessageBox::critical(this, tr("Synkron"), tr("Cannot write file %1: %2").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    temp_settings = new QSettings (dir.absoluteFilePath(".backup.syncdb"), QSettings::IniFormat);
}

void MainWindow::readTempSettings()
{

}

int MainWindow::autoCleanTemp()
{
    QMap<QString, int> repeated_map;
    //int deleted_files = 0;
    QStringList to_be_deleted;
    QStringList time_groups = temp_settings->childGroups();
    for (int i = time_groups.count() - 1; i >= 0; --i) {
        temp_settings->beginGroup(time_groups.at(i));
        QStringList file_groups = temp_settings->childGroups();
        for (int n = 0; n < file_groups.count(); ++n) {
            temp_settings->beginGroup(file_groups.at(n));
            int rep = repeated_map.value(temp_settings->value("old_file_path").toString(), 0);
            if (rep >= restore_clean_auto_files->value()) {
                //QMessageBox::information(this, "", temp_settings->value("old_file_path").toString());
                if (!to_be_deleted.contains(temp_settings->value("temp_file_path").toString())) {
                    to_be_deleted << temp_settings->value("temp_file_path").toString();
                }
                /*deleteTempFile(temp_settings->value("temp_file_path").toString());
                deleted_files++;*/
            } else {
                repeated_map.insert(temp_settings->value("old_file_path").toString(), rep + 1);
            }
            temp_settings->endGroup();
        }
        temp_settings->endGroup();
    }
    //QMessageBox::information(this, "", temp_settings->value("temp_file_path").toString());
    for (int i = 0; i < to_be_deleted.count(); ++i) {
        deleteTempFile(to_be_deleted.at(i));
    }
    restore_list->clear();
    return to_be_deleted.count();
}
