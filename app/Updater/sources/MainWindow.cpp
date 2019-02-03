
/**
 * QWidget
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
#include "../../sources/Network.h"
#include "../../sources/Config.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QProgressBar>
#include <QVBoxLayout>

MainWindow::MainWindow(float version)
	: _version(version)
{
	// Members
	_network     = new Network;
	_layout      = new QVBoxLayout(this);
	_text        = new QLabel;
	_progressBar = new QProgressBar;

	// Properties
	this->setWindowTitle(Fk::ApplicationTitle);

	_progressBar->setRange(0,0);

	// Layouts
	_layout->addWidget(_text);
	_layout->addWidget(_progressBar);
}

MainWindow::~MainWindow()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~MainWindow()";
	#endif
}

void MainWindow::run()
{
	_text->setText("<h3>" + tr("Searching update...") + "</h3>");

	if (_network->get(Fk::UrlCheckVersion, 5, 2))
	{
		double newVersion = _network->response().simplified().toFloat();

		if (newVersion > _version)
		{
			_text->setText("<h3>" + tr("A newer version is available !") + "</h3>");
			this->update();
		}

		else
		{
			_text->setText("<h3>" + tr("Your already have the latest version.") + "</h3>");
			_progressBar->setRange(0,1);
			_progressBar->setValue(1);
		}
	}

	else
	{
		_text->setText("<h3>" + tr("Impossible to get latest version number.") + "</h3>");
		_progressBar->setRange(0,1);
		_progressBar->setValue(1);
	}
}

void MainWindow::update()
{
	_text->setText("<h3>" + tr("Updating...") + "</h3>");

	QString urlFiles = Fk::UrlHome;

	#if defined(Q_WS_WIN)
		urlFiles += "/downloads/update/windows/";

	#elif defined(Q_WS_X11)
		urlFiles += "/downloads/update/linux/";

	#else
		QMessageBox::critical(this, tr("Error"), tr("We are sorry, automatic update doesn't exist for your system."));
		return;

	#endif

	if (_network->get(urlFiles + "files.txt", 5, 2))
	{
		bool ok = true;
		QDir dir;

		// Foreach file to update (I don't trust XML - security issues all the time)
		foreach (QString filename, _network->response().split('\n'))
		{
			QRegExp regex("<addDir>(.+)</addDir>", Qt::CaseInsensitive);

			// Add dirs
			if (filename.contains(regex))
				dir.mkpath(QApplication::applicationDirPath() + "/" + regex.cap(1));

			regex.setPattern("<addFile>(.+)</addFile>");

			// Add files
			if (filename.contains(regex))
			{
				_text->setText("<h3>" + tr("Downloading %1...").arg(regex.cap(1).section("/", -1)) + "</h3>");

				// Get fileName from http
				if (_network->get(urlFiles + regex.cap(1), 30, 1))
				{
					QFile file(QApplication::applicationDirPath() + "/" + regex.cap(1));

					if (file.open(QFile::WriteOnly | QFile::Truncate) && file.isWritable())
					{
						file.write(_network->response());
						file.close();
					}

					else
						ok = false;
				}

				else
					ok = false;
			}

			regex.setPattern("<removeDir>(.+)</removeDir>");

			// Remove dirs
			if (filename.contains(regex))
				dir.rmdir(QApplication::applicationDirPath() + "/" + regex.cap(1));

			regex.setPattern("<removeFile>(.+)</removeFile>");

			// Remove files
			if (filename.contains(regex))
				QFile::remove(QApplication::applicationDirPath() + "/" + regex.cap(1));

			if ( ! ok)
				break;
		}

		if (ok)
		{
			_text->setText("<h3>" + tr("Update done !") + "</h3>");
			_progressBar->setRange(0,1);
			_progressBar->setValue(1);
		}

		else
		{
			_text->setText("<h3>" + tr("An error occured while downloading files.") + "</h3>");
			_progressBar->setRange(0,1);
			_progressBar->setValue(1);
		}
	}

	else
	{
		_text->setText("<h3>" + tr("Impossible to download files list.") + "</h3>");
		_progressBar->setRange(0,1);
		_progressBar->setValue(1);
	}
}

void MainWindow::launchProgram()
{
	#if defined(Q_WS_WIN)
		QProcess::startDetached("FilmsKollection.exe", QStringList(), QApplication::applicationDirPath() + "/");

	#elif defined(Q_WS_X11)
		QProcess::startDetached("FilmsKollection", QStringList(), QApplication::applicationDirPath() + "/");

	#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	this->launchProgram();
	event->accept();
}
