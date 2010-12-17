/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2010 Matus Tomlein (matus.tomlein@gmail.com)

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

#ifndef MTAPPLICATION_H
#define MTAPPLICATION_H

#include <QApplication>

class MainWindow;

class MTApplication : public QApplication
{
    Q_OBJECT
protected:
    void init() { app_main_window = NULL; };
public:
    MTApplication(int & argc, char ** argv):
    QApplication(argc, argv) { init(); };
    MTApplication(int & argc, char ** argv, bool GUIenabled):
    QApplication(argc, argv, GUIenabled) { init(); };
    MTApplication(int & argc, char ** argv, Type type):
    QApplication(argc, argv, type) { init(); };
#ifdef Q_WS_X11
    MTApplication(Display * display, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0):
    QApplication(display, visual, colormap) { init(); };
    MTApplication(Display * display, int & argc, char ** argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0):
    QApplication(display, argc, argv, visual, colormap) { init(); };
#endif
#ifdef Q_WS_MAC
    bool macEventFilter(EventHandlerCallRef, EventRef) {
        /*switch (GetEventKind(event)) {
            case kEventAppActivated: if (app_main_window != NULL) {
                    if (!app_main_window->runHidden() || app_main_window->shownManually())
                    { app_main_window->show(); }
                } break;
            }*/
        return false;
    };
#endif
    void setAppMainWindow(MainWindow * w) { app_main_window = w; };
private:
    MainWindow * app_main_window;
};

#endif // MTAPPLICATION_H
