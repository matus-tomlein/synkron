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

#ifndef SYNCFORM_H
#define SYNCFORM_H

#include <QWidget>

class MTProgressBar;
class AbstractSyncPage;
class FolderActionGroup;
class SyncFile;
class MessageHandler;
class SyncAction;

namespace Ui {
    class SyncForm;
}

class SyncForm : public QWidget
{
    Q_OBJECT

public:
    explicit SyncForm(MTProgressBar *, AbstractSyncPage *, QWidget *parent = 0);
    ~SyncForm();

    void startSync(SyncAction * = NULL);
    void startSync(SyncFile *, FolderActionGroup *);

private slots:
    void syncFinished(int, int);
    void showMessageBox(const QString = QString());

private:
    Ui::SyncForm *ui;

    AbstractSyncPage * page;
    MTProgressBar * progress_bar;
    MessageHandler * msg_handler;
};

#endif // SYNCFORM_H
