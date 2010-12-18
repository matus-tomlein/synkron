#ifndef SYNCFOLDERSFORM_H
#define SYNCFOLDERSFORM_H

#include <QWidget>

namespace Ui {
    class SyncFoldersForm;
}

class SyncFoldersForm : public QWidget {
    Q_OBJECT
public:
    SyncFoldersForm(QWidget *parent = 0);
    ~SyncFoldersForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SyncFoldersForm *ui;
};

#endif // SYNCFOLDERSFORM_H
