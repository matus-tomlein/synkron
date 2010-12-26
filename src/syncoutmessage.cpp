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

void SyncOutMessage::labelFolder(int f_i, int length, const QString & label)
{
    QString * str = &((*fag)[f_i]);
    str->remove(0, length);
    str->prepend(label);
}

int SyncOutMessage::folderCount()
{
    return fag->count();
}

int SyncOutMessage::folderIdAt(int i)
{
    return fag->idAt(i);
}

const QString & SyncOutMessage::stringAt(int i)
{
    return fag->at(i);
}
