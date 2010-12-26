#include "analyseform.h"
#include "ui_analyseform.h"

#include "analyseaction.h"
#include "folders.h"
#include "folder.h"
#include "folderactiongroup.h"
#include "syncexceptionbundle.h"
#include "syncfile.h"
#include "analysetreewidgetitem.h"

#include <QTimer>
#include <QMessageBox>

AnalyseForm::AnalyseForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalyseForm)
{
    ui->setupUi(this);
    current_level_item = NULL;

    ui->tree->setExpandsOnDoubleClick(false);
    QObject::connect(ui->tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(treeItemDoubleClicked(QTreeWidgetItem*,int)));
    QObject::connect(ui->tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(treeItemClicked(QTreeWidgetItem*,int)));
}

AnalyseForm::~AnalyseForm()
{
    delete ui;
}

void AnalyseForm::analyse(Folders * folders, SyncExceptionBundle * bundle)
{
    ui->tree->clear();
    sf_queue.clear();
    current_level_item = NULL;
    this->folders = folders;
    AnalyseAction * aa = new AnalyseAction(folders, bundle);

    QObject::connect(aa, SIGNAL(finished(SyncFile*)), this, SLOT(syncFileReceived(SyncFile*)), Qt::QueuedConnection);

    ui->folders_table->clearContents();
    ui->folders_table->setRowCount(0);
    for (int i = 0; i < folders->count(); ++i) {
        ui->folders_table->insertRow(ui->folders_table->rowCount());
        ui->folders_table->setItem(ui->folders_table->rowCount() - 1, 0, new QTableWidgetItem(folders->at(i)->label()));
        ui->folders_table->setItem(ui->folders_table->rowCount() - 1, 1, new QTableWidgetItem);
    }

    aa->start();
}

void AnalyseForm::syncFileReceived(SyncFile * sf)
{
    sf_queue.clear();

    loadSyncFile(sf);
}

void AnalyseForm::loadSyncFile(SyncFile * parent_sf)
{
    loadSyncFile(nextLevelItem(parent_sf));
}

void AnalyseForm::loadSyncFile(AnalyseTreeWidgetItem * root_item)
{
    SyncFile * parent_sf = root_item->syncFile();
    for (int i = 0; i < parent_sf->childCount(); ++i) {
        new AnalyseTreeWidgetItem(parent_sf->childAt(i), root_item);
    }

    sf_queue << parent_sf;
    root_item->setExpanded(true);
}

void AnalyseForm::removeItemChildren(AnalyseTreeWidgetItem * item)
{
    if (item) {
        for (int i = item->childCount() - 1; i >= 0; --i) {
            delete item->takeChild(i);
        }
    }
}

void AnalyseForm::treeItemDoubleClicked(QTreeWidgetItem * qitem, int)
{
    if (!sf_queue.count())
        return;

    AnalyseTreeWidgetItem * item = (AnalyseTreeWidgetItem *) qitem;

    if (item->childCount() == 0) {
        loadSyncFile(item->syncFile());
    } else {
        if (item == current_level_item)
            return;

        sf_queue.takeLast();
        while (current_level_item = (AnalyseTreeWidgetItem *) current_level_item->parent()) {
            removeItemChildren(current_level_item);
            sf_queue.takeLast();
            if (current_level_item == item) {
                loadSyncFile(item);
                break;
            }
        }
    }
}

AnalyseTreeWidgetItem * AnalyseForm::nextLevelItem(SyncFile * sf)
{
    if (!current_level_item) {
        current_level_item = new AnalyseTreeWidgetItem(sf, ui->tree);
    } else {
        removeItemChildren(current_level_item);
        current_level_item = new AnalyseTreeWidgetItem(sf, current_level_item);
    }
    return current_level_item;
}

void AnalyseForm::treeItemClicked(QTreeWidgetItem * item, int)
{
    updateSelectedInfo((AnalyseTreeWidgetItem *) item);
}

void AnalyseForm::updateSelectedInfo(AnalyseTreeWidgetItem * item)
{
    SyncFile * sf = item->syncFile();

    QStringList rel_path;
    for (int i = 1; i < sf_queue.count(); ++i) {
        if (sf == sf_queue.at(i))
            break;
        rel_path << sf_queue.at(i)->getName();
    }
    rel_path << sf->getName();
    ui->rel_path_le->clear();
    ui->rel_path_le->setText(rel_path.join("/"));

    ui->folder_chb->setChecked(sf->isDir());

    QTableWidgetItem * folder_item;
    for (int i = 0; i < folders->count(); ++i) {
        folder_item = ui->folders_table->item(i, 1);
        switch (sf->fileStatusInFolder(folders->at(i)->index())) {
        case SyncFile::OK:
            folder_item->setText(tr("OK"));
            break;

        case SyncFile::Obsolete:
            folder_item->setText(tr("Obsolete"));
            break;

        default:
            folder_item->setText(tr("Not found"));
            break;
        }
    }
}
