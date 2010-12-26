#ifndef SYNCFORM_H
#define SYNCFORM_H

#include <QWidget>

class MTProgressBar;
class Folders;
class SyncExceptionBundle;
class MessageHandler;

namespace Ui {
    class SyncForm;
}

class SyncForm : public QWidget
{
    Q_OBJECT

public:
    explicit SyncForm(MTProgressBar *, Folders *, QWidget *parent = 0);
    ~SyncForm();

    void startSync(SyncExceptionBundle *);

private slots:
    void syncFinished();
    void showMessageBox(const QString = QString());

private:
    Ui::SyncForm *ui;

    Folders * folders;
    MTProgressBar * progress_bar;
    MessageHandler * msg_handler;
};

#endif // SYNCFORM_H
