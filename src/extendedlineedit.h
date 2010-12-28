/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2011 Matus Tomlein (matus.tomlein@gmail.com)

 Synkron is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Synkron is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Synkron; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#ifndef EXTENDEDLINEEDIT_H
#define EXTENDEDLINEEDIT_H

#include <QLineEdit>
class QResizeEvent;
class QToolButton;
class QStyle;
class QEvent;

class ExtendedLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    ExtendedLineEdit(QWidget * parent = 0);

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);

private slots:
    void setClearButtonVisible(const QString &);

private:
    QToolButton * xle_btn_clear;
};

#endif // EXTENDEDLINEEDIT_H
