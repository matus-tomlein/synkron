#ifndef SYNCOUTMESSAGE_H
#define SYNCOUTMESSAGE_H

#include <QString>

class FolderActionGroup;

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

    SyncOutMessage(SyncOutMessageAction, FolderActionGroup *, bool = false, const QString & = QString());

    SyncOutMessageAction getType() { return message_type; }
    bool isErrorMessage() { return error; }
    const QString & firstString();
    int firstFolderId();
    const QString & secondString();
    int secondFolderId();
    const QString & errorString() { return error_str; }

private:
    QString error_str;
    FolderActionGroup * fag;
    bool error;
    SyncOutMessageAction message_type;
};

#endif // SYNCOUTMESSAGE_H
