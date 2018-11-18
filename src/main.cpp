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


ClientConnection::ClientConnection(MainWindow * parent, QTcpSocket * socket)
{
    c_parent = parent;
    c_socket = socket;
    c_blocksize = 0;
    QObject::connect(c_socket, SIGNAL(readyRead()), this, SLOT(read()));
    read();
}

void ClientConnection::read()
{
    QDataStream in(c_socket);
    in.setVersion(QDataStream::Qt_4_2);

    if (c_blocksize == 0) {
        if (c_socket->bytesAvailable() < static_cast<int>(sizeof(quint64)))
            return;

        in >> c_blocksize;
    }

    if ( static_cast<quint64> (c_socket->bytesAvailable()) < c_blocksize)
        return;

    QString received_string; QString buffer;
    do {
        in >> buffer; received_string.append(buffer);
    } while (!in.atEnd());

    QTextStream input(&received_string); QStringList bufferlist;
    while (!input.atEnd()) {
        buffer = input.readLine();
        if (!buffer.startsWith("[Synkron")) continue;
        buffer.truncate(buffer.count() - 2);
        bufferlist = buffer.split("\"");

        if (bufferlist.at(0) == "[Synkron raise];") c_parent->show();
        else if (bufferlist.at(0) == "[Synkron globalRename ") {
            c_parent->show();
            bool ok = false;
            QString file_name = QFileInfo(bufferlist.at(1)).fileName();
            QString new_name = QInputDialog::getText(c_parent, tr("Synkron - Rename file"), tr("Type a new name for \"%1\":").arg(bufferlist.at(1)), QLineEdit::Normal, file_name, &ok);
            if (ok) c_parent->globalRename(bufferlist.at(1), new_name);
        }
        else if (bufferlist.at(0) == "[Synkron globalDelete ") { c_parent->show(); c_parent->globalDelete(bufferlist.at(1)); }
        else if (bufferlist.at(0) == "[Synkron loadMultisync ") {
            c_parent->show();
            QFileInfo file_info (bufferlist.at(1));
            c_parent->loadTab(file_info.absoluteFilePath());
        }
    }

    c_socket->close();
    delete this;
}

void MainWindow::httpRequestFinished(bool error)
{
httpRequestFinished_start:
    if (error) {
        switch (QMessageBox::critical(this, tr("Synkron"), tr("Failed to check for updates."), tr("&Try again"), tr("Cancel"), nullptr, 1)) {
        case 0: // Try again
            checkForUpdates(); return; break;
        case 1: // Cancel
            return; break;
        }
    }
    QString str(http_buffer->data()); QTextStream in(&str);
    if (in.readLine() != "[Synkron.current-version]") { error = true; goto httpRequestFinished_start; }
    QString current_ver = in.readLine();
    if (in.readLine() != "[Synkron.current-version.float]") { error = true; goto httpRequestFinished_start; }
    float f_current_ver = in.readLine().toFloat();
    if (in.readLine() != "[Synkron.release-notes]") { error = true; goto httpRequestFinished_start; }
    QString release_notes;
    while (!in.atEnd()) { release_notes.append(in.readLine()); }
    if (f_current_ver <= f_ver) {
        QMessageBox::information(this, tr("Synkron"), tr("Your Synkron is up-to-date."));
    } else {
        QString info; QTextStream out(&info);
        out << "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body><p>" << endl;
        out << "<b>" << tr("Synkron %1 is available now.").arg(current_ver) << "</b><br><br>" << endl;
        out << release_notes << endl << "</p></body></html>";
        QMessageBox::information(this, tr("Synkron"), info);
    }
}

// --- Connection ---
// +++ About +++

void MainWindow::about ()
{
    About *about = new About(ver, QVariant(QDate::currentDate().year()).toString());
    about->show();
}

About::About(QString ver, QString year/*, QString qtver*/)
{
    setupUi(this);
    //NOTE: Really stupid way to appaend the text, you can use the UI instead!
    QString about = "<p style=\"font-family: sans-serif; font-style:italic;\"><span style=\"font-size:12pt;\">Synkron</span><br>";
    about.append("<span style=\"font-size:8pt;\">");
    about.append(tr("Version"));
    about.append(QString(" %1</span></p><p></p>").arg(ver));
    about.append("http://synkron.sourceforge.net/");
    about.append("<p style=\"font-family: sans-serif; font-size:8pt;\">");
    about.append(tr("Synkron is a simple Qt application designed for easy folder synchronisation."));
    about.append("</p><p></p>");
    about.append("<p style=\"font-family: sans-serif; font-size:8pt;\">");
    about.append(tr("This programme is distributed under the terms of the GPL v2."));
    about.append("</p><p></p>");
    about.append(QString::fromUtf8("<p style=\"font-family: sans-serif; font-size:8pt;\">2005-%1 <span style=\"font-style:italic;\">Mat\303\272\305\241 Tomlein</span> (matus.tomlein@gmail.com)</p>").arg(year));
    about.append("<p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
    about.append(tr("The programme is provided AS IS with ABSOLUTELY NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."));
    about.append("</p>");
#ifdef Q_WS_MAC
    about.remove("font-family: sans-serif;");
    about.replace("font-size:12pt;", "font-size:14pt;");
    about.replace("font-size:8pt;", "font-size:10pt;");
#endif
    about_synkron_tb->setHtml(about);
    QString aboutQt = "<p style=\"font-family: sans-serif; font-style:italic;\"><span style=\"font-size:12pt;\">";
    aboutQt.append(tr("About Qt"));
    aboutQt.append("</span></p><p></p><p style=\"font-family: sans-serif; font-size:8pt; font-style:italic;\">");
    aboutQt.append(tr("This programme uses Qt Open Source Edition version %1.").arg(qVersion()));
    aboutQt.append("</p><p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
    aboutQt.append(tr("Qt is a C++ toolkit for cross-platform application development."));
    aboutQt.append("</p><p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
    aboutQt.append(tr("Qt provides single-source portability across MS Windows, Mac OS X, Linux and all major commercial Unix variants. Qt is also available for embedded devices as Qtopia Core."));
    aboutQt.append("</p><p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
    aboutQt.append(tr("Qt is a Trolltech product. See <span style=\"font-style:italic;\">http://www.trolltech.com/qt/</span> for more information."));
    aboutQt.append("</p>");
#ifdef Q_WS_MAC
    aboutQt.remove("font-family: sans-serif;");
    aboutQt.replace("font-size:12pt;", "font-size:14pt;");
    aboutQt.replace("font-size:8pt;", "font-size:10pt;");
#endif
    about_qt_tb->setHtml(aboutQt);
}

// --- About ---
// +++ Languages +++


void MainWindow::checkForUpdates()
{
    delete http_buffer;

#if QT_VERSION >= 0x050000
    QByteArray bArr = http->get(QNetworkRequest(QUrl("http://synkron.sourceforge.net/current-version")))->readAll();
    http_buffer = new QBuffer(&bArr);
#else
    http_buffer = new QBuffer(this);
    http->setHost("synkron.sourceforge.net");
    http->get("/current-version", http_buffer);
#endif

}

void MainWindow::changeLanguage()
{
    QWidget * lang_widget = new QWidget(this, Qt::Dialog);
    lang_widget->setWindowModality(Qt::WindowModal);
    lang_widget->setAttribute(Qt::WA_DeleteOnClose);
    lang_widget->setWindowTitle(tr("Change language - Synkron"));
    QGridLayout * lang_glayout = new QGridLayout(lang_widget);
    lang_glayout->setMargin(6); lang_glayout->setSpacing(6);
    QLabel * lang_label = new QLabel(lang_widget);
    lang_label->setText(tr("Select your preferred language"));
    lang_glayout->addWidget(lang_label, 0, 0);
    langComboBox = new QComboBox(lang_widget);
    QStringList langs(synkron_i18n.keys()); langs.sort();
    for (int i = 0; i < langs.count(); ++i) {
        langComboBox->addItem(langs.at(i));
        if (langs.at(i) == "English") { langComboBox->setCurrentIndex(i); }
    }
    lang_glayout->addWidget(langComboBox, 1, 0);
    QDialogButtonBox * lang_buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, lang_widget);
    QObject::connect(lang_buttonbox, SIGNAL(accepted()), this, SLOT(langChanged()));
    QObject::connect(lang_buttonbox, SIGNAL(rejected()), lang_widget, SLOT(close()));
    lang_glayout->addWidget(lang_buttonbox, 2, 0);
    lang_widget->show();
}

void MainWindow::langChanged()
{
    if (langComboBox == nullptr) { return; }
    QString lang = synkron_i18n.value(langComboBox->currentText(), langComboBox->currentText());
    QString current_lang = sync_settings->value("lang", "English").toString();
    if (current_lang != lang) {
        sync_settings->setValue("lang", lang);
        QMessageBox::information(this, tr("Synkron"), tr("You need to restart Synkron for the changes to apply."));
    }
    if (langComboBox->parent() == nullptr) { return; }
    QWidget * lang_widget = static_cast<QWidget *>(langComboBox->parent());
    lang_widget->close();
}

// --- Languages ---

void MainWindow::closeEvent(QCloseEvent * event)
{
    if (skip_close_event) {
        trayIcon->hide();
        event->accept();
        return;
    }
    if (!no_closedialogue) {
#ifdef Q_WS_MAC
        event->ignore();
        this->hide();
        return;
#else
        if (!closeDialogue()) {
            event->ignore();
            return;
        }
#endif
    }
    saveSettings();
    trayIcon->hide();
}

int main(int argc, char *argv[])
{
    MTApplication app(argc, argv);
    app.setOrganizationName("Matus Tomlein");
    app.setApplicationName("Synkron");

    QFileInfo file_info (app.arguments().at(0));
    QDir dir (file_info.dir()); QSettings * sync_settings;
#ifdef PORTABLE_APP
    dir.cdUp(); dir.cdUp();
    dir.cd("Data"); dir.cd("settings");
#endif
    if (dir.exists("Synkron.ini")) {
        sync_settings = new QSettings (dir.absoluteFilePath("Synkron.ini"), QSettings::IniFormat);
    } else {
        sync_settings = new QSettings (QSettings::IniFormat, QSettings::UserScope, "Matus Tomlein", "Synkron");
    }
    QString lang = sync_settings->value("lang").toString();
    if (lang.isEmpty()) {
        lang = QLocale::languageToString(QLocale::system().language());
        sync_settings->setValue("lang", lang);
    }
    if (lang == "C") { lang = "English"; sync_settings->setValue("lang", lang); }
    if (lang != "English") {
        QTranslator * translator = new QTranslator;
        translator->load(QString(":/i18n/Synkron-%1.qm").arg(lang.replace(" ", "_")));
        app.installTranslator(translator);
    }

    MainWindow *window = new MainWindow (sync_settings);
    if (window->runHidden() && app.arguments().count() <= 1) {
        window->hide();
    } else {
        window->show();
    }
    app.setAppMainWindow(window);
    return app.exec();
}
