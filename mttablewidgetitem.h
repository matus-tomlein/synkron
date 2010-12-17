#ifndef MTTABLEWIDGETITEM_H
#define MTTABLEWIDGETITEM_H

class SyncOutMessage;

#include <QTableWidgetItem>

class MTTableWidgetItem : public QTableWidgetItem
{
public:
    MTTableWidgetItem(SyncOutMessage *);

    void loadSyncOutMessage(SyncOutMessage *);

};

#endif // MTTABLEWIDGETITEM_H
