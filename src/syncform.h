#ifndef SYNCFORM_H
#define SYNCFORM_H

#include <QWidget>

class MTProgressBar;
class AbstractSyncPage;
class FolderActionGroup;
class SyncFile;
class MessageHandler;
class SyncAction;

namespace Ui {
    class SyncForm;
}

class SyncForm : public QWidget
{
    Q_OBJECT

public:
    explicit SyncForm(MTProgressBar *, AbstractSyncPage *, QWidget *parent = 0);
    ~SyncForm();

    void startSync(SyncAction * = NULL);
    void startSync(SyncFile *, FolderActionGroup *);

private slots:
    void syncFinished();
    void showMessageBox(const QString = QString());

private:
    Ui::SyncForm *ui;

    AbstractSyncPage * page;
    MTProgressBar * progress_bar;
    MessageHandler * msg_handler;
};

#endif // SYNCFORM_H
