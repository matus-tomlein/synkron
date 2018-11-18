/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2009 Matus Tomlein (matus.tomlein@gmail.com)

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
#include <QResizeEvent>
#include <QToolButton>
#include <QStyle>
#include <QEvent>

class ExtendedLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    ExtendedLineEdit(QWidget * parent = 0):
    QLineEdit(parent) {
        xle_btn_clear = new QToolButton(this);
        if (layoutDirection() == Qt::LeftToRight) {
            xle_btn_clear->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/clear.png")));
        } else {
            xle_btn_clear->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/clear_rtl.png")));
        }
        xle_btn_clear->setIconSize(QSize(13, 13));
        xle_btn_clear->setCursor(Qt::ArrowCursor);
        xle_btn_clear->setStyleSheet("QToolButton { border: none; padding: 0px; }");
        xle_btn_clear->hide();
        QObject::connect(xle_btn_clear, SIGNAL(clicked()), this, SLOT(clear()));
        QObject::connect(xle_btn_clear, SIGNAL(clicked()), this, SIGNAL(returnPressed()));
        QObject::connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(setClearButtonVisible(const QString &)));
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(xle_btn_clear->sizeHint().width() + frameWidth + 1));
        QSize minsize = minimumSizeHint();
        setMinimumSize(qMax(minsize.width(), xle_btn_clear->sizeHint().height() + frameWidth * 2 + 2),
                        qMax(minsize.height(), xle_btn_clear->sizeHint().height() + frameWidth * 2 + 2));
    };

protected:
    void resizeEvent(QResizeEvent *) {
        QSize s = xle_btn_clear->sizeHint();
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        if (layoutDirection() == Qt::LeftToRight) {
            xle_btn_clear->move(rect().right() - frameWidth - s.width(), (rect().bottom() + 1 - s.height())/2);
        } else {
            xle_btn_clear->move(frameWidth, (rect().bottom() + 1 - s.height())/2);
        }
    };
    void enterEvent(QEvent * event) {
		if (!hasFocus()) { selectAll(); setFocus(); }
		event->accept();
	};

private slots:
    void setClearButtonVisible(const QString & text) {
        xle_btn_clear->setVisible(!text.isEmpty());
    };

private:
    QToolButton * xle_btn_clear;
};

#endif // EXTENDEDLINEEDIT_H
