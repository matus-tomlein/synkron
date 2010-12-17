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

#include "maincontroller.h"
#include "mainwindow.h"
#include "mtapplication.h"
#include "module.h"

#include <QString>
#include <QFileInfo>
#include <QLocale>
#include <QTranslator>
#include <QVariant>
#include <QMessageBox>

MainController::MainController()
{
    module = new Module(qApp->arguments().at(0));
    setupTranslations();

    f_ver = 1.70;
    ver = new QString("1.7.0");

    window = new MainWindow(module);
    window->show();
}

/**
 translation setup at startup
 */
void MainController::setupTranslations()
{
    QString lang = module->value("lang").toString();
    if (lang.isEmpty()) {
        lang = QLocale::languageToString(QLocale::system().language());
        module->setValue("lang", lang);
    }
    if (lang == "C") { lang = "English"; module->setValue("lang", lang); }
    if (lang != "English") {
        QTranslator * translator = new QTranslator;
        translator->load(QString(":/i18n/Synkron-%1.qm").arg(lang.replace(" ", "_")));
        qApp->installTranslator(translator);
    }
}
