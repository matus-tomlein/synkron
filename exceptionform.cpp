#include "exceptionform.h"
#include "ui_exceptionform.h"

#include "exceptions.h"
#include "exceptionbundle.h"
#include "exceptiongroup.h"

#include <QTreeWidgetItem>
#include <QFileDialog>

ExceptionForm::ExceptionForm(QTreeWidgetItem * nav_parent_item, Exceptions * exceptions, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExceptionForm)
{
    ui->setupUi(this);

    this->nav_parent_item = nav_parent_item;
    this->exceptions = exceptions;

    bundles_map = new QMap<QTreeWidgetItem *, ExceptionBundle *>;
    current_bundle = NULL;

    // Blacklist and filters items initialization +++
    QTreeWidgetItem * blacklist_group_item = new QTreeWidgetItem(ui->items_tree, QStringList(tr("Blacklist")));
    blacklist_group_item->setIcon(0, QIcon(":/images/blacklist.png"));
    blacklist_group_item->setExpanded(true);
    file_blacklist_group_item = new QTreeWidgetItem(blacklist_group_item, QStringList(tr("Files")));
    file_blacklist_group_item->setExpanded(true);
    folder_blacklist_group_item = new QTreeWidgetItem(blacklist_group_item, QStringList(tr("Folders")));
    folder_blacklist_group_item->setExpanded(true);
    filter_group_item = new QTreeWidgetItem(ui->items_tree, QStringList(tr("Filters")));
    filter_group_item->setExpanded(true);
    filter_group_item->setIcon(0, QIcon(":/images/filter16.png"));
    // Blacklist and filters items initialization ---

    QObject::connect(ui->bl_file_add_btn, SIGNAL(clicked()), this, SLOT(addBlacklistFile()));
    QObject::connect(ui->bl_folder_add_btn, SIGNAL(clicked()), this, SLOT(addBlacklistFolder()));
    QObject::connect(ui->filter_add_btn, SIGNAL(clicked()), this, SLOT(addFilter()));
    QObject::connect(ui->name_le, SIGNAL(textChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
    QObject::connect(ui->bl_file_browse_btn, SIGNAL(clicked()), this, SLOT(browseBlacklistFile()));
    QObject::connect(ui->bl_folder_browse_btn, SIGNAL(clicked()), this, SLOT(browseBlacklistFolder()));
    QObject::connect(ui->items_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));

    ui->items_tree->header()->setStretchLastSection(false);
    ui->items_tree->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->items_tree->header()->setResizeMode(1, QHeaderView::ResizeToContents);

    load();
}

ExceptionForm::~ExceptionForm()
{
    for (int i = ui->items_tree->topLevelItemCount() - 1; i >= 0; --i) {
        if (ui->items_tree->topLevelItem(i)->childCount()) {
            deleteChildItems(ui->items_tree->topLevelItem(i));
        }
        delete ui->items_tree->topLevelItem(i);
    }

    delete bundles_map;
    delete ui;
}

void ExceptionForm::deleteChildItems(QTreeWidgetItem * item)
{
    for (int i = item->childCount() - 1; i >= 0; --i) {
        if (item->child(i)->childCount()) {
            deleteChildItems(item->child(i));
        }
        delete item->takeChild(i);
    }
}

void ExceptionForm::changeEvent(QEvent *e)
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
  * Creates a new ExceptionBundle object as well as a trewidgetitem and adds them to the bundles_map map.
  */
void ExceptionForm::addBundle()
{
    ExceptionBundle * bundle = exceptions->addBundle();
    addBundle(bundle);
    displayBundle(bundle);
}

/**
  * Creates a navigation item and adds it with the bundle to the bundles map.
  */
void ExceptionForm::addBundle(ExceptionBundle * bundle)
{
    QTreeWidgetItem * item = new QTreeWidgetItem();
    nav_parent_item->addChild(item);

    item->setText(0, bundle->value("name").toString());
    item->setToolTip(0, bundle->value("name").toString());

    bundles_map->insert(item, bundle);
}

void ExceptionForm::navigationItemClicked(QTreeWidgetItem * item)
{
    if (!bundles_map->contains(item)) return;

    displayBundle(bundles_map->value(item));
}

void ExceptionForm::displayBundle(QTreeWidgetItem * item)
{
    displayBundle(bundles_map->value(item));
}

/**
  * Changes the current bundle items in the items tree.
  */
void ExceptionForm::displayBundle(ExceptionBundle * bundle)
{
    deleteChildItems(file_blacklist_group_item);
    deleteChildItems(folder_blacklist_group_item);
    deleteChildItems(filter_group_item);

    current_bundle = bundle;

    QList<ExceptionGroup *> * groups = bundle->groups();
    for (int i = 0; i < groups->count(); ++i) {
        int type = groups->at(i)->value("type").toInt();

        if (type == ExceptionGroup::FileBlacklist) {
            populateGroupItem(groups->at(i), file_blacklist_group_item);
        } else if (type == ExceptionGroup::FolderBlacklist) {
            populateGroupItem(groups->at(i), folder_blacklist_group_item);
        } else if (type == ExceptionGroup::Filters) {
            populateGroupItem(groups->at(i), filter_group_item);
        }
    }

    ui->name_le->setText(bundle->value("name").toString());
}

/**
  * Called when the current bundle changes. Puts items from the group into the item.
  */
void ExceptionForm::populateGroupItem(ExceptionGroup * group, QTreeWidgetItem * item)
{
    for (int i = 0; i < group->count(); ++i) {
        addChildToItem(group->at(i), item);
    }
}

void ExceptionForm::addChildToItem(const QString & s, QTreeWidgetItem * parent)
{
    QTreeWidgetItem * item = new QTreeWidgetItem;
    item->setText(0, s);
    item->setText(1, tr("Remove"));
    item->setIcon(1, QIcon(":/images/remove.png"));
    parent->addChild(item);
}

void ExceptionForm::addBlacklistFile()
{
    if (!current_bundle) return;
    ExceptionGroup * group = current_bundle->groupByType(ExceptionGroup::FileBlacklist);
    if (!group) return;

    QString path = QDir::cleanPath(ui->bl_file_path_le->text());
    if (path.isEmpty()) return;

    group->addItem(path);
    addChildToItem(path, file_blacklist_group_item);
}

void ExceptionForm::addBlacklistFolder()
{
    if (!current_bundle) return;
    ExceptionGroup * group = current_bundle->groupByType(ExceptionGroup::FolderBlacklist);
    if (!group) return;

    QString path = QDir::cleanPath(ui->bl_folder_path_le->text());
    if (path.isEmpty()) return;

    group->addItem(path);
    addChildToItem(path, folder_blacklist_group_item);
}

void ExceptionForm::addFilter()
{
    if (!current_bundle) return;
    ExceptionGroup * group = current_bundle->groupByType(ExceptionGroup::Filters);
    if (!group) return;

    QString path = ui->filter_extension_le->text();
    if (path.isEmpty()) return;

    group->addItem(path);
    addChildToItem(path, filter_group_item);
}

void ExceptionForm::nameChanged(const QString & name)
{
    if (!current_bundle) return;
    current_bundle->setValue("name", name);
    bundles_map->key(current_bundle)->setText(0, name);
}

void ExceptionForm::removeBundle()
{
   if (current_bundle) removeBundle(current_bundle);
}

void ExceptionForm::removeBundle(QTreeWidgetItem * item)
{
    if (bundles_map->contains(item)) removeBundle(bundles_map->value(item));
}

void ExceptionForm::removeBundle(ExceptionBundle * bundle)
{
    if (bundle->value("default").toBool() == true) return;

    if (current_bundle == bundle) current_bundle = NULL;
    delete bundles_map->key(bundle);
    exceptions->removeBundle(bundle);
}

void ExceptionForm::load()
{
    for (int i = 0; i < exceptions->count(); ++i) {
        addBundle(exceptions->at(i));
    }

    if (exceptions->count()) displayBundle(exceptions->at(0));
}

void ExceptionForm::browseBlacklistFile()
{
    ui->bl_file_path_le->setText(QFileDialog::getOpenFileName(this, tr("Choose a file"), QDir::homePath()));
}

void ExceptionForm::browseBlacklistFolder()
{
    ui->bl_folder_path_le->setText(QFileDialog::getExistingDirectory(
            this,
            "Choose a directory",
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void ExceptionForm::itemClicked(QTreeWidgetItem * item, int col)
{
    if (col == 1 && !item->childCount()) {
        ExceptionGroup * group = itemGroup(item->parent());
        if (group) {
            group->removeItem(item->text(0));
            delete item;
        }
    }
}

ExceptionGroup * ExceptionForm::itemGroup(QTreeWidgetItem * item)
{
    if (item == file_blacklist_group_item) return current_bundle->groupByType(ExceptionGroup::FileBlacklist);
    else if (item == folder_blacklist_group_item) return current_bundle->groupByType(ExceptionGroup::FolderBlacklist);
    else if (item == filter_group_item) return current_bundle->groupByType(ExceptionGroup::Filters);
    return NULL;
}

int ExceptionForm::count()
{
    return bundles_map->count();
}
