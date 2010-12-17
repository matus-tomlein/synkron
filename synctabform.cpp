/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2010 Matus Tomlein (matus.tomlein@gmail.com)

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

#include "synctabform.h"
#include "ui_synctabform.h"
#include "syncfolderform.h"
#include "abstractsyncpage.h"
#include "folder.h"
#include "folders.h"
#include "syncadvancedview.h"
#include "syncaction.h"
#include "syncoutmessage.h"
#include "mttablewidgetitem.h"

#include <QList>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QProgressBar>

/**
  * i is the index of the tab displayed in the title.
  */
SyncTabForm::SyncTabForm(AbstractSyncPage * page, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncTabForm)
{
    ui->setupUi(this);

    folder_forms = new QList<SyncFolderForm *>;
    nav_item = NULL;
    this->page = page;

    progress_bar = new QProgressBar();
    progress_bar->setHidden(true);

    ui->sync_log_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QObject::connect(ui->add_folder_btn, SIGNAL(clicked()), this, SLOT(addFolder()));
    QObject::connect(ui->tab_name_le, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));
    QObject::connect(ui->advanced_btn, SIGNAL(clicked()), this, SLOT(toggleAdvanced()));
    QObject::connect(ui->sync_btn, SIGNAL(clicked()), this, SLOT(sync()));

    advanced_view = new SyncAdvancedView(page, ui->advanced_tree);
    load();
    makeReady();
    hideAdvanced();
}

SyncTabForm::~SyncTabForm()
{
    delete advanced_view;
    for (int n = folder_forms->count() - 1; n >= 0; --n) {
        delete folder_forms->takeAt(n);
    }
    delete folder_forms;
    delete ui;
}

int SyncTabForm::index()
{
    return page->index();
}

void SyncTabForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * Returns the title of the tab.
  */
QString SyncTabForm::title()
{
    return page->value("title").toString();
}

/**
  * Adds a new folder form (SyncFolderForm)
  */
void SyncTabForm::addFolder()
{
    int f_id = getHighestFolderId() + 1;
    addFolder(page->addFolder(f_id));
}

/**
  * Adds a new folder form (SyncFolderForm)
  */
void SyncTabForm::addFolder(Folder * folder)
{
    SyncFolderForm * folder_form = new SyncFolderForm(folder, this);
    folder_form->makeYourselfAdvancedTreeItems(ui->advanced_tree);
    QObject::connect(folder_form, SIGNAL(sigRemove(int)), this, SLOT(removeFolder(int)));

    ui->folders_vlayout->addWidget(folder_form);
    folder_forms->append(folder_form);
}

/**
 * Returns the highest id of all folders in the tab
 */
int SyncTabForm::getHighestFolderId()
{
    int f_id = 0;

    for (int n = 0; n < folder_forms->count(); ++n) {
        if (folder_forms->at(n)->index() > f_id) f_id = folder_forms->at(n)->index();
    }

    return f_id;
}

/**
  * Removes folder form with the given folder id
  */
void SyncTabForm::removeFolder(int f_id) {
    qApp->processEvents(QEventLoop::AllEvents);

    for (int n = 0; n < folder_forms->count(); ++n) {
        if (folder_forms->at(n)->index() == f_id) {
            delete folder_forms->takeAt(n);
            page->closeFolder(f_id);

            return;
        }
    }
}

/**
 * Assigns a navigation item to the form. The item name will be updated when it is changed in the form.
 */
void SyncTabForm::assignNavigationItem(QTreeWidgetItem * item)
{
    nav_item = item;
}

void SyncTabForm::nameChanged(QString name)
{
    if (nav_item) nav_item->setText(0, name);
    page->setValue("title", name);
}
/**
  * Loads folders and other settings from the SyncPage (on startup)
  */
void SyncTabForm::load()
{
    Folders * folders = page->foldersObject();
    for (int i = 0; i < folders->count(); ++i) {
        addFolder(folders->at(i));
    }

    ui->tab_name_le->setText(page->value("title").toString());
}

/**
  * This is called after all settings were loaded to make sure everything is ready for the user to use the tab
  * (like there are at least 2 folders...)
  */
void SyncTabForm::makeReady()
{
    while (folder_forms->count() < 2) addFolder();
}

/**
  * Show/hide the advanced options tree
  */
void SyncTabForm::toggleAdvanced()
{
    QList<int> sizes = ui->advanced_splitter->sizes();
    if (sizes.last() == 0) {
        sizes.clear();
        sizes << 1;
        sizes << 1;
    } else {
        sizes.clear();
        sizes << 1;
        sizes << 0;
    }
    ui->advanced_splitter->setSizes(sizes);
}

/**
  * Hides the advaced tree by resizing the splitter.
  */
void SyncTabForm::hideAdvanced()
{
    QList<int> sizes;

    sizes << 1;
    sizes << 0;

    ui->advanced_splitter->setSizes(sizes);
}

/**
  * Triggered by user. Starts the sync by creating SyncAction.
  */
void SyncTabForm::sync()
{
    progress_bar->setHidden(false);
    SyncAction * sa = new SyncAction(page->foldersObject(), page->syncExceptionBundle());
    QObject::connect(sa, SIGNAL(messageBox(QString)), this, SLOT(showMessageBox(QString)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(filesCounted(int)), progress_bar, SLOT(setMaximum(int)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(finished()), this, SLOT(syncFinished()));
    QObject::connect(sa, SIGNAL(syncOutMessage(SyncOutMessage*)), this, SLOT(syncOutMessage(SyncOutMessage*)));

    sa->start();
}

void SyncTabForm::showMessageBox(const QString s)
{
    QMessageBox::information(NULL, "", s);
}

QWidget * SyncTabForm::navigationItemWidget()
{
    QWidget * w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->addWidget(progress_bar);
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    layout->setSpacing(2);
    return w;
}

void SyncTabForm::syncFinished()
{
    progress_bar->setHidden(true);
}

void SyncTabForm::syncOutMessage(SyncOutMessage * msg)
{
    ui->sync_log_table->insertRow(ui->sync_log_table->rowCount());
    MTTableWidgetItem *item = new MTTableWidgetItem(msg);
    ui->sync_log_table->setItem(ui->sync_log_table->rowCount() - 1, 0, item);
    delete msg;
}
