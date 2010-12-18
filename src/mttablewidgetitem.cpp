#include "mttablewidgetitem.h"
#include "syncoutmessage.h"

MTTableWidgetItem::MTTableWidgetItem(SyncOutMessage *sync_out_message) :
    QTableWidgetItem()
{
    loadSyncOutMessage(sync_out_message);
}

void MTTableWidgetItem::loadSyncOutMessage(SyncOutMessage * message)
{
    switch (message->getType()) {
    case SyncOutMessage::FileCopied:
        this->setText(QString("File %1 copied from %2").arg(message->firstString()).arg(message->secondString()));
        break;

    case SyncOutMessage::FileUpdated:
        this->setText(QString("File %1 updated from %2").arg(message->firstString()).arg(message->secondString()));
        break;

    case SyncOutMessage::FolderCreated:
        this->setText(QString("Folder %1 created").arg(message->firstString()));
        break;

    case SyncOutMessage::FileDeleted:
        this->setText(QString("File %1 deleted").arg(message->firstString()));
        break;

    case SyncOutMessage::FolderDeleted:
        this->setText(QString("Folder %1 deleted").arg(message->firstString()));
        break;

    }

}
