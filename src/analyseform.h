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

#ifndef ANALYSEFORM_H
#define ANALYSEFORM_H

#include <QWidget>

class Folders;
class SyncExceptionBundle;
class AnalyseFile;
class SyncFile;
class AnalyseTreeWidgetItem;
class FolderActionGroup;
class AbstractSyncPage;

class QTreeWidgetItem;

namespace Ui {
    class AnalyseForm;
}

class AnalyseForm : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyseForm(AbstractSyncPage *, QWidget *parent = 0);
    ~AnalyseForm();

    void analyse();

private slots:
    void syncFileReceived(AnalyseFile *);
    void treeItemDoubleClicked(QTreeWidgetItem *, int);
    void treeItemSelectionChanged();
    void updateSelectedInfo(AnalyseTreeWidgetItem *);
    void openSelected(QAction *);
    void aboutToShowOpenMenu();
    void blacklistSelected(QAction *);
    void aboutToShowBlacklistMenu();
    void syncSelected();

private:
    void loadSyncFile(AnalyseFile *);
    void loadSyncFile(AnalyseTreeWidgetItem *);
    void removeItemChildren(AnalyseTreeWidgetItem *);
    AnalyseTreeWidgetItem * nextLevelItem(AnalyseFile *);
    QStringList relativePath(AnalyseFile *);

    Ui::AnalyseForm *ui;

    AbstractSyncPage * page;
    QList<AnalyseFile *> sf_queue;
    AnalyseTreeWidgetItem * current_level_item;
    AnalyseFile * current_sf;

signals:
    void syncSig(SyncFile *, FolderActionGroup *);
};

#endif // ANALYSEFORM_H
