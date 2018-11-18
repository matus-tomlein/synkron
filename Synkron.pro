TEMPLATE = app
QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
INCLUDEPATH += src
HEADERS += src/mainwindow.h \
     src/syncpage.h \
     src/mtfile.h \
     src/mtadvancedgroupbox.h \
     src/extendedlineedit.h \
     src/extendedlistwidget.h \
     src/extendedtreewidget.h \
     src/syncfolders.h \
     src/mtstringset.h \
     src/mtpathdialogue.h \
     src/mtdictionary.h
FORMS += ui/mainwindow.ui \
     ui/about.ui \
     ui/multisyncpage.ui \
     ui/syncviewitem.ui
SOURCES += src/main.cpp \
     src/sync.cpp \
     src/restore.cpp \
     src/blacklist.cpp \
     src/multisync.cpp \
     src/scheduler.cpp \
     src/filters.cpp \
     src/mtfile.cpp \
     src/syncfolders.cpp \
     src/syncview.cpp \
     src/analyse.cpp \
     src/tabs.cpp \
     src/other.cpp \
     src/settings.cpp \
     src/syncdb.cpp \
     src/mainwindow.cpp
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
     
     isEmpty(prefix):prefix = /usr/local
     isEmpty(PREFIX):PREFIX = $$prefix
     BINDIR = $$PREFIX/bin
     DATADIR = $$PREFIX/share

     INSTALLS = target \
                desktop \
                pixmaps \

     target.path = $$BINDIR
     desktop.path = $$DATADIR/applications/
     desktop.files += $${TARGET}.desktop
     pixmaps.path = $$DATADIR/pixmaps/
     pixmaps.files += images/Synkron128.png
}
macx {
     # Comment the following line to use the NSFileManager class
     # to perform copy operations instead of QFile + /usr/bin/touch:
     DEFINES += USE_UNIX_TOUCH_COMMAND
     SOURCES += ./src/mtfile_macx.mm
     LIBS += -framework Carbon
     ICON = images/Synkron128.icns
     icons.path = Contents/Resources
     icons.files = images/slist.icns
     QMAKE_BUNDLE_DATA += icons
     CONFIG += x86
#     QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
#     QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
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
TRANSLATIONS += \
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
     i18n/Synkron-Valencian.ts \
     i18n/Synkron-Romanian.ts \
     i18n/Synkron-Turkish.ts

OTHER_FILES += \
    i18n/Synkron-Valencian.ts \
    i18n/Synkron-Turkish.ts \
    i18n/Synkron-Spanish.ts \
    i18n/Synkron-Slovak.ts \
    i18n/Synkron-Russian.ts \
    i18n/Synkron-Romanian.ts \
    i18n/Synkron-French.ts \
    i18n/Synkron-German.ts \
    i18n/Synkron-i18n.ts \
    i18n/Synkron-Italian.ts \
    i18n/Synkron-Japanese.ts \
    i18n/Synkron-Polish.ts \
    i18n/Synkron-Finnish.ts \
    i18n/Synkron-Dutch.ts \
    i18n/Synkron-Czech.ts \
    i18n/Synkron-Chinese.ts \
    i18n/Synkron-Brazilian_Portuguese.ts \
    i18n/Synkron-Arabic.ts
