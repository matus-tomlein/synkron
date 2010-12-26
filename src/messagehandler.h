#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>

class MTTableWidgetItem;
class Folders;
class SyncOutMessage;
class QTableWidget;

class MessageHandler : public QObject
{
    Q_OBJECT

public:
    MessageHandler(Folders *, QTableWidget *);

public slots:
    void logMessage(SyncOutMessage *);

private:
    void addLogItem(SyncOutMessage *, const QString &, const QStringList &);

    Folders * folders;
    QTableWidget * log_tbl;
};

#endif // MESSAGEHANDLER_H
