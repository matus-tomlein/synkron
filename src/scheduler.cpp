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

#include "mainwindow.h"

void MainWindow::addSchedule()
{
    addSchedule("Untitled schedule");
    tw_schedules->setCurrentCell(tw_schedules->rowCount() - 1, 0);
}

SyncSchedule * MainWindow::addSchedule(QString name)
{
    QTableWidgetItem * item = new QTableWidgetItem;
    setScheduleStatusOn(false, item);
    tw_schedules->insertRow(tw_schedules->rowCount());
    tw_schedules->setRowHeight(tw_schedules->rowCount() - 1, 16);
    tw_schedules->setItem(tw_schedules->rowCount() - 1, 1, item);
    item = new QTableWidgetItem;
    item->setText(name);
    tw_schedules->setItem(tw_schedules->rowCount() - 1, 0, item);
    SyncSchedule * schedule = new SyncSchedule (this);
    schedule->scheduling = false;
    schedule->timing_tab_index = 0;
    schedule->parent_item = item;
    QObject::connect(schedule, SIGNAL(sigsched(QWidget *)), this, SLOT(sync(QWidget *)));
    item_sched_map[item] = schedule;
    return schedule;
}

void MainWindow::removeSchedule()
{
    if(tw_schedules->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No schedule selected.")); return; }
    int m = tw_schedules->currentRow(); tw_schedules->clearSelection(); schedGroupBox->setEnabled(false); sched_removed = true;
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove the selected schedule from the list?"));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        if (item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0))->scheduling==true) break;
        delete item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
        item_sched_map.remove(tw_schedules->item(m, 0));
        tw_schedules->removeRow(m);
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    } sched_removed = false;
    if (tw_schedules->rowCount()!=0) {
        if (tw_schedules->currentRow()==0) scheduleActivated(0, 0, 0, 0);
        tw_schedules->setCurrentCell(0, 0);
        schedGroupBox->setEnabled(true);
    }
}

void MainWindow::scheduleActivated(int, int, int previousRow, int)
{
    if (sched_removed) return;
    if (schedGroupBox->isEnabled()) saveSchedSettings(previousRow);
    activateSchedule();
}

void MainWindow::activateSchedule()
{
    if (tw_schedules->currentItem()==0) {
        if (tw_schedules->rowCount()!=0) tw_schedules->setCurrentCell(0, 0);
        else return;
    }
    int row = tw_schedules->currentRow();
    schedGroupBox->setEnabled(true);
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(row, 0));
    sched_name->setText(tw_schedules->item(row, 0)->text());
    sched_interval_spin->setValue(schedule->periodical_interval);
    timing_tabWidget->setCurrentIndex(schedule->timing_tab_index);
    sched_tab_lw->clear();
    sched_multitab_lw->clear();
    sched_time_lw->clear();
    sched_sync_dates->clear();
    QListWidgetItem * litem;
    //QMapIterator<QWidget *, SyncPage *> i(tabs);
    for (int i = 0; i < tabWidget->count(); ++i) {
        litem = new QListWidgetItem (tabWidget->tabText(i));
        if (schedule->sched_tab_list.contains(tabWidget->tabText(i))) litem->setCheckState(Qt::Checked);
        else litem->setCheckState(Qt::Unchecked);
        sched_tab_lw->addItem(litem);
    }
    for (int i = 0; i < multi_tabWidget->count(); ++i) {
        litem = new QListWidgetItem (multi_tabWidget->tabText(i));
        if (schedule->sched_multitab_list.contains(multi_tabWidget->tabText(i))) litem->setCheckState(Qt::Checked);
        else litem->setCheckState(Qt::Unchecked);
        sched_multitab_lw->addItem(litem);
    }
    for (int n = 0; n < schedule->sched_time_list.count(); ++n) {
        litem = new QListWidgetItem(schedule->sched_time_list.at(n));
        if (schedule->sched_checked_time_list.contains(schedule->sched_time_list.at(n))) litem->setCheckState(Qt::Checked);
        else litem->setCheckState(Qt::Unchecked);
        sched_time_lw->addItem(litem);
    }
    bool show_dates = false;
    for (int n = 0; n < schedule->sched_date_list.count(); ++n) {
        litem = new QListWidgetItem(schedule->sched_date_list.at(n));
        if (schedule->sched_checked_date_list.contains(schedule->sched_date_list.at(n))) {
            litem->setCheckState(Qt::Checked); show_dates = true;
        }
        else litem->setCheckState(Qt::Unchecked);
        sched_sync_dates->addItem(litem);
    }
    for (int n = 0; n < sched_days_lw->count(); ++n) {
        sched_days_lw->item(n)->setCheckState(Qt::Checked);
    }
    for (int n = 0; n < schedule->sched_unchecked_days_list.count(); ++n) {
        sched_days_lw->item(schedule->sched_unchecked_days_list.at(n).toInt())->setCheckState(Qt::Unchecked);
        show_dates = true;
    }
    sched_dates_agb->setChecked(show_dates);
    enableSchedule(row);
    setScheduleStatusOn(schedule->scheduling, tw_schedules->item(row, 1));
}

void MainWindow::addSchedTime()
{
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (schedule->sched_time_list.contains(sched_time_edit->time().toString("H:mm"))) return;
    QListWidgetItem * item = new QListWidgetItem (sched_time_edit->time().toString("H:mm"));
    item->setCheckState(Qt::Checked);
    sched_time_lw->addItem(item);
    schedule->sched_time_list << sched_time_edit->time().toString("H:mm");
    schedule->sched_checked_time_list << sched_time_edit->time().toString("H:mm");
}

void MainWindow::removeSchedTime()
{
    if (sched_time_lw->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No sync time selected.")); return; }
    SyncSchedule * schedule;
    schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (schedule->sched_time_list.contains(sched_time_lw->currentItem()->text())) {
        schedule->sched_time_list.removeAt(schedule->sched_time_list.indexOf(sched_time_lw->currentItem()->text()));
    }
    delete sched_time_lw->currentItem();
}

void MainWindow::schedTabClicked(QListWidgetItem * item)
{
    if (item == 0) return;
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (item->checkState()==Qt::Checked) {
        if (!schedule->sched_tab_list.contains(item->text())) {
            schedule->sched_tab_list << item->text();
        }
    } else {
        if (schedule->sched_tab_list.contains(item->text())) {
            schedule->sched_tab_list.removeAll(item->text());
        }
    }
}

void MainWindow::schedMultitabClicked(QListWidgetItem * item)
{
    if (item == 0) return;
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (item->checkState()==Qt::Checked) {
        if (!schedule->sched_multitab_list.contains(item->text())) {
            schedule->sched_multitab_list << item->text();
        }
    } else {
        if (schedule->sched_multitab_list.contains(item->text())) {
            schedule->sched_multitab_list.removeAll(item->text());
        }
    }
}

void MainWindow::schedTimeClicked(QListWidgetItem * item)
{
    if (item == 0) return;
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (item->checkState()==Qt::Checked) {
        if (!schedule->sched_checked_time_list.contains(item->text())) {
            schedule->sched_checked_time_list << item->text();
        }
    } else {
        if (schedule->sched_checked_time_list.contains(item->text())) {
            schedule->sched_checked_time_list.removeAll(item->text());
        }
    }
}

void MainWindow::schedIntervalChanged(int i)
{
    if (!schedGroupBox->isEnabled()) return;
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    schedule->periodical_interval = i;
}

void MainWindow::timingTabIndexChanged(int i)
{
    if (!schedGroupBox->isEnabled()) return;
    item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0))->timing_tab_index = i;
}

void MainWindow::saveSchedSettings(int row)
{
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(row, 0));
    schedule->sched_tab_list.clear();
    for (int i = 0; i < sched_tab_lw->count(); ++i) {
        if (sched_tab_lw->item(i)->checkState()!=Qt::Checked) continue;
        schedule->sched_tab_list << sched_tab_lw->item(i)->text();
    }
    schedule->sched_multitab_list.clear();
    for (int i = 0; i < sched_multitab_lw->count(); ++i) {
        if (sched_multitab_lw->item(i)->checkState()!=Qt::Checked) continue;
        schedule->sched_multitab_list << sched_multitab_lw->item(i)->text();
    }
    schedule->sched_time_list.clear();
    schedule->sched_checked_time_list.clear();
    for (int i = 0; i < sched_time_lw->count(); ++i) {
        if (sched_time_lw->item(i)->checkState()!=Qt::Checked) {
            schedule->sched_time_list << sched_time_lw->item(i)->text();
        } else {
            schedule->sched_time_list << sched_time_lw->item(i)->text();
            schedule->sched_checked_time_list << sched_time_lw->item(i)->text();
        }
    }
    schedule->sched_date_list.clear();
    schedule->sched_checked_date_list.clear();
    for (int i = 0; i < sched_sync_dates->count(); ++i) {
        schedule->sched_date_list << sched_sync_dates->item(i)->text();
        if (sched_sync_dates->item(i)->checkState()==Qt::Checked) {
            schedule->sched_checked_date_list << sched_sync_dates->item(i)->text();
        }
    }
    schedule->sched_unchecked_days_list.clear();
    for (int i = 0; i < sched_days_lw->count(); ++i) {
        if (sched_days_lw->item(i)->checkState() == Qt::Unchecked) {
            schedule->sched_unchecked_days_list << QString("%1").arg(i);
        }
    }
}

void MainWindow::setScheduleStatusOn(bool on, QTableWidgetItem * item)
{
    if (!on) {
        item->setText(tr("Off"));
        item->setBackground(Qt::red);
        item->setForeground(Qt::white);
    } else {
        item->setText(tr("On"));
        item->setBackground(Qt::darkGreen);
        item->setForeground(Qt::white);
    }
}

void SyncSchedule::startSchedule()
{
    if (sched_parent->tw_schedules->currentItem() == parent_item) sched_parent->saveSchedSettings(parent_item->row());
    if (sched_tab_list.count()==0 && sched_multitab_list.count()==0) return;
    for (int t = 0; t < timers.count(); ++t) {
        timers.at(t)->stop();
        delete timers.at(t);
    } timers.clear(); QTime time = QTime::currentTime(); QTimer * timer; bool scheduled = false;
    QDateTime current_datetime = QDateTime::currentDateTime();
    if (timing_tab_index == 0) {
        for (int r = 0; r < sched_checked_time_list.count(); ++r) {
            timer = new QTimer(this);
            QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(stop()));
            QObject::connect(timer, SIGNAL(timeout()), this, SLOT(syncSchedule()));
            QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkSchedStatus()));
            int h = time.msecsTo(QTime(sched_checked_time_list.at(r).split(':').at(0).toInt(), sched_checked_time_list.at(r).split(':').at(1).toInt()));
            if (h<0) {
                QDateTime datetime (QDate::currentDate().addDays(1),
                            QTime(sched_checked_time_list.at(r).split(':').at(0).toInt(), sched_checked_time_list.at(r).split(':').at(1).toInt()));
                h = current_datetime.secsTo(datetime)*1000;
            } if (h<0) { delete timer; continue; }
            timer->start(h); scheduled = true;
            timers << timer;
        }
    } else {
        timer = new QTimer(this);
        QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(stop()));
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(syncSchedule()));
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkSchedStatus()));
        timer->start(periodical_interval*60000);
        scheduled = true;
        timers << timer;
    }
    if (scheduled) {
        scheduling = true;
        sched_parent->setScheduleStatusOn(true, sched_parent->tw_schedules->item(parent_item->row(), 1));
        if (sched_parent->tw_schedules->currentIndex().isValid()) sched_parent->enableSchedule(sched_parent->tw_schedules->currentRow());
    } else scheduling = false;
}

void MainWindow::startSchedule(QTableWidgetItem * item)
{
    //if (tw_schedules->currentItem()==item) saveSchedSettings(item->row());
    item_sched_map.value(item)->startSchedule();
    /*if (schedule->sched_tab_list.count()==0 && schedule->sched_multitab_list.count()==0) return;
    for (int t = 0; t < schedule->timers.count(); ++t) {
        schedule->timers.at(t)->stop();
        delete schedule->timers.at(t);
    } schedule->timers.clear(); QTime time = QTime::currentTime(); QTimer * timer; bool scheduled = false;
    QDateTime current_datetime = QDateTime::currentDateTime();
    if (schedule->timing_tab_index == 0) {
        bool start_today = true;
        QDate sync_date;
        for (int i = 0; i < sched_sync_dates->count(); ++i) {
            if (sched_sync_dates->item(i)->checkState() == Qt::Unchecked) continue;
            sync_date = QDate::fromString(sched_sync_dates->item(i)->text(), Qt::ISODate);
            if (sync_date != QDate::currentDate()) start_today = false;
            else start_today = true;
        }
        if (start_today) {
            for (int r = 0; r < schedule->sched_checked_time_list.count(); ++r) {
                timer = new QTimer(this);
                QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(stop()));
                QObject::connect(timer, SIGNAL(timeout()), schedule, SLOT(syncSchedule()));
                QObject::connect(timer, SIGNAL(timeout()), schedule, SLOT(checkSchedStatus()));
                int h = time.msecsTo(QTime(schedule->sched_checked_time_list.at(r).split(':').at(0).toInt(), schedule->sched_checked_time_list.at(r).split(':').at(1).toInt()));
                if (h<0) {
                    QDateTime datetime (QDate(QDateTime::currentDateTime().date().year(), QDateTime::currentDateTime().date().month(), QDateTime::currentDateTime().date().day()+1),
                                QTime(schedule->sched_checked_time_list.at(r).split(':').at(0).toInt(), schedule->sched_checked_time_list.at(r).split(':').at(1).toInt()));
                    h = current_datetime.secsTo(datetime)*1000;
                } if (h<0) { delete timer; continue; }
                timer->start(h); scheduled = true;
                schedule->timers << timer;
            }
        } else {
            QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(stop()));
            QObject::connect(timer, SIGNAL(timeout()), schedule, SLOT(startSchedule()));
            QObject::connect(timer, SIGNAL(timeout()), schedule, SLOT(checkSchedStatus()));
            QDateTime datetime (QDate::currentDate().addDays(1));
            timer->start(current_datetime.secsTo(datetime)*1000);
            scheduled = true;
            schedule->timers << timer;
        }
    } else {
        timer = new QTimer(this);
        QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(stop()));
        QObject::connect(timer, SIGNAL(timeout()), schedule, SLOT(syncSchedule()));
        QObject::connect(timer, SIGNAL(timeout()), schedule, SLOT(checkSchedStatus()));
        timer->start(schedule->periodical_interval*60000);
        scheduled = true;
        schedule->timers << timer;
    }
    if (scheduled) {
        schedule->scheduling = true;
        setScheduleStatusOn(true, tw_schedules->item(item->row(), 1));
        if (tw_schedules->currentIndex().isValid()) enableSchedule(tw_schedules->currentRow());
    } else schedule->scheduling = false;*/
}

void MainWindow::stopSchedule(QTableWidgetItem * item)
{
    SyncSchedule * schedule = item_sched_map.value(item);
    for (int t = 0; t < schedule->timers.count(); ++t) {
        schedule->timers.at(t)->stop();
        delete schedule->timers.at(t);
    } schedule->timers.clear();
    schedule->scheduling = false;
    if (tw_schedules->currentIndex().isValid()) enableSchedule(tw_schedules->currentRow());
    setScheduleStatusOn(false, tw_schedules->item(item->row(), 1));
}

void MainWindow::startAllSchedules()
{
    for (int i = 0; i < tw_schedules->rowCount(); ++i) {
        startSchedule(tw_schedules->item(i, 0));
    }
}

void MainWindow::stopAllSchedules()
{
    for (int i = 0; i < tw_schedules->rowCount(); ++i) {
        stopSchedule(tw_schedules->item(i, 0));
    }
}

SyncSchedule::SyncSchedule(MainWindow * parent)
{
    sched_parent = parent;
    periodical_interval = 1;
}

void SyncSchedule::syncSchedule()
{
    bool start_today = true;
    if (timing_tab_index == 0) {
        QDate sync_date;
        bool force_start_today = false;
        for (int i = 0; i < sched_parent->sched_sync_dates->count(); ++i) {
            if (sched_parent->sched_sync_dates->item(i)->checkState() == Qt::Unchecked) continue;
            sync_date = QDate::fromString(sched_parent->sched_sync_dates->item(i)->text(), Qt::ISODate);
            if (sync_date != QDate::currentDate()) start_today = false;
            else {
                start_today = true; force_start_today = true;
                break;
            }
        }
        int day = QDate::currentDate().dayOfWeek() - 1;
        if (sched_unchecked_days_list.contains(QString("%1").arg(day))) {
            if (!force_start_today) start_today = false;
        } else {
            start_today = true;
        }
    }
    if (start_today) {
        sched_parent->syncingAll = true; int all_synced_files = 0;
        for (int i = 0; i < sched_tab_list.count(); ++i) {
            for (int x = 0; x < sched_parent->tabWidget->count(); ++x) {
                if (sched_parent->tabWidget->tabText(x)==sched_tab_list.at(i)) {
                    all_synced_files += sched_parent->tabs.value(sched_parent->tabWidget->widget(x))->sync();
                }
            }
        }
        for (int i = 0; i < sched_multitab_list.count(); ++i) {
            for (int x = 0; x < sched_parent->multi_tabWidget->count(); ++x) {
                if (sched_parent->multi_tabWidget->tabText(x)==sched_multitab_list.at(i)) {
                    all_synced_files += ((MultisyncPage *) sched_parent->multi_tabWidget->widget(x))->sync();
                }
            }
       }
       sched_parent->showTrayMessage(tr("Synchronisation complete"), tr("%1 files synchronised").arg(all_synced_files));
       sched_parent->syncingAll = false;
       if (sched_parent->actionShut_down_after_sync->isChecked()) {
            if (!sched_parent->isSyncing()) {
                sched_parent->shutDownComputer();
            }
        }
        if (sched_parent->actionQuit_after_sync->isChecked()) {
            if (!sched_parent->isSyncing()) {
                sched_parent->close();
            }
        }
    }
}

void SyncSchedule::checkSchedStatus()
{
    /*if (timing_tab_index == 0) {
        for (int t = 0; t < sched_checked_time_list.count(); ++t) {
          if (QTime(sched_checked_time_list.at(t).split(':').at(0).toInt(), sched_checked_time_list.at(t).split(':').at(1).toInt())>QTime::currentTime()) return;
       }
    }*/
    for (int i = 0; i < timers.count(); ++i) {
        delete timers.at(i);
    }
    timers.clear();
    startSchedule();
    sched_parent->reloadSchedStatus();
}

void MainWindow::reloadSchedStatus()
{
    QMapIterator<QTableWidgetItem*, SyncSchedule*> i(item_sched_map);
    while (i.hasNext()) {
        i.next();
        if (i.value()->scheduling==false) {
            setScheduleStatusOn(false, tw_schedules->item(i.key()->row(), 1));
        }
    }
    if (tw_schedules->currentIndex().isValid()) enableSchedule(tw_schedules->currentRow());
}

void MainWindow::enableSchedule(int row)
{
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(row, 0));
    sched_name->setDisabled(schedule->scheduling);
    sched_tab_lw->setDisabled(schedule->scheduling);
    sched_multitab_lw->setDisabled(schedule->scheduling);
    timing_tabWidget->setDisabled(schedule->scheduling);
    sched_start->setDisabled(schedule->scheduling);
    sched_stop->setEnabled(schedule->scheduling);
}

void MainWindow::setSchedDatesGB()
{
    sched_dates_agb->setTitle(tr("Dates/Days"));

    QHBoxLayout * lw_layout = new QHBoxLayout;
    sched_sync_dates = new QListWidget (sched_dates_agb);
    connect(sched_sync_dates, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(schedDateClicked(QListWidgetItem *)));
    lw_layout->addWidget(sched_sync_dates);
    sched_days_lw = new QListWidget (sched_dates_agb);
    QListWidgetItem * day_item = new QListWidgetItem (tr("Monday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    day_item = new QListWidgetItem (tr("Tuesday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    day_item = new QListWidgetItem (tr("Wednesday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    day_item = new QListWidgetItem (tr("Thursday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    day_item = new QListWidgetItem (tr("Friday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    day_item = new QListWidgetItem (tr("Saturday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    day_item = new QListWidgetItem (tr("Sunday"), sched_days_lw);
    day_item->setCheckState(Qt::Checked);
    sched_days_lw->addItem(day_item);
    connect(sched_days_lw, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(schedDayClicked(QListWidgetItem *)));
    lw_layout->addWidget(sched_days_lw);
    sched_dates_agb->addLayout(lw_layout, 0, 0);

    QHBoxLayout * add_rem_date_layout = new QHBoxLayout;
    sched_date_edit = new QDateEdit (sched_dates_agb);
    sched_date_edit->setDate(QDate::currentDate());
    add_rem_date_layout->addWidget(sched_date_edit);

    add_date_btn = new QPushButton (sched_dates_agb);
    add_date_btn->setText(tr("Add"));
    connect(add_date_btn, SIGNAL(released()), this, SLOT(addSchedDate()));
    add_rem_date_layout->addWidget(add_date_btn);
    add_rem_date_layout->addStretch();
    rem_date_btn = new QPushButton (sched_dates_agb);
    rem_date_btn->setText(tr("Remove"));
    connect(rem_date_btn, SIGNAL(released()), this, SLOT(removeSchedDate()));
    add_rem_date_layout->addWidget(rem_date_btn);

    sched_dates_agb->addLayout(add_rem_date_layout, 1, 0);
}

void MainWindow::addSchedDate()
{
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (schedule->sched_date_list.contains(sched_date_edit->date().toString(Qt::ISODate))) return;
    QListWidgetItem * item = new QListWidgetItem (sched_date_edit->date().toString(Qt::ISODate));
    item->setCheckState(Qt::Checked);
    sched_sync_dates->addItem(item);
    schedule->sched_date_list << sched_date_edit->date().toString(Qt::ISODate);
    schedule->sched_checked_date_list << sched_date_edit->date().toString(Qt::ISODate);
}

void MainWindow::removeSchedDate()
{
    if (sched_sync_dates->currentItem()==0) { QMessageBox::warning(this, tr("Synkron"), tr("No sync date selected.")); return; }
    SyncSchedule * schedule;
    schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (schedule->sched_date_list.contains(sched_sync_dates->currentItem()->text())) {
        schedule->sched_date_list.removeAt(schedule->sched_date_list.indexOf(sched_sync_dates->currentItem()->text()));
    }
    delete sched_sync_dates->currentItem();
}

void MainWindow::schedDateClicked(QListWidgetItem * item)
{
    if (item == 0) return;
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (item->checkState()==Qt::Checked) {
        if (!schedule->sched_checked_date_list.contains(item->text())) {
            schedule->sched_checked_date_list << item->text();
        }
    } else {
        if (schedule->sched_checked_date_list.contains(item->text())) {
            schedule->sched_checked_date_list.removeAll(item->text());
        }
    }
}

void MainWindow::schedDayClicked(QListWidgetItem * item)
{
    if (item == 0) return;
    SyncSchedule * schedule = item_sched_map.value(tw_schedules->item(tw_schedules->currentRow(), 0));
    if (item->checkState()==Qt::Checked) {
        if (schedule->sched_unchecked_days_list.contains(QString("%1").arg(sched_days_lw->row(item)))) {
            schedule->sched_unchecked_days_list.removeAll(QString("%1").arg(sched_days_lw->row(item)));
        }
    } else {
        if (!schedule->sched_unchecked_days_list.contains(QString("%1").arg(sched_days_lw->row(item)))) {
            schedule->sched_unchecked_days_list << QString("%1").arg(sched_days_lw->row(item));
        }
    }
}

//QDate::fromString()
