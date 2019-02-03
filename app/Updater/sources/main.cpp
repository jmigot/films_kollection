
/**
 * Entry point of program - main() function
 *
 * @date April 2011
 * @author Jean-Luc Migot (jean-luc.migot@filmskollection.org)
 *
 *
 * Copyright (C) 2011 Jean-Luc Migot
 *
 * This file is part of FilmsKollection.
 *
 * FilmsKollection is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FilmsKollection is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "../../sources/Config.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
	if (argc <= 1)
		return EXIT_SUCCESS;

	QApplication *application(new QApplication(argc, argv));

	// This is only for testing needed dll (when program is run outside Qt Creator with dll paths)
	//application->setLibraryPaths(QStringList(QApplication::applicationDirPath()));

	// Application name used by system to manage processus correctly
	application->setApplicationName(Fk::ApplicationTitle + " - Updater");

	// QString must be UTF-8 (as all code is written in UTF-8 format)
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	// Create config (that will read config file)
	Config *config = new Config;

	// Traduction
	QTranslator translator;
	QString filename(":/lang/" + config->value(Fk::Language).toString());

	// If locale traduction file doesn't exists, take english by default
	if ( ! QFile::exists(filename))
		filename = ":/lang/en";

	translator.load(filename);
	application->installTranslator(&translator);

	delete config;

	// Create main window
	MainWindow window(QString(argv[1]).toFloat());

	// Show widget
	QDesktopWidget desktop;

	int screenWidth = desktop.screenGeometry().width();
	int screenHeight = desktop.screenGeometry().height();

	window.adjustSize();
	window.move((screenWidth - window.width()) / 2, (screenHeight - window.height()) / 2);
	window.show();

	// Launch update
	window.run();

	return application->exec();
}

