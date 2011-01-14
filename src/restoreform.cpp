#include "restoreform.h"
#include "ui_restoreform.h"

#include "backuphandler.h"

RestoreForm::RestoreForm(BackupHandler * backup_handler, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RestoreForm)
{
    ui->setupUi(this);

    this->backup_handler = backup_handler;

    QObject::connect(ui->tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(itemExpanded(QTreeWidgetItem*)));
}

RestoreForm::~RestoreForm()
{
    delete ui;
}

void RestoreForm::reload()
{
    QStringList * new_dates = backup_handler->newDates();

    QTreeWidgetItem * item;
    for (int i = 0; i < new_dates->count(); ++i) {
        item = new QTreeWidgetItem(ui->tree);
        item->setText(0, new_dates->at(i));
        item->setData(0, Qt::UserRole, new_dates->at(i));
        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        ui->tree->addTopLevelItem(item);
    }

    delete new_dates;
}

void RestoreForm::itemExpanded(QTreeWidgetItem *)
{

}
