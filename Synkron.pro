QT += network \
    sql \
    xml \
    xmlpatterns
TARGET = Synkron
TEMPLATE = app
CONFIG += thread
UI_DIR = tmp/ui
MOC_DIR = tmp/moc
RCC_DIR = tmp/rcc
OBJECTS_DIR = tmp/obj
INCLUDEPATH += src ui
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/mtapplication.cpp \
    src/maincontroller.cpp \
    src/settings.cpp \
    src/mtfile.cpp \
    src/synctabform.cpp \
    src/folders.cpp \
    src/syncfolders.cpp \
    src/folder.cpp \
    src/syncfolderform.cpp \
    src/module.cpp \
    src/extendedtreewidget.cpp \
    src/abstractsyncpage.cpp \
    src/syncpage.cpp \
    src/syncfolder.cpp \
    src/abstractsyncadvancedview.cpp \
    src/syncadvancedview.cpp \
    src/exceptions.cpp \
    src/exceptionbundle.cpp \
    src/exceptiongroup.cpp \
    src/blacklistgroup.cpp \
    src/filtergroup.cpp \
    src/fileblacklistgroup.cpp \
    src/folderblacklistgroup.cpp \
    src/exceptionform.cpp \
    src/syncaction.cpp \
    src/folderactiongroup.cpp \
    src/syncfile.cpp \
    src/abstractexceptionbundle.cpp \
    src/syncexceptionbundle.cpp \
    src/exceptionfile.cpp \
    src/filecompare.cpp \
    src/syncoutmessage.cpp \
    src/mttablewidgetitem.cpp \
    src/extendedlineedit.cpp \
    src/mtprogressbar.cpp \
    src/messagehandler.cpp \
    src/analyseaction.cpp \
    src/analyseform.cpp \
    src/analysetreewidgetitem.cpp \
    src/syncform.cpp \
    src/analysefile.cpp \
    src/syncactionoptions.cpp \
    src/syncactiongeneraloptions.cpp \
    src/syncactionfolderoptions.cpp \
    src/backuphandler.cpp \
    src/backupaction.cpp \
    src/restoreform.cpp
HEADERS += src/mainwindow.h \
    src/mtapplication.h \
    src/maincontroller.h \
    src/settings.h \
    src/mtfile.h \
    src/mtadvancedgroupbox.h \
    src/extendedlineedit.h \
    src/extendedlistwidget.h \
    src/extendedtreewidget.h \
    src/synctabform.h \
    src/folders.h \
    src/syncfolders.h \
    src/folder.h \
    src/syncfolderform.h \
    src/module.h \
    src/abstractsyncpage.h \
    src/syncpage.h \
    src/syncfolder.h \
    src/abstractsyncadvancedview.h \
    src/syncadvancedview.h \
    src/exceptions.h \
    src/exceptionbundle.h \
    src/exceptiongroup.h \
    src/blacklistgroup.h \
    src/filtergroup.h \
    src/fileblacklistgroup.h \
    src/folderblacklistgroup.h \
    src/exceptionform.h \
    src/syncaction.h \
    src/folderactiongroup.h \
    src/syncfile.h \
    src/abstractexceptionbundle.h \
    src/syncexceptionbundle.h \
    src/exceptionfile.h \
    src/filecompare.h \
    src/syncoutmessage.h \
    src/mttablewidgetitem.h \
    src/mtprogressbar.h \
    src/messagehandler.h \
    src/analyseaction.h \
    src/analyseform.h \
    src/analysetreewidgetitem.h \
    src/syncform.h \
    src/analysefile.h \
    src/syncactionoptions.h \
    src/syncactiongeneraloptions.h \
    src/syncactionfolderoptions.h \
    src/backuphandler.h \
    src/backupaction.h \
    src/restoreform.h
FORMS += ui/mainwindow.ui \
    ui/synctabform.ui \
    ui/syncfolderform.ui \
    ui/exceptionform.ui \
    ui/navigationitemform.ui \
    ui/analyseform.ui \
    ui/syncform.ui \
    ui/restoreform.ui
RESOURCES += resources.qrc \
    i18n.qrc
DESTDIR = ./
unix { 
    OBJECTS_DIR = .build.unix/
    MOC_DIR = .build.unix/
    RCC_DIR = .build.unix/
}
unix:!macx { 
    DEFINES += USE_UNIX_TOUCH_COMMAND
    TARGET = synkron
}
macx { 
    # Comment the following line to use the NSFileManager class
    # to perform copy operations instead of QFile + /usr/bin/touch:
    DEFINES += USE_UNIX_TOUCH_COMMAND
    SOURCES += src/mtfile_macx.mm
    ICON = images/Synkron128.icns
    icons.path = Contents/Resources
    icons.files = images/slist.icns
    QMAKE_BUNDLE_DATA += icons
    CONFIG += x86
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
}
win32 { 
    # If not commented, the following line ensures that Synkron is compiled
    # as a portable app. More info at portableapps.com
    # DEFINES += PORTABLE_APP
    OBJECTS_DIR = .build.win32/
    MOC_DIR = .build.win32/
    RCC_DIR = .build.win32/
    RC_FILE = Synkron.rc
}
TRANSLATIONS += i18n/Synkron-Arabic.ts \
    i18n/Synkron-Brazilian_Portuguese.ts \
    i18n/Synkron-Chinese.ts \
    i18n/Synkron-Czech.ts \
    i18n/Synkron-Dutch.ts \
    i18n/Synkron-Finnish.ts \
    i18n/Synkron-French.ts \
    i18n/Synkron-German.ts \
    i18n/Synkron-Italian.ts \
    i18n/Synkron-Japanese.ts \
    i18n/Synkron-Polish.ts \
    i18n/Synkron-Russian.ts \
    i18n/Synkron-Slovak.ts \
    i18n/Synkron-Spanish.ts \
    i18n/Synkron-Valencian.ts
OTHER_FILES += 
