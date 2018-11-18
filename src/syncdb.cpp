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

void AbstractSyncPage::saveFolderDatabase(QString dir_path)
{
    if (dir_path.isEmpty()) return;
    QString status_text = status_table_item->text();
    status_table_item->setText(tr("Creating database")); qApp->processEvents();
    QDir dir (dir_path);
    if (!text_database) {
        if (!dir.cd(".synkron.sync")) {
            if (!dir.mkdir(".synkron.sync")) {
                status_table_item->setText(status_text);
                return;
            }
            dir.cd(".synkron.sync");
        }
        if (!dir.cd(tabNameText())) {
            if (!dir.mkdir(tabNameText())) {
                status_table_item->setText(status_text);
                return;
            }
            dir.cd(tabNameText());
        }
        saveFolderEntries(dir.absolutePath(), dir_path);
    } else {
        QFile file(QString("%1/%2").arg(dir.absolutePath()).arg(".synkron.syncdb"));
        MTDictionary other_tabs = getFolderDatabaseOfOtherTabs(file);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            addTableItem(tr("Cannot write file %1: %2").arg(file.fileName()).arg(file.errorString()), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
            status_table_item->setText(status_text); qApp->processEvents();
            return;
        }
        QTextStream write_sfile(&file);
        write_sfile.setCodec("UTF-8");
        for (int i = 0; i < other_tabs.count(); ++i) {
            if (other_tabs.key(i).startsWith("<:?:>")) {
                write_sfile << other_tabs.key(i) << endl;
            } else {
                write_sfile << other_tabs.key(i) << "<->" << other_tabs.value(i) << endl;
            }
        }
        MTDictionary entry_list = getEntryList(dir.absolutePath(), dir.absolutePath());
        write_sfile << "<:?:>" << tabNameText() << endl;
        for (int i = 0; i < entry_list.count(); ++i) {
            write_sfile << entry_list.key(i) << "<->" << entry_list.value(i) << endl;
        }
    }
    status_table_item->setText(status_text);
}

MTDictionary AbstractSyncPage::getFolderDatabase(QString dir_path)
{
    //QStringList prop_files_list;
    MTDictionary prop_files_list;
    QFile file(QString("%1/%2").arg(dir_path).arg(".synkron.syncdb"));
    if (!file.exists()) return prop_files_list;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //QMessageBox::critical(this, tr("Save database"), tr("Cannot write file %1:\n%2.").arg(db_file_name).arg(file.errorString()));
        return prop_files_list;
    }
    QTextStream read_sfile(&file);
    read_sfile.setCodec("UTF-8");
    QString line; QString last_tab;
    while (!read_sfile.atEnd()) {
        line = read_sfile.readLine();
        if (line.startsWith("<:?:>")) {
            last_tab = line;
            last_tab.remove(0, 5);
            continue;
        }
        if (last_tab != tabNameText()) continue;
        //prop_files_list << line;
        if (line.split("<->").count() < 2) {
            prop_files_list.setValue(line.split("<->").first(), "");
        } else {
            prop_files_list.setValue(line.split("<->").first(), line.split("<->").last());
        }
    }
    if (prop_files_list.count() == 0) {
        if (!read_sfile.seek(0)) return prop_files_list;
        read_sfile.setCodec("UTF-8");
        while (!read_sfile.atEnd()) {
            line = read_sfile.readLine();
            if (line.startsWith("<:?:>")) break;
            //prop_files_list << line;
            if (line.split("<->").count() < 2) {
                prop_files_list.setValue(line.split("<->").first(), "");
            } else {
                prop_files_list.setValue(line.split("<->").first(), line.split("<->").last());
            }
        }
    }
    return prop_files_list;
}

void AbstractSyncPage::saveAllFolderDatabases()
{
    QStringList paths = syncFoldersList();
    for (int i = 0; i < paths.count(); ++i) {
        saveFolderDatabase(paths.at(i));
    }
}

MTDictionary AbstractSyncPage::getFolderDatabaseOfOtherTabs(QFile & file)
{
    MTDictionary other_tabs;
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream read_sfile(&file);
        read_sfile.setCodec("UTF-8");
        QString last_tab; QStringList used_tabs; QString line;
        while (!read_sfile.atEnd()) {
            line = read_sfile.readLine();
            if (line.startsWith("<:?:>")) {
                last_tab = line;
                last_tab.remove(0, 5);
                if (used_tabs.contains(last_tab)) {
                    last_tab = tabNameText();
                } else {
                    used_tabs << last_tab;
                }
            }
            if (last_tab == tabNameText()) continue;
            //other_tabs << line;
            if (line.split("<->").count() < 2) {
                other_tabs.setValue(line.split("<->").first(), "");
            } else {
                other_tabs.setValue(line.split("<->").first(), line.split("<->").last());
            }
        }
        file.close();
    }
    return other_tabs;
}

void AbstractSyncPage::deleteFolderDatabase(QString dir_path)
{
    if (dir_path.isEmpty()) return;
    QString status_text = status_table_item->text();
    status_table_item->setText(tr("Removing database")); qApp->processEvents();
    if (!text_database) {
        QDir dir (dir_path);
        if (!dir.cd(".synkron.sync")) {
            status_table_item->setText(status_text);
            return;
        }
        syncing = true;
        mp_parent->removeDir(dir.absoluteFilePath(tabNameText()));
        syncing = false;
    } else {
        QDir dir (dir_path);
        QFile file(QString("%1/%2").arg(dir.absolutePath()).arg(".synkron.syncdb"));
        MTDictionary other_tabs = getFolderDatabaseOfOtherTabs(file);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            addTableItem(tr("Cannot write file %1: %2").arg(file.fileName()).arg(file.errorString()), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
            status_table_item->setText(status_text);
            return;
        }
        QTextStream write_sfile(&file);
        write_sfile.setCodec("UTF-8");
        for (int i = 0; i < other_tabs.count(); ++i) {
            if (other_tabs.key(i).startsWith("<:?:>")) {
                write_sfile << other_tabs.key(i) << endl;
            } else {
                write_sfile << other_tabs.key(i) << "<->" << other_tabs.value(i) << endl;
            }
        }
    }
    status_table_item->setText(status_text);
}

void AbstractSyncPage::deleteAllFolderDatabases()
{
    QStringList paths = syncFoldersList();
    for (int i = 0; i < paths.count(); ++i) {
        deleteFolderDatabase(paths.at(i));
    }
}

MTDictionary AbstractSyncPage::getEntryList(QString dir_path, QString parent_dir)
{
    MTDictionary entry_list;
    QDir dir (parent_dir);
    QFileInfo fi;
    QStringList entries = getFolderEntries(dir_path);

    for (int i = 0; i < entries.count(); ++i) {
        fi.setFile(QString("%1/%2").arg(dir_path).arg(entries.at(i)));
        if (fi.fileName().startsWith(".synkron.sync")) continue;
        if (!fi.isDir() || fi.isSymLink()) {
            entry_list.setValue(dir.relativeFilePath(fi.absoluteFilePath()), fi.lastModified().toString(Qt::ISODate));
        } else {
            entry_list.setValue(dir.relativeFilePath(fi.absoluteFilePath()), "");
            entry_list << getEntryList(fi.absoluteFilePath(), parent_dir);
        }
    }
    return entry_list;
}

QStringList AbstractSyncPage::getFolderEntries(QString dir_path)
{
    QStringList entries;
#ifdef Q_WS_WIN
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    hFind = FindFirstFile(QString("%1/*").arg(dir_path).replace('/', '\\').toStdWString().c_str(), &FindFileData);
    QString file_name;
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
#ifdef UNICODE
            file_name = QString::fromUtf16((ushort*) FindFileData.cFileName);
#else
            file_name = QString::fromLocal8Bit(FindFileData.cFileName);
#endif
            if (file_name == "." || file_name == "..") continue;
            entries << file_name;
        } while (FindNextFile(hFind, &FindFileData) != 0);
    }
    FindClose(hFind);
#else
    entries = QDir(dir_path).entryList(QDir::NoDotAndDotDot | QDir::Hidden | QDir::Files | QDir::AllDirs, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
#endif
    return entries;
}

void AbstractSyncPage::saveFolderEntries(QString db_dir_path, QString sync_dir_path)
{
    QDir db_dir(db_dir_path);
    QDir sync_dir(sync_dir_path);
    MTStringSet file_names;
    /*QStringList entries = sync_dir.entryList(QDir::NoDotAndDotDot | QDir::Hidden | QDir::Files | QDir::AllDirs, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);*/
    QStringList entries = getFolderEntries(sync_dir.absolutePath());
    for (int n = 0; n < entries.count(); ++n) {
        file_names << entries.at(n);
    }
    entries = db_dir.entryList(QDir::NoDotAndDotDot | QDir::Hidden | QDir::Files | QDir::AllDirs, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
    for (int n = 0; n < entries.count(); ++n) {
        file_names << entries.at(n);
    }
    MTFile * file;
    QFileInfo * db_fi = new QFileInfo;
    QFileInfo * sync_fi = new QFileInfo;
    for (int i = 0; i < file_names.count(); ++i) {
        sync_fi->setFile(sync_dir.absoluteFilePath(file_names.at(i)));
        db_fi->setFile(db_dir.absoluteFilePath(file_names.at(i)));
        if (sync_fi->exists()) {
            if (sync_fi->fileName().startsWith(".synkron.sync")) continue;
            if (!sync_fi->isDir() || sync_fi->isSymLink()) {
                //fi->setFile(dir.absoluteFilePath(entries.at(i).fileName()));
                if (!db_fi->exists() || sync_fi->lastModified() != db_fi->lastModified()) {
                    file = new MTFile(db_fi->absoluteFilePath());
                    file->openAndTouch(sync_fi->absoluteFilePath());
                    delete file;
                }
            } else {
                if (!db_fi->exists() && !db_dir.mkdir(sync_fi->fileName())) continue;
                saveFolderEntries(db_fi->absoluteFilePath(), sync_fi->absoluteFilePath());
            }
        } else {
            if (!db_fi->isDir() || db_fi->isSymLink()) {
                file = new MTFile(db_fi->absoluteFilePath());
                file->remove();
                delete file;
            } else {
                mp_parent->removeDir(db_fi->absoluteFilePath());
            }
        }
    }
    delete sync_fi;
    delete db_fi;
}

bool AbstractSyncPage::isInGroupDatabase(QString file_name)
{
    QString sync_folder_path;
    QStringList s_folders_list = currentSyncFoldersList();
    for (int i = 0; i < s_folders_list.count(); ++i) {
        if (file_name.startsWith(s_folders_list.at(i))) {
            sync_folder_path = s_folders_list.at(i);
            break;
        }
    }
    if (sync_folder_path == "") return false;
    if (!text_database) {
        file_name.remove(0, sync_folder_path.count());
        QFileInfo fi(QString("%1/.synkron.sync/%2%3").arg(sync_folder_path).arg(tabNameText()).arg(file_name));
        if (fi.exists()) return true;
    } else {
        MTDictionary prop_files_list = folder_prop_list_map.value(sync_folder_path);
        if (prop_files_list.contains(QDir(sync_folder_path).relativeFilePath(file_name))) return true;
        //if (prop_files_list.contains(file_name)) return true;
    }
    return false;
}

bool AbstractSyncPage::isInDatabase(QString file_name)
{
    //QDir dir;
    QString sync_folder_path;
    if (file_name.startsWith(syncFolder1Text())) {
        //dir.setPath(syncFolder1Text());
        sync_folder_path = syncFolder1Text();
    } else if (file_name.startsWith(syncFolder2Text())) {
        //dir.setPath(syncFolder2Text());
        sync_folder_path = syncFolder2Text();
    } else {
        return false;
    }
    if (!text_database) {
        file_name.remove(0, sync_folder_path.count());
        QFileInfo fi(QString("%1/.synkron.sync/%2%3").arg(sync_folder_path).arg(tabNameText()).arg(file_name));
        if (fi.exists()) return true;
    } else {
        MTDictionary prop_files_list = folder_prop_list_map.value(sync_folder_path);
        //if (prop_files_list.contains(dir.relativeFilePath(file_name))) return true;
        if (prop_files_list.contains(QDir(sync_folder_path).relativeFilePath(file_name))) return true;
    }
    return false;
}

void AbstractSyncPage::changeTabNameInDatabase(QString new_name, QString old_name)
{
    if (!text_database) {
        QStringList paths = syncFoldersList();
        for (int i = 0; i < paths.count(); ++i) {
            QDir dir (paths.at(i));
            if (!dir.cd(".synkron.sync")) continue;
            dir.rename(old_name, new_name);
        }
    } else {
        QStringList paths = syncFoldersList();
        for (int i = 0; i < paths.count(); ++i) {
            QDir dir(paths.at(i));
            QFile file(QString("%1/%2").arg(dir.absolutePath()).arg(".synkron.syncdb"));
            if (!file.exists()) return;
            if (!file.open(QFile::ReadOnly | QFile::Text)) {
                //QMessageBox::critical(this, tr("Save database"), tr("Cannot write file %1:\n%2.").arg(db_file_name).arg(file.errorString()));
                return;
            }
            QTextStream read_sfile(&file);
            read_sfile.setCodec("UTF-8");
            QStringList new_db;
            QString line; QString last_tab;
            while (!read_sfile.atEnd()) {
                line = read_sfile.readLine();
                if (line.startsWith("<:?:>")) {
                    last_tab = line;
                    last_tab.remove(0, 5);
                    if (last_tab == old_name) {
                        line = QString("<:?:>%1").arg(new_name);
                    }
                }
                new_db << line;
            }
            file.close();
            if (!file.open(QFile::WriteOnly | QFile::Text)) {
                //addTableItem(tr("Cannot write file %1: %2").arg(file.fileName()).arg(file.errorString()), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
                return;
            }
            QTextStream write_sfile(&file);
            write_sfile.setCodec("UTF-8");
            for (int n = 0; n < new_db.count(); ++n) {
                write_sfile << new_db.at(n) << endl;
            }
        }
    }
}

bool AbstractSyncPage::checkForCollision(QString path1, QString path2)
{
    int col1 = fileIsDifferentFromDB(path1);
    if (col1 == 0) return false;
    if (col1 == fileIsDifferentFromDB(path2)) {
        addTableItem(tr("Collision found between files: %1 and %2").arg(path1).arg(path2), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkCyan), QBrush(Qt::white));
        return true;
    }
    return false;
}

int AbstractSyncPage::fileIsDifferentFromDB(QString path)
{
    QString folder;
    QStringList s_folders_list = currentSyncFoldersList();
    for (int i = 0; i < s_folders_list.count(); ++i) {
        if (path.startsWith(s_folders_list.at(i))) {
            folder = s_folders_list.at(i);
            break;
        }
    }
    if (folder.isEmpty()) return -1;
    if (!text_database) {
        QString db_file_name = path.right(path.count() - folder.count());
        MTFileInfo fi_db(QString("%1/.synkron.sync/%2%3").arg(folder).arg(tabNameText()).arg(db_file_name));
        if (!fi_db.exists()) return -1;
        MTFileInfo fi(path);
        if (fi.lastModified().toString(Qt::ISODate) != fi_db.lastModified().toString(Qt::ISODate)) {
            return 1;
        }
    } else {
        MTDictionary dict = folder_prop_list_map.value(folder);
        if (!dict.contains(QDir(folder).relativeFilePath(path))) return -1;
        QString date_str = dict.value(dict.indexOfKey(QDir(folder).relativeFilePath(path)));
        if (date_str.isEmpty()) return 0;
        MTFileInfo file_info(path);
        if (file_info.lastModified().toString(Qt::ISODate) != date_str) {
            return 1;
        }
    }
    return 0;
}

void AbstractSyncPage::displayCollisions()
{
    QApplication::restoreOverrideCursor();
    QDialog * dialogue = new QDialog (this, Qt::Dialog);
    dialogue->setWindowModality(Qt::WindowModal);
    dialogue->setWindowTitle(tr("Collisions found"));
    QGridLayout * col_glayout = new QGridLayout(dialogue);
    col_glayout->setMargin(4); col_glayout->setSpacing(10);
    QLabel * clabel = new QLabel(dialogue);
    clabel->setText(tr("Collided files:"));
    col_glayout->addWidget(clabel, 0, 0);
    QTableWidget * dial_tw = new QTableWidget(0, 2, dialogue);
    dial_tw->setHorizontalHeaderLabels(QStringList() << tr("Newer files") << tr("Older files"));
    dial_tw->verticalHeader()->hide();
    dial_tw->setShowGrid(false);
#if QT_VERSION >= 0x050000
    dial_tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    dial_tw->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

    dial_tw->setLayoutDirection(Qt::LeftToRight);
    QCheckBox * newer; QCheckBox * older;
    for (int i = 0; i < collided.count(); ++i) {
        newer = new QCheckBox;
        newer->setText(collided.key(i));
        newer->setToolTip(collided.key(i));
        older = new QCheckBox;
        older->setText(collided.value(i));
        older->setToolTip(collided.value(i));
        new MTCheckBoxGroup(newer, older);
        dial_tw->insertRow(dial_tw->rowCount());
        dial_tw->setCellWidget(dial_tw->rowCount()-1, 0, newer);
        dial_tw->setCellWidget(dial_tw->rowCount()-1, 1, older);
    }
    dial_tw->resizeRowsToContents();
    col_glayout->addWidget(dial_tw, 1, 0);
    QPushButton * ok_button = new QPushButton(dialogue);
    ok_button->setText(tr("OK"));
    connect(ok_button, SIGNAL(released()), dialogue, SLOT(accept()));
    QPushButton * cancel_button = new QPushButton(dialogue);
    cancel_button->setText(tr("Cancel"));
    connect(cancel_button, SIGNAL(released()), dialogue, SLOT(reject()));
    QHBoxLayout * hbuttons = new QHBoxLayout;
    hbuttons->addStretch();
    hbuttons->addWidget(ok_button);
    hbuttons->addWidget(cancel_button);
    col_glayout->addLayout(hbuttons, 2, 0);
    dialogue->resize(500, 250);
    dialogue->show();
    switch (dialogue->exec()) {
    case 0:
        break;
    case 1:
        for (int i = 0; i < dial_tw->rowCount(); ++i) {
            if (((QCheckBox *) dial_tw->cellWidget(i, 0))->isChecked()) {
                copyFile(((QCheckBox *) dial_tw->cellWidget(i, 0))->text(), ((QCheckBox *) dial_tw->cellWidget(i, 1))->text(), true);
            } else if (((QCheckBox *) dial_tw->cellWidget(i, 1))->isChecked()) {
                copyFile(((QCheckBox *) dial_tw->cellWidget(i, 1))->text(), ((QCheckBox *) dial_tw->cellWidget(i, 0))->text(), true);
            }
        }
    default:
        break;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void AbstractSyncPage::propagatedStateChanged(bool checked)
{
    if (checked) {
        if (move->isChecked()) move->toggle();
        //if (clone_folder1->isChecked()) clone_folder1->click();
    }
    if (countSyncFolders() == 2) {
        if (!checked && (alert_collisions->isChecked() || propagate_deletions->isChecked())) return;
        move->setEnabled(!checked);
        //clone_folder1->setEnabled(!checked);
    }
}

void AbstractSyncPage::propagatedClicked(bool checked)
{
    if (mp_parent->isHidden() || loading) return;
    if (checked) {
        if (!alert_collisions->isChecked() || !propagate_deletions->isChecked()) {
            saveAllFolderDatabases();
        }
    } else {
        if (!alert_collisions->isChecked() && !propagate_deletions->isChecked()) {
            deleteAllFolderDatabases();
        }
    }
}

void SyncPage::useTextDatabase(bool use)
{
    if (mp_parent->isHidden() || loading) return;
    if (text_database == use) return;
    if (alert_collisions->isChecked() || propagate_deletions->isChecked()) {
        deleteAllFolderDatabases();
        text_database = use;
        saveAllFolderDatabases();
    } else text_database = use;
}
