#include "taskview.h"

#include <QTreeWidget>

TaskView::TaskView(QTreeWidget * tree)
{
    this->tree = tree;

    syncs_tree_item = new QTreeWidgetItem(this->tree);
    syncs_tree_item->setText(0, QObject::tr("Syncs"));
    syncs_tree_item->setIcon(0, QIcon(":/images/Synchronise.png"));

    schedules_tree_item = new QTreeWidgetItem(this->tree);
    schedules_tree_item->setText(0, QObject::tr("Schedules"));
    schedules_tree_item->setIcon(0, QIcon(":/images/scheduler16.png"));
}
