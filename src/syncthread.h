#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

class SyncAction;

#include <QThread>

class SyncThread : public QThread
{
    Q_OBJECT

public:
    SyncThread(SyncAction *);
    ~SyncThread();

    void run();

private:
    SyncAction * sa;
};

#endif // SYNCTHREAD_H
