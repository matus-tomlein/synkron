#include "syncthread.h"

#include "syncaction.h"

SyncThread::SyncThread(SyncAction * sa)
    : QThread()
{
    this->sa = sa;
    start();
    moveToThread(this);
}

SyncThread::~SyncThread()
{
    delete sa;
}

void SyncThread::run()
{
    QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()), Qt::DirectConnection);
    sa->sync();
}
