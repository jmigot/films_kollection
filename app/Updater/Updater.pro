QT += network
HEADERS += sources/MainWindow.h \
    ../sources/Network.h \
    ../sources/Config.h
SOURCES += sources/MainWindow.cpp \
    sources/main.cpp \
    ../sources/Network.cpp \
    ../sources/Config.cpp
TRANSLATIONS = lang/lang_en.ts \
    lang/lang_fr.ts
RESOURCES += Resources.qrc
