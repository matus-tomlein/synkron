#include "backupaction.h"
#include "mtfile.h"

#include <QString>
#include <QDateTime>

BackupAction::BackupAction(QString * tp)
{
    temp = new QDir(*tp);
    time_str = new QString(QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss"));
    num_backed_up = -1;
}

BackupAction::~BackupAction()
{
    emit commit();

    delete temp;
    delete time_str;
}

bool BackupAction::backupFile(MTFile * file)
{
    if (num_backed_up == -1) {
        if (!temp->mkdir(*time_str) || !temp->cd(*time_str)) {
            return false;
        }
        num_backed_up = 0;
    }

    if (!file->copy(temp->absoluteFilePath(QString("%3.%4")
                            .arg(QFileInfo(*file).fileName())
                            .arg(num_backed_up)))) {
        return false;
    }

    emit fileBackedUp(file->fileName(), num_backed_up, *time_str);

    num_backed_up++;
    return true;
}
