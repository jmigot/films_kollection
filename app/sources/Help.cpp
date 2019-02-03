
/**
 * QWidget to show program help
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

#include "Help.h"
#include "Config.h"
#include "MainWindow.h"
#include "Utils.h"

#include <QDebug>
#include <QFile>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Help::Help(Utils *utils)
	: Widget(utils, tr("Help"), QPixmap(":/images/help"), true)
{
	// Members
	_text = new QTextBrowser;

	// Properties
	QPalette palette;
	palette.setColor(QPalette::Base, QColor(0, 0, 0, 1));

	_text->setOpenExternalLinks(true);
	_text->setPalette(palette);
	_text->setFrameShape(QTextEdit::NoFrame);

	// Connect
	connect(_close, SIGNAL(clicked()), _utils->window(), SLOT(reloadCurrentFilm()));

	// Layouts
	_layout->addWidget(_text);
	_layout->addWidget(_close, 0, Qt::AlignHCenter);

	this->run();
}

Help::~Help()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Help()";
	#endif
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
void Help::run()
{
	// Load help file based on locale
	QFile file(":/help/" + _utils->config()->value(Fk::Language).toString());

	// If locale help file doesn't exists, take english by default
	if ( ! file.exists())
		file.setFileName(":/help/en");

	if (file.exists() && file.open(QFile::ReadOnly | QFile::Text) && file.isReadable())
	{
		_text->setHtml(file.readAll());
		file.close();
	}
}
