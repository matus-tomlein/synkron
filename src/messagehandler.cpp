#include "messagehandler.h"

#include "mttablewidgetitem.h"
#include "syncoutmessage.h"
#include "folders.h"
#include "folder.h"

#include <QHeaderView>

MessageHandler::MessageHandler(Folders * folders, QTableWidget * log_tbl)
{
    this->folders = folders;
    this->log_tbl = log_tbl;

    this->log_tbl->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

void MessageHandler::logMessage(SyncOutMessage * msg)
{
    Folder * folder;
    QString rel_path;
    QStringList msg_labels;
    for (int i = 0; i < msg->folderCount(); ++i) {
        folder = folders->byId(msg->folderIdAt(i));
        if (!folder)
            continue;
        msg_labels << folder->label();

        if (!i)
            rel_path = msg->stringAt(i).mid(folder->path().size());
        //msg->labelFolder(i, folder->path().size(), folder->label());
    }

    addLogItem(msg, rel_path, msg_labels);

    delete msg;
}

void MessageHandler::addLogItem(SyncOutMessage * msg, const QString & rel_path, const QStringList & msg_labels)
{
    QString action;
    QString folders_item_str;
    log_tbl->insertRow(log_tbl->rowCount());
    log_tbl->setItem(log_tbl->rowCount() - 1, 0, new QTableWidgetItem(rel_path));

    switch (msg->getType()) {
    case SyncOutMessage::FileCopied:
        action = tr("Copied");
        folders_item_str = tr("From %1 to %2").arg(msg_labels.first()).arg(msg_labels.last());
        break;

    case SyncOutMessage::FileUpdated:
        action = tr("Updated");
        folders_item_str = tr("From %1 to %2").arg(msg_labels.first()).arg(msg_labels.last());
        break;

    case SyncOutMessage::FolderCreated:
        action = tr("Created");
        folders_item_str = tr("In %1").arg(msg_labels.first());
        break;

    case SyncOutMessage::FileDeleted:
    case SyncOutMessage::FolderDeleted:
        action = tr("Deleted");
        folders_item_str = tr("From %1").arg(msg_labels.first());
        break;
    }

    log_tbl->setItem(log_tbl->rowCount() - 1, 1, new QTableWidgetItem(action));
    log_tbl->setItem(log_tbl->rowCount() - 1, 2, new QTableWidgetItem(folders_item_str));
}
