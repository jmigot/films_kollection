
/**
 * QListWidgetItem which represents a film
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

#include "Film.h"
#include "Database.h"
#include "Utils.h"

#include <QDebug>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Film::Film(Utils *utils, const QString &filename, const QString &dateTimeAdd, bool isFilename)
	: _utils(utils), _title(filename), _dateTimeAdd(dateTimeAdd)
{
	// Members
	_id         = -1;
	_title      = _utils->titleFromFilename(_title);
	_duration   = 0;
	_note       = -1;
	_isComplete = false;
	_isSeen     = false;
	_isPart     = false;
	_idNextPart = -1;
	_isChecked  = false;

	if (isFilename)
		 _filename = filename;

	// Set text and icon
	this->setText(_title);
	this->setIcon(QIcon(":/images/cross"));

	// Film is uncomplete, set it bold in list
	QFont font;
	font.setWeight(QFont::Bold);
	this->setFont(font);
}

Film::Film(Utils *utils,
		   qint32 id,
		   const QString &title,
		   const QString &originalTitle,
		   const QString &countries,
		   const QString &genders,
		   qint16 duration,
		   const QString &year,
		   const QString &actors,
		   const QString &realisators,
		   const QString &synopsis,
		   const QString &trailerUrl,
		   float note,
		   const QString &comment,
		   bool isComplete,
		   bool isSeen,
		   const QString &dateTimeAdd,
		   const QString &filename,
		   bool isPart,
		   qint32 idNextPart)
	: _utils(utils),
	  _id(id),
	  _title(title),
	  _originalTitle(originalTitle),
	  _countries(countries),
	  _genders(genders),
	  _duration(duration),
	  _year(year),
	  _actors(actors),
	  _realisators(realisators),
	  _synopsis(synopsis),
	  _trailerUrl(trailerUrl),
	  _note(note),
	  _comment(comment),
	  _isComplete(isComplete),
	  _isSeen(isSeen),
	  _dateTimeAdd(dateTimeAdd),
	  _filename(filename),
	  _isPart(isPart),
	  _idNextPart(idNextPart)
{
	// Members
	_isChecked = false;

	// Set text and icon
	this->setText(_title);

	if (_isSeen)
		this->setIcon(QIcon(":/images/apply_green"));

	else
		this->setIcon(QIcon(":/images/cross"));

	// If uncomplete, film become bold in list
	if ( ! _isComplete)
	{
		QFont font;
		font.setWeight(QFont::Bold);
		this->setFont(font);
	}
}

Film::~Film()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Film()";
	#endif
}

void Film::checkIfComplete(bool jacketIsNull)
{
	this->setIsComplete( ! _title.isEmpty()         &&
						 ! _originalTitle.isEmpty() &&
						 ! _countries.isEmpty()     &&
						 ! _genders.isEmpty()       &&
						 _duration != 0             &&
						 ! _year.isEmpty()          &&
						 ! _actors.isEmpty()        &&
						 ! _realisators.isEmpty()   &&
						 ! _synopsis.isEmpty()      &&
						 ! jacketIsNull);
}

// Get
qint32 Film::id() const
{
	return _id;
}

QString Film::title() const
{
	return _title;
}

QString Film::titleFromFilename() const
{
	QString title(_utils->titleFromFilename(_filename));

	// If filename is empty, title is taken from _title
	if (title.isEmpty())
		title = _title;

	return title;
}

QString Film::originalTitle() const
{
	return _originalTitle;
}

QString Film::countries() const
{
	return _countries;
}

QString Film::genders() const
{
	return _genders;
}

qint16 Film::duration() const
{
	return _duration;
}

QString Film::year() const
{
	return _year;
}

QString Film::actors() const
{
	return _actors;
}

QString Film::realisators() const
{
	return _realisators;
}

QString Film::synopsis() const
{
	return _synopsis;
}

QPixmap Film::jacket() const
{
	QPixmap jacket;

	// Get jacket bytes from db
	QByteArray bytes(_utils->db()->jacket(_id));

	// Load bytes in QPixmap
	if ( ! bytes.isEmpty())
		jacket.loadFromData(bytes);

	return jacket;
}

QString Film::trailerUrl() const
{
	return _trailerUrl;
}

float Film::note() const
{
	return _note;
}

QString Film::comment() const
{
	return _comment;
}

bool Film::isComplete() const
{
	return _isComplete;
}

bool Film::isSeen() const
{
	return _isSeen;
}

QString Film::dateTimeAdd() const
{
	return _dateTimeAdd;
}

QString Film::filename() const
{
	return _filename;
}

QString Film::shortFilename() const
{
	QString shortFilename(_filename);

	// Take only string after the last "/"
	if (shortFilename.contains("/"))
		shortFilename = shortFilename.section("/", -1);

	return shortFilename;
}

QString Film::extension() const
{
	QString extension;

	if (_filename.contains("."))
		extension = "." + _filename.section(".", -1);

	return extension;
}

QString Film::path() const
{
	if (_filename.contains("/"))
	{
		// Remove film name and extension from filename
		QString path(_filename.section("/", 0, -2));

		// Add a "/" at end of path
		if ( ! path.endsWith("/"))
			path.append("/");

		return path;
	}

	return QString();
}

bool Film::isPart() const
{
	return _isPart;
}

qint32 Film::idNextPart() const
{
	return _idNextPart;
}

bool Film::isChecked() const
{
	return _isChecked;
}

// Set
void Film::setId(int id)
{
	_id = id;
}

void Film::setTitle(const QString &title)
{
	if (title.count() > 0)
	{
		_title = title;

		// Set list text
		this->setText(_title);
	}
}

void Film::setOriginalTitle(const QString &originalTitle)
{
	_originalTitle = originalTitle;
}

void Film::setCountries(const QString &countries)
{
	_countries = countries;
}

void Film::setGenders(const QString &genders)
{
	_genders = genders;
}

void Film::setDuration(qint16 duration)
{
	_duration = duration;
}

void Film::setYear(const QString &year)
{
	_year = year;
}

void Film::setActors(const QString &actors)
{
	_actors = actors;
}

void Film::setRealisators(const QString &realisators)
{
	_realisators = realisators;
}

void Film::setSynopsis(const QString &synopsis)
{
	_synopsis = synopsis;
}

void Film::setTrailerUrl(const QString &trailerUrl)
{
	_trailerUrl = trailerUrl;
}

void Film::setNote(float note)
{
	_note = note;
}

void Film::setComment(const QString &comment)
{
	_comment = comment;
}

void Film::setIsComplete(bool isComplete)
{
	_isComplete = isComplete;

	// Normal font
	QFont font;

	// If uncomplete, film is bold in list
	if ( ! _isComplete)
		font.setWeight(QFont::Bold);

	// Set font
	this->setFont(font);
}

void Film::setIsSeen(bool isSeen)
{
	_isSeen = isSeen;

	// Set icon depends on isSeen value
	if (isSeen)
		this->setIcon(QIcon(":/images/apply_green"));

	else
		this->setIcon(QIcon(":/images/cross"));
}

void Film::setFilename(const QString &filename)
{
	_filename = filename;
}

void Film::setIsPart(bool isPart)
{
	_isPart = isPart;
}

void Film::setIdNextPart(int idNextPart)
{
	// Possibles values are -1 and 0..n
	if (idNextPart < 0)
		idNextPart = -1;

	_idNextPart = idNextPart;
}

void Film::setPath(QString path)
{
	if ( ! _filename.isEmpty())
	{
		// Add a "/" at end of path
		if ( ! path.endsWith("/"))
			path.append("/");

		// Set filename to path + film filename and extension
		_filename = path + this->shortFilename();
	}
}

void Film::setIsChecked(bool isChecked)
{
	_isChecked = isChecked;

	// If film is checked, set icon to checked
	if (isChecked)
		this->setIcon(QIcon(":/images/apply_blue"));

	// Else set icon to seen/not seen
	else
	{
		if (_isSeen)
			this->setIcon(QIcon(":/images/apply_green"));

		else
			this->setIcon(QIcon(":/images/cross"));
	}
}
