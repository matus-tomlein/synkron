#include "mttreewidgetitem.h"

#include <QString>

void MTTreeWidgetItem::setFirstColumnText(const QString & text)
{
    this->setText(0, text);
}
