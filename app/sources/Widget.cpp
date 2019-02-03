
/**
 * Abstract QWidget
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

#include "Widget.h"
#include "Config.h"
#include "Utils.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Widget::Widget(Utils *utils, const QString &title, const QPixmap &pixmap, bool integratedWidget, bool modal)
	: _utils(utils)
{
	// Members
	_layout      = new QVBoxLayout(this);
	_titleLayout = new QHBoxLayout;
	_titlePixmap = new QLabel;
	_title       = new QLabel("<h1>" + title + "</h1>");
	_close       = new QPushButton(QIcon(":/images/cross"), tr("Close"));

	// Properties
	if ( ! integratedWidget)
	{
		this->setWindowTitle(title);

		if (modal)
			this->setWindowModality(Qt::ApplicationModal);

		this->setAttribute(Qt::WA_DeleteOnClose);
		this->setWindowIcon(QIcon(":/images/icon"));
	}

	_titleLayout->setAlignment(Qt::AlignHCenter);
	_titlePixmap->setPixmap(_utils->scale(pixmap, Fk::IconRatio));
	_title->setStyleSheet("color: #5555CC");

	// Connects
	connect(_close, SIGNAL(clicked()), this, SLOT(close()));

	// Layouts
	_layout->addLayout(_titleLayout);
	_titleLayout->addWidget(_titlePixmap);
	_titleLayout->addWidget(_title);
	_layout->addWidget(new QLabel("<hr />"));
}

Widget::~Widget()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Widget()";
	#endif
}

/*--------------------------------------------------------------------*\
		Protected
\*--------------------------------------------------------------------*/
void Widget::display(bool fixSize, int width, int height)
{
	// Adjust widget size after all child widgets have been added */
	this->adjustSize();

	// If a fixed width or height is required
	if (width != -1 || height != -1)
	{
		if (height == -1)
			height = this->height();

		if (width == -1)
			width = this->width();

		this->resize(width, height);
	}

	// If fix size is required
	if (fixSize)
		this->setFixedSize(this->size());

	// Calculate coordinates of center of the screen
	QDesktopWidget desktop;
	int x = (desktop.screenGeometry().width() - this->width()) / 2;
	int y = (desktop.screenGeometry().height() - this->height()) / 2;

	// Move widget on center of the screen and show it
	this->move(x, y);
	this->show();
}
