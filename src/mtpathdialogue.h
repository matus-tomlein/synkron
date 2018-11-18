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

#ifndef MTPATHDIALOGUE_H
#define MTPATHDIALOGUE_H

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>

class MTPathDialogue : public QWidget
{
    Q_OBJECT

public:
    MTPathDialogue() { parent = 0; };
    MTPathDialogue(QWidget * parent_w) { parent = parent_w; };

public slots:
    QString getPath(bool is_folder, QString default_text = QString()) {
        folder = is_folder;
        dialogue = new QDialog (parent, Qt::Dialog);
        dialogue->setWindowModality(Qt::WindowModal);
        dialogue->setWindowTitle(folder ? tr("Enter folder path") : tr("Enter file path"));
        QGridLayout * dial_glayout = new QGridLayout (dialogue);
        dial_glayout->setMargin(4); dial_glayout->setSpacing(10);
        QLabel * dial_label = new QLabel (dialogue);
        dial_label->setText(folder ? tr("Enter folder path:") : tr("Enter file path:"));
        dial_glayout->addWidget(dial_label, 0, 0);
        dial_le = new QLineEdit (dialogue);
        dial_le->setText(default_text);
        dial_glayout->addWidget(dial_le, 1, 0);
        QHBoxLayout * hlayout = new QHBoxLayout;
        QPushButton * dial_browse = new QPushButton (dialogue);
        dial_browse->setText(tr("Browse"));
        hlayout->addWidget(dial_browse);
        hlayout->addStretch();
        QPushButton * dial_ok = new QPushButton (dialogue);
        dial_ok->setText(tr("OK"));
        dial_ok->setDefault(true);
        hlayout->addWidget(dial_ok);
        QPushButton * dial_cancel = new QPushButton (dialogue);
        dial_cancel->setText(tr("Cancel"));
        hlayout->addWidget(dial_cancel);
        dial_glayout->addLayout(hlayout, 2, 0);
        connect(dial_cancel, SIGNAL(released()), dialogue, SLOT(reject()));
        connect(dial_ok, SIGNAL(released()), dialogue, SLOT(accept()));
        connect(dial_browse, SIGNAL(released()), this, SLOT(browse()));
        switch (dialogue->exec()) {
            case 0: // Cancel
                return "";
                break;
            case 1: // OK
                return QDir(dial_le->text()).path();
                break;
        }
        return "";
    };

    void browse() {
        QString path;
        if (folder) {
            path = QFileDialog::getExistingDirectory(
                        dialogue,
                        "Choose a directory",
                        QDir::homePath(),
                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        } else {
            path = QFileDialog::getOpenFileName(dialogue, tr("Choose a file"), QDir::homePath());
        }
        if (!path.isEmpty()) {
            dial_le->setText(path);
        }
    };

private:
    QWidget * parent;
    QDialog * dialogue;
    QLineEdit * dial_le;
    bool folder;
};

#endif // MTPATHDIALOGUE_H
