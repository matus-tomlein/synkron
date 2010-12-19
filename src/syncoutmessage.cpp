#include "syncoutmessage.h"
#include "folderactiongroup.h"

#include <QString>

SyncOutMessage::SyncOutMessage(SyncOutMessageAction message_type, FolderActionGroup * fag, bool error, const QString & error_str_arg)
{
    this->message_type = message_type;
    this->fag = fag;
    this->error = error;
    this->error_str = error_str_arg;
}

const QString & SyncOutMessage::firstString()
{
    return fag->first();
}

int SyncOutMessage::firstFolderId()
{
    return fag->firstId();
}

const QString & SyncOutMessage::secondString()
{
    return fag->last();
}

int SyncOutMessage::secondFolderId()
{
    return fag->lastId();
}
