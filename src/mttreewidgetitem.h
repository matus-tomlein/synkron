#ifndef MTTREEWIDGETITEM_H
#define MTTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class QString;

class MTTreeWidgetItem : public QTreeWidgetItem
{
public:
    MTTreeWidgetItem(QTreeWidgetItem * parent, const QStringList & strings):
    QTreeWidgetItem(parent, strings) {};

public slots:
    void setFirstColumnText(const QString &);
};

#endif // MTTREEWIDGETITEM_H
