
/**
 * QToolBar to propose a search engine for films informations
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

#include "SearchBar.h"
#include "Config.h"
#include "Film.h"
#include "MainWindow.h"
#include "Utils.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QStringListModel>
#include <QTime>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
SearchBar::SearchBar(Utils *utils)
	: _utils(utils)
{
	// Members
	_searchType    = new QComboBox;
	_searchEdit    = new QComboBox;
	_wordListModel = new QStringListModel(this);
	_completer     = new QCompleter(_wordListModel, _searchEdit);
	_allFilms      = new QPushButton(QIcon(":/images/apply_blue"), tr("All films"));
	_randomFilm    = new QPushButton(QIcon(":/images/bomb"), tr("Random film"));
	_seenFilms     = new QCheckBox(tr("Seen"));
	_notSeenFilms  = new QCheckBox(tr("Not seen"));
	_checkedFilms  = new QCheckBox(tr("Checked"));

	// Properties
	this->setMovable(false);

	// Items for search
	QStringList items;
	items << tr("Search")
		  << tr("Title")
		  << tr("Original title")
		  << tr("Country")
		  << tr("Gender")
		  << tr("Duration")
		  << tr("Year")
		  << tr("Actor")
		  << tr("Realisator")
		  << tr("Synopsis")
		  << tr("Note")
		  << tr("Comment")
		  << tr("Add date")
		  << tr("Filename")
		  << tr("Dir");

	_searchType->addItems(items);
	_searchType->setMaxVisibleItems(items.count());

	_searchEdit->setEditable(true);
	_searchEdit->setMaxVisibleItems(50);
	_searchEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	_searchEdit->installEventFilter(this);

	_completer->setCaseSensitivity(Qt::CaseInsensitive);
	_completer->setMaxVisibleItems(15);
	_searchEdit->setCompleter(_completer);

	// Check "seen film" and "not seen films"
	_seenFilms->setChecked(true);
	_notSeenFilms->setChecked(true);

	// Connects
	connect(_searchType, SIGNAL(currentIndexChanged(int)), this, SLOT(switchCompleter(int)));
	connect(_searchEdit, SIGNAL(editTextChanged(QString)), this, SLOT(search()));
	connect(_completer, SIGNAL(activated(QString)), this, SLOT(search()));
	connect(_allFilms, SIGNAL(clicked()), this, SLOT(displayAllFilms()));
	connect(_randomFilm, SIGNAL(clicked()), this, SLOT(displayRandomFilm()));
	connect(_seenFilms, SIGNAL(clicked()), this, SLOT(search()));
	connect(_notSeenFilms, SIGNAL(clicked()), this, SLOT(search()));
	connect(_checkedFilms, SIGNAL(clicked()), this, SLOT(search()));

	// Show widget
	this->addWidget(_searchType);
	this->addWidget(_searchEdit);
	this->addSeparator();
	this->addWidget(_allFilms);
	this->addWidget(_randomFilm);
	this->addSeparator();
	this->addWidget(new QLabel(tr("Show films : ")));
	this->addWidget(_seenFilms);
	this->addWidget(_notSeenFilms);
	this->addWidget(_checkedFilms);
}

SearchBar::~SearchBar()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~SearchBar()";
	#endif
}

void SearchBar::reload()
{
	// Save current text because switchCompleter() will clear it
	QString search(_searchEdit->currentText());

	// Reload completer
	this->switchCompleter(_searchType->currentIndex());
	_searchEdit->setEditText(search);

	// Reload search
	this->search();
}

void SearchBar::nextFilm() const
{
	// Get visible films
	QList<Film*> filmsList(_utils->filmsList(Fk::Visible));

	// If there is at least 1 film
	if (filmsList.count() > 0)
	{
		int i;

		// Search if a visible film is selected (we do a reverse in case a group of films is selected: the next one not selected will correctly be)
		for (i = filmsList.count() - 1; i >= 0; i--)
			if (filmsList.at(i)->isSelected())
				break;

		// Select the next film (0 in case no film is selected (i == -1) or i+1
		_utils->window()->switchFilm(filmsList.at((i + 1) % filmsList.count()), true);
	}
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
bool SearchBar::eventFilter(QObject *object, QEvent *event)
{
	QKeyEvent *keyEvent(dynamic_cast<QKeyEvent*>(event));

	// On  Return or Enter key press, go on next film
	if (object == _searchEdit && keyEvent != NULL && keyEvent->type() == QEvent::KeyPress && (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter))
		this->nextFilm();

	return false;
}

/*--------------------------------------------------------------------*\
		Public slots
\*--------------------------------------------------------------------*/
void SearchBar::search()
{
	// First, show all films (to clear any previous search)
	this->showAllFilms();

	// Get all films except parts
	QList<Film*> filmsList(_utils->filmsList(Fk::All));

	// If "checked films" is checked, hide unchecked films
	if (_checkedFilms->isChecked())
	{
		foreach (Film *film, filmsList)
			if ( ! film->isChecked())
				film->setHidden(true);
	}

	// If "seen films" is checked, hide not seen films
	if ( ! _seenFilms->isChecked())
	{
		foreach (Film *film, filmsList)
			if (film->isSeen())
				film->setHidden(true);
	}

	// If "not seen films" is checked, hide seen films
	if ( ! _notSeenFilms->isChecked())
	{
		foreach (Film *film, filmsList)
			if ( ! film->isSeen())
				film->setHidden(true);
	}

	// If search text exists, do a textual search on the correct selected field
	if ( ! _searchEdit->currentText().simplified().isEmpty())
	{
		// Escape pattern to unmatch all special regex caracters
		QString pattern(QRegExp::escape(_searchEdit->currentText().simplified()));

		// Replace accents
		pattern = pattern.replace(QRegExp("[aàâ]"), "[aàâ]");
		pattern = pattern.replace(QRegExp("[eéèê]"), "[eéèê]");
		pattern = pattern.replace(QRegExp("[iïî]"), "[iïî]");
		pattern = pattern.replace(QRegExp("[oô]"), "[oô]");
		pattern = pattern.replace(QRegExp("[uùû]"), "[uùû]");

		QRegExp regex(pattern, Qt::CaseInsensitive);

		// Get type of search
		int type = _searchType->currentIndex();

		// Search general (on almost all fields at the same time)
		if (type == 0)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() &&
					 ! film->title().contains(regex) &&
					 ! film->originalTitle().contains(regex) &&
					 ! film->countries().contains(regex) &&
					 ! film->genders().contains(regex) &&
					 ! film->year().contains(regex) &&
					 ! film->actors().contains(regex) &&
					 ! film->realisators().contains(regex) &&
					 ! film->synopsis().contains(regex) &&
					 ! film->comment().contains(regex) &&
					 ! film->filename().contains(regex))
					film->setHidden(true);
		}

		// Title
		else if (type == 1)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->title().contains(regex))
					film->setHidden(true);
		}

		// Original title
		else if (type == 2)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->originalTitle().contains(regex))
					film->setHidden(true);
		}

		// Countries
		else if (type == 3)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->countries().contains(regex))
					film->setHidden(true);
		}

		// Genders
		else if (type == 4)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->genders().contains(regex))
					film->setHidden(true);
		}

		// Duration
		else if (type == 5)
		{
			// Match a correct duration search
			QRegExp regex2("(~|>|>=|<|<=)?([0-9]+) h ([0-9]+)");
			int nbMin = 0;
			QString signe;

			if (_searchEdit->currentText().contains(regex2))
			{
				nbMin = regex2.cap(2).toInt() * 60 + regex2.cap(3).toInt();
				signe = regex2.cap(1);
			}

			foreach (Film *film, filmsList)
			{
				if ( ! film->isHidden())
				{
					// Create duration text
					QTime time(film->duration() / 60, film->duration() % 60);
					QString duration;

					if (film->duration())
						duration = time.toString("H 'h' mm");

					// If duration text match the regex
					if ( ! signe.isEmpty() && duration.contains(regex2))
					{
						int filmNbMin = regex2.cap(2).toInt() * 60 + regex2.cap(3).toInt();

						// Approximation
						if (signe == "~" && (filmNbMin <= nbMin - (nbMin / 10) || filmNbMin >= nbMin + (nbMin / 10)))
							film->setHidden(true);

						// Inferior or superior
						else if ((signe == ">"  && filmNbMin <= nbMin)||
								 (signe == ">=" && filmNbMin < nbMin) ||
								 (signe == "<"  && filmNbMin >= nbMin)||
								 (signe == "<=" && filmNbMin > nbMin))
							film->setHidden(true);
					}

					// If no signe, just match text
					else if ( ! duration.contains(regex))
						film->setHidden(true);
				}
			}
		}

		// Year
		else if (type == 6)
		{
			// Match a correct year search
			QRegExp regex2("(~|>|>=|<|<=)?([0-9]+)");
			int year = 0;
			QString signe;

			if (_searchEdit->currentText().contains(regex2))
			{
				year  = regex2.cap(2).toInt();
				signe = regex2.cap(1);
			}

			foreach (Film *film, filmsList)
			{
				if ( ! film->isHidden())
				{
					if ( ! signe.isEmpty() && film->year().contains(regex2))
					{
						int filmYear = regex2.cap(2).toInt();

						// Approximation
						if (signe == "~" && (filmYear <= year - 2 || filmYear >= year + 2))
							film->setHidden(true);

						// Inferior or superior
						if ((signe == ">" && filmYear <= year) ||
							(signe == ">=" && filmYear < year) ||
							(signe == "<" && filmYear >= year) ||
							(signe == "<=" && filmYear > year))
							film->setHidden(true);
					}

					// If no signe, just match text
					else if ( ! film->year().contains(regex))
						film->setHidden(true);
				}
			}
		}

		// Actors
		else if (type == 7)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->actors().contains(regex))
					film->setHidden(true);
		}

		// Realisators
		else if (type == 8)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->realisators().contains(regex))
					film->setHidden(true);
		}

		// Synopsis
		else if (type == 9)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->synopsis().contains(regex))
					film->setHidden(true);
		}

		// Note
		else if (type == 10)
		{
			// Match a correct year search
			QRegExp regex2("(~|>|>=|<|<=)?([0-9.,]+)");
			float note = 0;
			QString signe;

			if (_searchEdit->currentText().contains(regex2))
			{
				note  = regex2.cap(2).toFloat();
				signe = regex2.cap(1);
			}

			foreach (Film *film, filmsList)
			{
				if ( ! film->isHidden())
				{
					if ( ! signe.isEmpty() && film->note() != -1 && QString::number(film->note()).contains(regex2))
					{
						float filmNote = regex2.cap(2).toFloat();

						// Approximation
						if (signe == "~" && (filmNote <= note - 1 || filmNote >= note + 1))
							film->setHidden(true);

						// Inferior or superior
						if ((signe == ">"  && filmNote <= note)||
							(signe == ">=" && filmNote < note) ||
							(signe == "<"  && filmNote >= note)||
							(signe == "<=" && filmNote > note))
							film->setHidden(true);
					}

					// If no signe, just match text
					else if (QString::number(film->note(), 'f', 1) != QString::number(note, 'f', 1))
						film->setHidden(true);
				}
			}
		}

		// Comment
		else if (type == 11)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->comment().contains(regex))
					film->setHidden(true);
		}

		// DateTimeAdd
		else if (type == 12)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->dateTimeAdd().contains(regex))
					film->setHidden(true);
		}

		// Filename
		else if (type == 13)
		{
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->filename().contains(regex))
					film->setHidden(true);
		}

		// Paths
		else if (type == 14)
		{
			/*
			   Because regex pattern can contains sets of accents [], we have to remake it for this special case
			   film->path() must EXACTLY contains the regex
			 */
			foreach (Film *film, filmsList)
				if ( ! film->isHidden() && ! film->path().contains(QRegExp("^" + regex.pattern() + "$")))
					film->setHidden(true);
		}

		/*
		   This code will deselect all films then determine which film(s) should be selected in list widget
		*/

		// Get selected films
		QList<Film*> filmsList2(_utils->filmsList(Fk::Visible));
		Film *selectedFilm = NULL;

		// If there is at least 1 film visible
		if (filmsList2.count() > 0)
		{
			// Search if a film is selected in visible films to keep it selected
			foreach (Film *film, filmsList2)
			{
				if (film->isSelected())
				{
					selectedFilm = film;
					break;
				}
			}

			// If no film is selected, the first one will be
			if (selectedFilm == NULL)
				selectedFilm = filmsList2.at(0);

			// Deselect all films (to prevent hidden films to be still selected)
			foreach (Film *film, filmsList)
				film->setSelected(false);

			// Select film
			_utils->window()->switchFilm(selectedFilm, true);
		}
	}

	else
		_utils->window()->reloadCurrentFilm();
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
void SearchBar::switchCompleter(int type)
{
	// Create wordlist with an empty string to begin
	QStringList wordList;
	wordList << QString();

	// Get all films except parts
	QList<Film*> filmList(_utils->filmsList(Fk::All));

	// Title
	if (type == 1)
		foreach (Film *film, filmList)
			wordList << film->title();

	// Original title
	else if (type == 2)
		foreach (Film *film, filmList)
			wordList << film->originalTitle();

	// Countries
	else if (type == 3)
	{
		foreach (Film *film, filmList)
		{
			wordList << film->countries();

			if ( ! film->countries().isEmpty())
				foreach (QString country, film->countries().split(","))
					wordList << country.simplified();
		}
	}

	// Genders
	else if (type == 4)
	{
		foreach (Film *film, filmList)
		{
			wordList << film->genders();

			if ( ! film->genders().isEmpty())
				foreach (QString gender, film->genders().split(","))
					wordList << gender.simplified();
		}
	}

	// Duration
	else if (type == 5)
	{
		foreach (Film *film, filmList)
		{
			if (film->duration() > 0)
			{
				QTime time(film->duration() / 60, film->duration() % 60);
				wordList << time.toString("H 'h' mm");
			}
		}
	}

	// Year
	else if (type == 6)
		foreach (Film *film, filmList)
			wordList << film->year();

	// Actors
	else if (type == 7)
	{
		foreach (Film *film, filmList)
			if ( ! film->actors().isEmpty())
				foreach (QString actor, film->actors().split(","))
					wordList << actor.simplified();
	}

	// Realisators
	else if (type == 8)
	{
		foreach (Film *film, filmList)
			if ( ! film->realisators().isEmpty())
				foreach (QString realisator, film->realisators().split(","))
					wordList << realisator.simplified();
	}

	// Note
	else if (type == 10)
	{
		foreach (Film *film, filmList)
			if (film->note() != -1)
				wordList << QString::number(film->note());
	}

	// Comment
	else if (type == 11)
		foreach (Film *film, filmList)
			wordList << film->comment();

	// DateTimeAdd
	else if (type == 12)
		foreach (Film *film, filmList)
			wordList << film->dateTimeAdd();

	// Filename
	else if (type == 13)
		foreach (Film *film, filmList)
			wordList << film->shortFilename();

	// Path
	else if (type == 14)
		wordList << _utils->filmsPaths();

	// Remove duplicates, sort list, set it as completer
	wordList.removeDuplicates();
	wordList.sort();
	_wordListModel->setStringList(wordList);
	_searchEdit->clear();
	_searchEdit->addItems(wordList);
}

void SearchBar::displayAllFilms()
{
	this->showAllFilms();

	_seenFilms->setChecked(true);
	_notSeenFilms->setChecked(true);
	_checkedFilms->setChecked(false);
	_searchType->setCurrentIndex(0);
	_searchEdit->clear();
	_searchEdit->clearEditText();
}

void SearchBar::showAllFilms() const
{
	QList<Film*> filmsList(_utils->filmsList(Fk::All));

	foreach (Film *film, filmsList)
		film->setHidden(false);

	_utils->window()->reloadCurrentFilm();
}

void SearchBar::displayRandomFilm() const
{
	QList<Film*> filmsList(_utils->filmsList(Fk::Visible));

	if (filmsList.count() > 0)
	{
		_utils->window()->switchFilm(filmsList.at(qrand() % filmsList.count()), true);

		if (_utils->config()->value(Fk::LaunchRandomFilm).toBool())
			_utils->window()->launchFilm();
	}
}

