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

#ifndef RESTOREFORM_H
#define RESTOREFORM_H

#include <QWidget>

class QTreeWidgetItem;

class BackupHandler;
class BackupDatabaseRecord;
class RestoreTreeWidgetItem;

namespace Ui {
    class RestoreForm;
}

class RestoreForm : public QWidget
{
    Q_OBJECT

public:
    explicit RestoreForm(BackupHandler *, QWidget *parent = 0);
    ~RestoreForm();

public slots:
    void reload();

private slots:
    void itemExpanded(QTreeWidgetItem *);
    void itemClicked(QTreeWidgetItem *, int);

    void recordActionFinished(BackupDatabaseRecord *);
    void recordActionFailed(BackupDatabaseRecord *);
    void allActionsFinished();

private:
    enum TreeColumnIndices { PathCol = 0, RestoreCol = 1, DeleteCol = 2 };

    Ui::RestoreForm *ui;
    BackupHandler * backup_handler;

    QList<RestoreTreeWidgetItem *> * items_in_action;
};

#endif // RESTOREFORM_H
