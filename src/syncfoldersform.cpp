#include "syncfoldersform.h"
#include "ui_syncfoldersform.h"

SyncFoldersForm::SyncFoldersForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncFoldersForm)
{
    ui->setupUi(this);
}

SyncFoldersForm::~SyncFoldersForm()
{
    delete ui;
}

void SyncFoldersForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
