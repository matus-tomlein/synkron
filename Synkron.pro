QT += network \
    sql \
    xml \
    xmlpatterns
TARGET = Synkron
TEMPLATE = app
CONFIG += thread
SOURCES += main.cpp \
    mainwindow.cpp \
    mtapplication.cpp \
    maincontroller.cpp \
    settings.cpp \
    mtfile.cpp \
    synctabform.cpp \
    folders.cpp \
    syncfolders.cpp \
    folder.cpp \
    syncfolderform.cpp \
    module.cpp \
    extendedtreewidget.cpp \
    abstractsyncpage.cpp \
    syncpage.cpp \
    syncfolder.cpp \
    abstractsyncadvancedview.cpp \
    syncadvancedview.cpp \
    exceptions.cpp \
    exceptionbundle.cpp \
    exceptiongroup.cpp \
    blacklistgroup.cpp \
    filtergroup.cpp \
    fileblacklistgroup.cpp \
    folderblacklistgroup.cpp \
    exceptionform.cpp \
    syncaction.cpp \
    folderactiongroup.cpp \
    syncfile.cpp \
    abstractexceptionbundle.cpp \
    syncexceptionbundle.cpp \
    exceptionfile.cpp \
    filecompare.cpp \
    syncoutmessage.cpp \
    mttablewidgetitem.cpp
HEADERS += mainwindow.h \
    mtapplication.h \
    maincontroller.h \
    settings.h \
    mtfile.h \
    mtadvancedgroupbox.h \
    extendedlineedit.h \
    extendedlistwidget.h \
    extendedtreewidget.h \
    synctabform.h \
    folders.h \
    syncfolders.h \
    folder.h \
    syncfolderform.h \
    module.h \
    abstractsyncpage.h \
    syncpage.h \
    syncfolder.h \
    abstractsyncadvancedview.h \
    syncadvancedview.h \
    exceptions.h \
    exceptionbundle.h \
    exceptiongroup.h \
    blacklistgroup.h \
    filtergroup.h \
    fileblacklistgroup.h \
    folderblacklistgroup.h \
    exceptionform.h \
    syncaction.h \
    folderactiongroup.h \
    syncfile.h \
    abstractexceptionbundle.h \
    syncexceptionbundle.h \
    exceptionfile.h \
    filecompare.h \
    syncoutmessage.h \
    mttablewidgetitem.h
FORMS += mainwindow.ui \
    synctabform.ui \
    syncfolderform.ui \
    exceptionform.ui \
    navigationitemform.ui
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
    SOURCES += mtfile_macx.mm
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
