
/**
 * QWidget to display film informations
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

#include "FilmView.h"
#include "Config.h"
#include "Film.h"
#include "MainWindow.h"
#include "Utils.h"

#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
FilmView::FilmView(Utils *utils)
	: _utils(utils)
{
	// Members
	_layout             = new QVBoxLayout(this);
	_title              = new QLabel;
	_informationsLayout = new QGridLayout;
	_jacketLabel        = new QLabel;
	_informations       = new QTextEdit;
	_noteLayout         = new QHBoxLayout;

	for (int i = 0; i < 5; i++)
		_notes.append(new QLabel);

	_note          = new QLabel;
	_synopsis      = new QTextEdit;
	_informations2 = new QLabel;
	_buttonsLayout = new QHBoxLayout;
	_switchSeen    = new QPushButton;
	_launchTrailer = new QPushButton(QIcon(":/images/launchTrailer"), tr("Launch trailer"));
	_launchFilm    = new QPushButton(QIcon(":/images/launchFilm"), tr("Launch film"));

	// Properties
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->installEventFilter(this);

	_title->setStyleSheet("color: #5555CC");
	_title->setWordWrap(true);

	// QTextEdit are transparent
	QPalette palette;
	palette.setColor(QPalette::Base, QColor(0, 0, 0, 1));

	_informations->setReadOnly(true);
	_informations->setPalette(palette);
	_informations->setFrameShape(QTextEdit::NoFrame);

	_synopsis->setReadOnly(true);
	_synopsis->setPalette(palette);
	_synopsis->setFrameShape(QTextEdit::NoFrame);

	_informations2->setWordWrap(true);

	// Connects
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), _utils->window(), SLOT(displayMenu(QPoint)));
	connect(_launchTrailer, SIGNAL(clicked()), _utils->window(), SLOT(launchTrailer()));
	connect(_launchFilm, SIGNAL(clicked()), _utils->window(), SLOT(launchFilm()));
	connect(_switchSeen, SIGNAL(clicked()), _utils->window(), SLOT(switchSeen()));

	// Layouts
	_layout->addWidget(_title);
	_layout->addLayout(_informationsLayout);

	_informationsLayout->addWidget(_jacketLabel, 0, 0, 6, 1);
	_informationsLayout->addWidget(_informations, 0, 1, 6, 1);
	_informationsLayout->addLayout(_noteLayout, 5, 1);

	for (int i = 0; i < 5; i++)
		_noteLayout->addWidget(_notes.at(i));

	_noteLayout->addWidget(_note);

	_noteLayout->setAlignment(Qt::AlignLeft);

	_layout->addWidget(_synopsis);
	_layout->addWidget(_informations2);
	_layout->addLayout(_buttonsLayout);

	_buttonsLayout->addWidget(_launchFilm);
	_buttonsLayout->addWidget(_launchTrailer);
	_buttonsLayout->addWidget(_switchSeen);
}

FilmView::~FilmView()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~FilmView()";
	#endif

	// Delete menu if still exists (in case program is quitted when a film with parts is shown)
	if (_launchTrailer->menu() != NULL)
		delete _launchTrailer->menu();

	if (_launchFilm->menu() != NULL)
		delete _launchFilm->menu();
}

void FilmView::setFilm(Film *film)
{
	// Title (escape html tag '<' else it won't appear)
	_title->setText("<h1>" + film->title().replace("<", "&lt;") + "</h1><br />");
	_title->setAlignment(Qt::AlignCenter);

	// Jacket
	QPixmap jacket(film->jacket());

	// If jacket is null, take default jacket
	if (jacket.isNull())
		jacket.load(":/images/defaultJacket");

	_jacketLabel->setPixmap(_utils->scaleToHeight(jacket, Fk::JacketRatio));

	// Informations
	QTime time(film->duration() / 60, film->duration() % 60);
	QString duration;

	if (film->duration() > 0)
		duration = time.toString("H 'h' mm");

	QString actors;

	// If config allow max actors, take all actors
	if (_utils->config()->value(Fk::NbActorsToDisplay).toInt() == _utils->config()->max(Fk::NbActorsToDisplay).toInt())
		actors = film->actors();

	// Else take only config number actors
	else
	{
		// Create actors list
		QStringList actorsList(film->actors().split(","));

		// Foreach actor from list from 0 to config nb, add actor to string
		foreach (QString actor, actorsList.mid(0, _utils->config()->value(Fk::NbActorsToDisplay).toInt()))
			actors += actor.simplified() + ", ";

		actors.chop(2);
	}

	QString informations("<b>" + tr("Original title: ") + "</b>" + film->originalTitle() + "<br /><br />" +
						 "<b>" + tr("Countries: ")      + "</b>" + film->countries()     + "<br /><br />" +
						 "<b>" + tr("Gender: ")         + "</b>" + film->genders()       + "<br /><br />" +
						 "<b>" + tr("Duration: ")       + "</b>" + duration              + "<br /><br />" +
						 "<b>" + tr("Year: ")           + "</b>" + film->year()          + "<br /><br /><br />" +
						 "<b>" + tr("Realized by: ")    + "</b>" + film->realisators()   + "<br /><br />" +
						 "<b>" + tr("With: ")           + "</b>" + actors                + "<br /><br />");

	_informations->setText(informations);

	// Note
	float note = _utils->roundNote(film->note());

	// Set note icons
	for (int i = 0; i < 5; i++)
	{
		// If note isn't defined, hide note icons
		_notes.at(i)->setHidden(film->note() == -1);

		// Else
		if (film->note() != -1)
		{
			// Take either 0, 25, 50, 75 or 100% of icon
			if (note >= 2 * i + 2)
				_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_100"), Fk::NoteRatio));

			else if (note >= 2.0 * i + 1.5)
				_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_75"), Fk::NoteRatio));

			else if (note >= 2 * i + 1)
				_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_50"), Fk::NoteRatio));

			else if (note >= 2.0 * i + 0.5)
				_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_25"), Fk::NoteRatio));

			else
				_notes.at(i)->setPixmap(_utils->scale(QPixmap(":/images/star_0"), Fk::NoteRatio));
		}
	}

	// Add note string to labels
	if (film->note() != -1)
		_note->setText("(" + QString::number(film->note()) + ")");

	else
		_note->clear();

	// Synopsis
	QString synopsis(film->synopsis());

	// Replace special tags with html tags
	synopsis = synopsis.replace(QRegExp("\\[(b|i|/b|/i|b */|br|/br)\\]"), "<\\1>");

	// Set synopsis
	_synopsis->setHtml("<br /><b>" + tr("Synopsis: ") + "</b><br />" + synopsis);

	// Informations 2
	QString filename;

	// If config does'nt allow show full path name, take only film filename and extension
	if (_utils->config()->value(Fk::ShowFullPathName).toBool())
		filename = film->filename();

	else
		filename = film->shortFilename();

	if ( ! filename.isEmpty())
		filename = "<br /><b>" + tr("File: ") + "</b>" + filename;

	QString parts;
	int nbParts = _utils->filmParts(film).count() + 1;

	if (nbParts > 1)
		parts = tr(" (%1 parts)").arg(QString::number(nbParts));

	QString comment;

	if( ! film->comment().isEmpty())
		comment = "<b>" + tr("Comment: ") + "</b>" + film->comment();

	_informations2->setText(comment + filename + parts);

	// If film has parts, install menu with all parts
	if (film->idNextPart() != -1)
	{
		// Get all parts + film
		QList<Film*> filmsList(_utils->filmParts(film));
		filmsList.prepend(film);

		// Launch trailer
		QMenu *menuTrailer(new QMenu);
		QList<QAction*> actionsTrailer;

		// Foreach film in parts with valid filename, add an action to the menu
		// ActionFilm is a QAction that can contains a pointer to the film, so MainWindow::launchTrailer() can retrieve trailerUrl
		foreach (Film* film, filmsList)
			if ( ! film->trailerUrl().isEmpty())
				actionsTrailer.append(new ActionFilm(_utils, menuTrailer, QIcon(":/images/launchTrailer"), film, Fk::LaunchTrailer));

		menuTrailer->addActions(actionsTrailer);
		_launchTrailer->setMenu(menuTrailer);
		_launchTrailer->setEnabled(actionsTrailer.count() > 0);

		// Launch film
		QMenu *menuFilm(new QMenu);
		QList<QAction*> actionsFilm;

		// Foreach film in parts with valid filename, add an action to the menu
		// ActionFilm is a QAction that can contains a pointer to the film, so MainWindow::launchFilm() can retrieve filename
		foreach (Film* film, filmsList)
			if ( ! _utils->searchFilename(film->filename()).isEmpty())
				actionsFilm.append(new ActionFilm(_utils, menuFilm, QIcon(":/images/launchFilm"), film, Fk::LaunchFilm));

		menuFilm->addActions(actionsFilm);
		_launchFilm->setMenu(menuFilm);
		_launchFilm->setEnabled(actionsFilm.count() > 0);
	}

	// Else, delete menu
	else
	{
		// Launch trailer
		_launchTrailer->setEnabled( ! film->trailerUrl().isEmpty());

		if (_launchTrailer->menu() != NULL)
		{
			delete _launchTrailer->menu();
			_launchTrailer->update();
		}

		// Launch film
		_launchFilm->setEnabled( ! _utils->searchFilename(film->filename()).isEmpty());

		if (_launchFilm->menu() != NULL)
		{
			delete _launchFilm->menu();
			_launchFilm->update();
		}
	}

	// Seen icon/text
	if (film->isSeen())
	{
		_switchSeen->setText(tr("Swich to not seen"));
		_switchSeen->setIcon(QIcon(":/images/cross"));
	}

	else
	{
		_switchSeen->setText(tr("Switch to already seen"));
		_switchSeen->setIcon(QIcon(":/images/apply_green"));
	}
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
bool FilmView::eventFilter(QObject *object, QEvent *event)
{
	QMouseEvent *mouseEvent(dynamic_cast<QMouseEvent*>(event));

	if (object == this && mouseEvent != NULL && mouseEvent->type() == QMouseEvent::MouseButtonDblClick)
		_utils->window()->launchFilm();

	return false;
}

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
ActionFilm::ActionFilm(Utils *utils, QMenu *menu, const QIcon &icon, Film *film, int type)
	: QAction(icon, film->titleFromFilename(), menu), _film(film)
{
	if (type == Fk::LaunchTrailer)
		connect(this, SIGNAL(triggered()), utils->window(), SLOT(launchTrailer()));

	else if (type == Fk::LaunchFilm)
		connect(this, SIGNAL(triggered()), utils->window(), SLOT(launchFilm()));
}

ActionFilm::~ActionFilm()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~ActionFilm()";
	#endif
}

Film* ActionFilm::film() const
{
	return _film;
}
