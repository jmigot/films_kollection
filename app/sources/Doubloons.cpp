
/**
 * QWidget to find film doubloons in collection
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

#include "Doubloons.h"
#include "Film.h"
#include "Utils.h"

#include <QDebug>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Doubloons::Doubloons(Utils *utils)
	: Widget(utils, tr("Search for doubloons"), QPixmap(":/images/doubloons"), false, false) // non-modal
{
	// Members
	_labelProgress = new QLabel;
	_progress      = new QProgressBar;
	_results       = new QTextEdit;

	// Properties
	_results->setReadOnly(true);

	// Layouts
	_layout->addWidget(_labelProgress);
	_layout->addWidget(_progress);
	_layout->addWidget(_results);
	_layout->addWidget(_close, 0, Qt::AlignHCenter);

	// Show widget
	this->display();

	// Launch scan
	this->scan();
}

Doubloons::~Doubloons()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Doubloons()";
	#endif
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
void Doubloons::scan()
{
	// Get all films  (without parts)
	QList<Film*> filmsList(_utils->filmsList(Fk::All));
	QStringList doubloons;

	_labelProgress->setText(tr("Comparaison between titles..."));
	_progress->setRange(0, filmsList.count());
	_progress->setValue(0);

	// Double foreach
	for (int i = 0; i < filmsList.count() - 1; i++)
	{
		for (int j = i + 1; j < filmsList.count(); j++)
		{
			// If doubloon isn't already in list, add it and break this loop because it can only catch same titles if continue
			if (filmsList.at(i)->title() == filmsList.at(j)->title())
			{
				doubloons.append(filmsList.at(i)->title());
				break;
			}
		}

		_progress->setValue(i + 1);
	}

	doubloons.removeDuplicates();

	// If no doubloons detected, display it
	if (doubloons.isEmpty())
		_labelProgress->setText(tr("Scan is over - <b>No doubloon found</b>"));

	// Else add each doubloon in result text
	else
	{
		_labelProgress->setText(tr("Scan is over - <b>%n doubloon(s) found</b>", "", doubloons.size()));

		foreach (QString doubloon, doubloons)
			_results->append(doubloon);
	}

	_progress->setValue(_progress->maximum());
}
