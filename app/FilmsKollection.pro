QT += network \
	sql
HEADERS += sources/MainWindow.h \
	sources/Database.h \
	sources/Config.h \
	sources/Parser.h \
	sources/Network.h \
	sources/Film.h \
	sources/Utils.h \
	sources/FilmView.h \
	sources/FilmModify.h \
	sources/SearchBar.h \
	sources/Dialog.h \
	sources/Progress.h \
	sources/Welcome.h \
	sources/AllChoices.h \
	sources/Statistics.h \
	sources/Doubloons.h \
	sources/Preferences.h \
	sources/Help.h \
	sources/ScanDirs.h \
	sources/Widget.h \
	sources/Choice.h \
	sources/About.h \
	sources/RenameFiles.h
SOURCES += sources/MainWindow.cpp \
	sources/Database.cpp \
	sources/Parser.cpp \
	sources/Network.cpp \
	sources/main.cpp \
	sources/Film.cpp \
	sources/Config.cpp \
	sources/Utils.cpp \
	sources/FilmView.cpp \
	sources/FilmModify.cpp \
	sources/SearchBar.cpp \
	sources/Dialog.cpp \
	sources/Progress.cpp \
	sources/Welcome.cpp \
	sources/AllChoices.cpp \
	sources/Statistics.cpp \
	sources/Doubloons.cpp \
	sources/Preferences.cpp \
	sources/Help.cpp \
	sources/ScanDirs.cpp \
	sources/Widget.cpp \
	sources/Choice.cpp \
	sources/About.cpp \
	sources/RenameFiles.cpp
RESOURCES += Resources.qrc
TRANSLATIONS = lang/lang_en.ts \
	lang/lang_fr.ts
win32:RC_FILE += Icon.rc
