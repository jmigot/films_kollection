
/**
 * QWidget to create custom progress bars waiters
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

#include "Progress.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Progress::Progress(const QString &title, const QString &text, int nbElements)
{
	// Members
	_layout        = new QVBoxLayout(this);
	_text          = new QLabel(text);
	_progressBar   = new QProgressBar;
	_stop          = new QPushButton(QIcon(":/images/cross"), tr("Stop"));

	// Properties
	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
	this->setWindowTitle(title);
	this->setWindowIcon(QIcon(":/images/icon"));
	this->setWindowModality(Qt::ApplicationModal);

	_progressBar->setRange(0, nbElements);
	_progressBar->setValue(0);

	// Connects
	connect(_stop, SIGNAL(clicked()), this, SLOT(stop()));

	// Layouts
	_layout->addWidget(_text);
	_layout->addWidget(_progressBar);

	this->adjustSize();

	// Calculate coordinates of center of the screen
	QDesktopWidget desktop;
	int x = (desktop.screenGeometry().width() - this->width()) / 2;
	int y = (desktop.screenGeometry().height() - this->height()) / 2;

	// Move widget on center of the screen and show it
	this->move(x, y);
	this->show();

	// Force refresh and display
	QApplication::flush();
	QApplication::processEvents();
}

Progress::~Progress()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Progress()";
	#endif
}

void Progress::setMax(int max)
{
	_progressBar->setRange(0, max);
}

void Progress::setText(const QString &text)
{
	_text->setText(text);

	// Try to refresh text
	_text->update();
	_layout->update();
	this->update();
	QApplication::flush();
	QApplication::processEvents();
}

void Progress::incrementValue()
{
	_progressBar->setValue(_progressBar->value() + 1);
}

void Progress::installStopButton()
{
	_layout->addWidget(_stop, 0, Qt::AlignHCenter);
}

// Get
bool Progress::isStopped() const
{
	return ! _stop->isEnabled();
}

/*--------------------------------------------------------------------*\
		Protected
\*--------------------------------------------------------------------*/
void Progress::closeEvent(QCloseEvent* event)
{
	event->ignore();
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
void Progress::stop()
{
	_stop->setDisabled(true);
}
