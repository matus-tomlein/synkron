#include "syncform.h"
#include "ui_syncform.h"

#include "mtprogressbar.h"
#include "folders.h"
#include "syncexceptionbundle.h"
#include "syncaction.h"
#include "messagehandler.h"
#include "abstractsyncpage.h"

#include <QMessageBox>

SyncForm::SyncForm(MTProgressBar * progress_bar, AbstractSyncPage * page, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncForm)
{
    ui->setupUi(this);

    this->progress_bar = progress_bar;
    this->page = page;

    msg_handler = new MessageHandler(page->foldersObject(), ui->sync_log_table);

    ui->sync_log_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

SyncForm::~SyncForm()
{
    delete ui;
}

void SyncForm::startSync(SyncAction * sa)
{
    progress_bar->setHidden(false);
    progress_bar->setValue(0);

    if (!sa)
        sa = new SyncAction(page->foldersObject()->folderActionGroup(), page->syncExceptionBundle());

    QObject::connect(sa, SIGNAL(messageBox(QString)), this, SLOT(showMessageBox(QString)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(filesCounted(int)), progress_bar, SLOT(setMaximum(int)), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(anotherItemChecked()), progress_bar, SLOT(increaseValue()), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(finished()), this, SLOT(syncFinished()), Qt::QueuedConnection);
    QObject::connect(sa, SIGNAL(syncOutMessage(SyncOutMessage*)), msg_handler, SLOT(logMessage(SyncOutMessage*)), Qt::QueuedConnection);

    sa->start();
}

void SyncForm::startSync(SyncFile * sf, FolderActionGroup * fag)
{
    startSync(new SyncAction(fag, page->syncExceptionBundle(), sf));
}

void SyncForm::syncFinished()
{
    progress_bar->setHidden(true);
}

void SyncForm::showMessageBox(const QString s)
{
    QMessageBox::information(NULL, "Synkron", s);
}
