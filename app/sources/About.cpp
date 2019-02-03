
/**
 * QWidget to show the about message
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

#include "About.h"
#include "Config.h"
#include "Utils.h"

#include <QDebug>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
About::About(Utils *utils)
	: Widget(utils, tr("About"), QPixmap(":/images/star_100"))
{
	// Members
	_text = new QTextBrowser;

	// Properties
	QPalette palette;
	palette.setColor(QPalette::Base, QColor(0, 0, 0, 1));

	// Transparent QTextBrowser
	_text->setOpenExternalLinks(true);
	_text->setPalette(palette);
	_text->setFrameShape(QTextEdit::NoFrame);

	// About message
	_text->setText("<h2>" + Fk::ApplicationTitle + " " + QString::number(Fk::Version) + "</h2>" +
				   "<h3>" + tr("Author") + "</h3>" +
				   "Jean-Luc Migot <a style='text-decoration:none; color: #5555CC' href='mailto:" + Fk::EmailContact + "'>" + Fk::EmailContact + "</a><br />"
				   "<h3>" + tr("Web site") + "</h3>"
				   "<a style='text-decoration:none; color: #5555CC' href='" + Fk::UrlHome + "'>" + Fk::UrlHome + "</a>");

	// Layouts
	_layout->addWidget(_text);
	_layout->addWidget(_close, 0, Qt::AlignHCenter);

	this->display();
}

About::~About()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~About()";
	#endif
}
