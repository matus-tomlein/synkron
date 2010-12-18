#include "syncoutmessage.h"

#include <QString>

SyncOutMessage::SyncOutMessage(SyncOutMessageAction message_type, const QString & str1_arg, const QString & str2_arg, bool error, const QString & error_str_arg)
{
    this->message_type = message_type;
    this->str1 = str1_arg;
    this->str2 = str2_arg;
    this->error = error;
    this->error_str = error_str_arg;
}

QString SyncOutMessage::formatMessage()
{
    switch (this->message_type) {
    case FileCopied:
        return QString("File %1 copied from %2").arg(str1).arg(str2);

    case FileUpdated:
        return QString("File %1 updated from %2").arg(str1).arg(str2);

    case FolderCreated:
        return QString("Folder %1 created").arg(str1);

    case FileDeleted:
        return QString("File %1 deleted").arg(str1);

    case FolderDeleted:
        return QString("Folder %1 deleted").arg(str1);
    }

    return QString();
}
