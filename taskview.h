#ifndef TASKVIEW_H
#define TASKVIEW_H

class QTreeWidget;
class QTreeWidgetItem;
class TaskItemView;

template<class T>
class QList;

class TaskView
{
public:
    TaskView(QTreeWidget *);

private:
    QTreeWidget * tree;
    QList<TaskItemView *> * items;

    QTreeWidgetItem * syncs_tree_item;
    QTreeWidgetItem * schedules_tree_item;
};

#endif // TASKVIEW_H
