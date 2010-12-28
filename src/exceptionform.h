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

#ifndef EXCEPTIONFORM_H
#define EXCEPTIONFORM_H

#include <QWidget>

class Exceptions;
class ExceptionBundle;
class ExceptionGroup;
class QTreeWidgetItem;

namespace Ui {
    class ExceptionForm;
}

class ExceptionForm : public QWidget {
    Q_OBJECT
public:
    ExceptionForm(QTreeWidgetItem *, Exceptions *, QWidget *parent = 0);
    ~ExceptionForm();

    int count();
    QMap<QTreeWidgetItem *, ExceptionBundle *> * bundles() { return bundles_map; }

public slots:
    void addBundle();
    void addBundle(ExceptionBundle *);
    void removeBundle();
    void removeBundle(QTreeWidgetItem *);
    void removeBundle(ExceptionBundle *);
    void navigationItemClicked(QTreeWidgetItem *);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ExceptionForm *ui;

    QTreeWidgetItem * nav_parent_item;
    Exceptions * exceptions;
    QMap<QTreeWidgetItem *, ExceptionBundle *> * bundles_map;
    ExceptionBundle * current_bundle;
    ExceptionGroup * itemGroup(QTreeWidgetItem *);

    QTreeWidgetItem * file_blacklist_group_item;
    QTreeWidgetItem * folder_blacklist_group_item;
    QTreeWidgetItem * filter_group_item;

    void displayBundle(QTreeWidgetItem *);
    void displayBundle(ExceptionBundle *);
    void deleteChildItems(QTreeWidgetItem *);
    void populateGroupItem(ExceptionGroup *, QTreeWidgetItem *);
    void addChildToItem(const QString &, QTreeWidgetItem *);
    void load();

private slots:
    void addBlacklistFile();
    void addBlacklistFolder();
    void browseBlacklistFile();
    void browseBlacklistFolder();
    void addFilter();
    void nameChanged(const QString &);
    void itemClicked(QTreeWidgetItem *, int);
};

#endif // EXCEPTIONFORM_H
