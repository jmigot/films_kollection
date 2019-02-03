
/**
 * QWidget to show statistics of collection
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

#include "Statistics.h"
#include "Config.h"
#include "Database.h"
#include "Film.h"
#include "MainWindow.h"
#include "Utils.h"

#include <QDebug>
#include <QFile>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Statistics::Statistics(Utils *utils)
	: Widget(utils, tr("Statistics"), QPixmap(":/images/statistics"))
{
	// Members
	_boxSeenFilms    = new QGroupBox;
	_boxDuration     = new QGroupBox;
	_boxSize         = new QGroupBox;
	_layoutSeenFilms = new QVBoxLayout(_boxSeenFilms);
	_layoutDuration  = new QFormLayout(_boxDuration);
	_layoutSize      = new QFormLayout(_boxSize);
	_notesLayout     = new QHBoxLayout;

	for (int i = 0; i < 5; i++)
		_notes.append(new QLabel);

	// Variables
	QList<Film*> filmsList(_utils->filmsList(Fk::All));
	QProgressBar* seenFilms(new QProgressBar);
	QFile file(_utils->db()->dbname());
	int nbFilms           = filmsList.count();
	int nbSeenFilms       = 0;
	int duration          = 0;
	int durationSeenFilms = 0;
	int dbSize            = 0;
	float note            = 0.0;
	int nbNotes           = 0;

	// Properties
	_layoutDuration->setLabelAlignment(Qt::AlignLeft);
	_layoutSize->setLabelAlignment(Qt::AlignLeft);

	// DB Size
	if (file.exists())
		dbSize = file.size();

	// Progress bars
	if (nbFilms > 0)
		seenFilms->setRange(0, nbFilms);

	// Calculs on collection
	foreach (Film *film, filmsList)
	{
		// Seen films
		if (film->isSeen())
		{
			nbSeenFilms++;
			durationSeenFilms += film->duration();
		}

		// Duration
		duration += film->duration();

		// Average note
		if (film->note() >= 0)
		{
			note += film->note();
			nbNotes++;
		}
	}

	// Seen films
	seenFilms->setValue(nbSeenFilms);

	// Average note (avoid division by zero)
	if (nbNotes > 0)
		note /= nbNotes;

	// Create round note, save original note to show it in text label
	float note2 = _utils->roundNote(note);

	for (int i = 0; i < 5; i++)
	{
		if (note2 >= 2 * i + 2)
			_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_100"), Fk::NoteRatio));

		else if (note2 >= 2.0 * i + 1.5)
			_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_75"), Fk::NoteRatio));

		else if (note2 >= 2 * i + 1)
			_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_50"), Fk::NoteRatio));

		else if (note2 >= 2.0 * i + 0.5)
			_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_25"), Fk::NoteRatio));

		else
			_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_0"), Fk::NoteRatio));
	}

	// Layouts
	_layout->addWidget(new QLabel(tr("You have <b>%n</b> film(s)", "", nbFilms) + tr(" (and <b>%n</b> part(s)).", "", _utils->filmsList(Fk::Parts).count())));
	_layout->addLayout(_notesLayout);

	_notesLayout->addWidget(new QLabel(tr("Average note: ")));
	_notesLayout->setAlignment(Qt::AlignLeft);

	for (int i = 0; i < 5; i++)
		_notesLayout->addWidget(_notes.at(i));

	_notesLayout->addWidget(new QLabel("(" + QString::number(note, 'f', 1) + ")"));

	_layout->addWidget(_boxSeenFilms);

	_layoutSeenFilms->addWidget(new QLabel(tr("Seen films")));
	_layoutSeenFilms->addWidget(seenFilms);
	_layoutSeenFilms->addWidget(new QLabel(tr("Seen: ") + "<b>" + QString::number(nbSeenFilms) + "</b>" + tr(" - To see: ") + "<b>" + QString::number(nbFilms - nbSeenFilms) + "</b>"));

	_layout->addWidget(_boxDuration);

	_layoutDuration->addRow(tr("Duration of films: "), new QLabel("<b>" + this->duration(duration) + "</b>"));
	_layoutDuration->addRow(tr("Duration of seen films: "), new QLabel("<b>" + this->duration(durationSeenFilms) + "</b>"));
	_layoutDuration->addRow(tr("Duration of not seen films: "), new QLabel("<b>" + this->duration(duration - durationSeenFilms) + "</b>"));

	_layout->addWidget(_boxSize);

	_layoutSize->addRow(tr("Collection size: "), new QLabel("<b>" + QString::number(dbSize / 1000000.0, 'f', 2) + " Mo</b>"));

	_layout->addWidget(_close, 0, Qt::AlignHCenter);

	this->display();
}

Statistics::~Statistics()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Statistics()";
	#endif
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
QString Statistics::duration(int duration) const
{
	if (duration == 0)
		return QString("0");

	QString result;

	// If duration is in days
	if (duration > 1440)
	{
		int nbDays = duration / 1440;
		result += tr("%n day(s)", "", nbDays);
	}

	// If duration is in hours
	if (duration > 60)
	{
		int nbHours = (duration % 1440) / 60;
		result += " " + QString("0" + QString::number(nbHours)).right(2) + " h ";
	}

	// Add minutes
	int nbMinutes = duration % 60;
	result += QString("0" + QString::number(nbMinutes)).right(2);

	return result;
}
