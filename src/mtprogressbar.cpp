#include "mtprogressbar.h"
#include <QMessageBox>

MTProgressBar::MTProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
}

void MTProgressBar::increaseValue()
{
    this->setValue(this->value() + 1);
}
