#ifndef SYNCOUTMESSAGE_H
#define SYNCOUTMESSAGE_H

#include <QString>

class SyncOutMessage
{
public:
    enum SyncOutMessageAction {
        FileCopied,
        FileUpdated,
        FolderCreated,
        FileDeleted,
        FolderDeleted
    };

    SyncOutMessage(SyncOutMessageAction, const QString &, const QString & = QString(), bool = false, const QString & = QString());
    //SyncOutMessage(SyncOutMessageAction, const QString &, bool = false);
    //SyncOutMessage(SyncOutMessageAction, const QString &, const QString &, bool, const QString &); // 1 path, error
    //SyncOutMessage(SyncOutMessageAction, const QString &, bool, const QString &); // 2 paths, error

    QString formatMessage();
    SyncOutMessageAction getType() { return message_type; }
    bool isErrorMessage() { return error; }
    const QString & firstString() { return str1; }
    const QString & secondString() { return str2; }
    const QString & errorString() { return error_str; }

private:
    QString str1;
    QString str2;
    QString error_str;
    bool error;
    SyncOutMessageAction message_type;
};

#endif // SYNCOUTMESSAGE_H
