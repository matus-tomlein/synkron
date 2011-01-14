#ifndef RESTOREFORM_H
#define RESTOREFORM_H

#include <QWidget>

class QTreeWidgetItem;

class BackupHandler;

namespace Ui {
    class RestoreForm;
}

class RestoreForm : public QWidget
{
    Q_OBJECT

public:
    explicit RestoreForm(BackupHandler *, QWidget *parent = 0);
    ~RestoreForm();

public slots:
    void reload();

private slots:
    void itemExpanded(QTreeWidgetItem *);

private:
    Ui::RestoreForm *ui;
    BackupHandler * backup_handler;
};

#endif // RESTOREFORM_H
