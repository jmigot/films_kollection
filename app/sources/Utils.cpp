
/**
 * Very important class on this program, it makes an access for all classes to all created classes (Network, Database, Config, Parser, MainWindow)
 * and propose some useful fonctions
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

#include "Utils.h"
#include "Config.h"
#include "Database.h"
#include "Film.h"
#include "MainWindow.h"
#include "Network.h"
#include "Parser.h"
#include "SearchBar.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <cmath>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Utils::Utils(MainWindow *window, Config* config)
	: _window(window), _config(config)
{
	// Members
	_database  = new Database(this);
	_network   = new Network;
	_parser    = new Parser(this);
	_searchBar = new SearchBar(this);
}

Utils::~Utils()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Utils()";
	#endif

	/*
	   Delete parts, others films will be automatically deleted by MainWindow::_listWidget as they are attached to it
	   We don't use this->removeAllFilms() because this method call a _window's method and we are not sure that _window object is still in memory
	*/
	foreach (Film* film, _filmsList)
		if (film->isPart())
			delete film;

	delete _parser;
	delete _network;
	delete _database;
	delete _config;
}

float Utils::roundNote(float note) const
{
	float newNote = 0.0;
	float rest    = std::modf(note, &newNote);

	// Round note to ~0.5
	if (rest > 0.7)
		newNote += 1.0;

	else if (rest > 0.3)
		newNote += 0.5;

	return newNote;
}

QString Utils::titleFromFilename(QString filename) const
{
	// Title is based on filename, which can be a valid filename or a simple film title, so we have to handle the valid filename's case
	if (filename.contains("/"))
		filename = filename.section("/", -1);

	if (filename.contains("."))
		filename = filename.section(".", 0, -2);

	return filename;
}

QPixmap Utils::scaleToHeight(const QPixmap &jacket, int scaleRatio) const
{
	// Choose scale ratio depending on parameter
	if (scaleRatio == Fk::JacketRatio)
		scaleRatio = 2 * _window->height() / 5;

	else if (scaleRatio == Fk::MiniJacketRatio)
		scaleRatio = _window->height() / 5;

	else if(scaleRatio == Fk::LogoRatio)
		scaleRatio = _window->height() / 2;

	// Scale jacket
	return jacket.scaledToHeight(scaleRatio, Qt::SmoothTransformation);
}

QPixmap Utils::scale(const QPixmap &note, int scaleRatio) const
{
	return note.scaled(scaleRatio, scaleRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void Utils::appendFilm(Film *film)
{
	_filmsList.append(film);
}

bool Utils::isInFilmsList(const QString &filename) const
{
	// Foreach film in QList, check filenames
	foreach (Film *film, _filmsList)
		if (film->filename() == filename)
			return true;

	return false;
}

QString Utils::searchFilename(const QString &filename) const
{
	// If filename is empty, no filename was found
	if (filename.isEmpty())
		return QString();

	// If filename exists, return it
	if (QFile::exists(filename))
		return filename;

	// Try the alternate dir path if exists
	if ( ! _config->value(Fk::OtherDirPath).toString().isEmpty())
	{
		QString shortFileName(filename);

		// Take only string after the last "/"
		if (shortFileName.contains("/"))
			shortFileName = shortFileName.section("/", -1);

		QStringList dirsList;
		QString otherDir(_config->value(Fk::OtherDirPath).toString());

		if ( ! otherDir.endsWith("/"))
			otherDir.append("/");

		this->allDirs(otherDir, dirsList);
		dirsList << otherDir;

		foreach (QString otherDir, dirsList)
			if (QFile::exists(otherDir + "/" + shortFileName))
				return otherDir + "/" + shortFileName;
	}

	// If Windows, scan all drives to find the film on an external drive (letter could have changed)
	// This will replace the 3 first letters of filename with c:/, d:/, e:/...
	#if defined(Q_WS_WIN)
		foreach (QFileInfo fileInfo, QDir::drives())
			if (QFile::exists(fileInfo.absolutePath() + filename.mid(3)))
				return fileInfo.absolutePath() + filename.mid(3);

	#endif

	// No filename was found
	return QString();
}

Film* Utils::film(int id) const
{
	// Search film corresponding to id
	foreach (Film *film, _filmsList)
		if (film->id() == id)
			return film;

	return NULL;
}

QList<Film*> Utils::filmParts(Film *film) const
{
	QList<Film*> filmParts;
	Film *currentFilm = film;

	// While the current film has a next part
	while (currentFilm->idNextPart() != -1)
	{
		// Current film become the next part, then append it to list
		currentFilm = this->film(currentFilm->idNextPart());
		filmParts.append(currentFilm);
	}

	return filmParts;
}

bool Utils::removeFile(Film *film) const
{
	/*
	   Try remove file from disk
	   We don't use this->searchFilename() because we are not sure the filename found is the right one !
	*/
	if (QFile::exists(film->filename()))
		return QFile::remove(film->filename());

	return true;
}

void Utils::removeFilm(Film *film)
{
	// Remove film from QList then delete it
	_filmsList.removeOne(film);
	delete film;
}

void Utils::removeAllFilms()
{
	// Delete parts first
	foreach (Film *film, _filmsList)
		if (film->isPart())
			delete film;

	// Then delete films from listWidget (only no-parts will be deleted here)
	_window->clearListWidget();

	// Clear QList (this is important, because pointers are now referencing nothing !)
	_filmsList.clear();
}

void Utils::scanDirectory(const QString &dirName, QStringList &filmsList) const
{
	QDir dir(dirName);
	int minimumFileSize = _config->value(Fk::MinimumFileSize).toInt();

	// Scan properties: all dirs and files, no symbolic links, sort with dirs first and by name, and filter with extensions list only
	dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	dir.setSorting(QDir::DirsFirst | QDir::Name);
	dir.setNameFilters(_config->value(Fk::Extensions).toStringList());

	foreach (QFileInfo itemInfo, dir.entryInfoList())
	{
		// Recursive scan over dirs
		if (itemInfo.isDir())
			this->scanDirectory(itemInfo.absoluteFilePath(), filmsList);

		// Add files that match to a film definition to list
		else if (itemInfo.isFile() && itemInfo.size() >= minimumFileSize)
		{
			QString fileName = itemInfo.canonicalFilePath();

			if ( ! this->isInFilmsList(fileName))
				filmsList << fileName;
		}
	}
}

void Utils::allDirs(const QString &dirName, QStringList &dirsList) const
{
	QDir dir(dirName);

	// Scan properties: all dirs, no symbolic links
	dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	foreach (QFileInfo itemInfo, dir.entryInfoList())
	{
		dirsList << itemInfo.canonicalFilePath() + "/";
		this->allDirs(itemInfo.canonicalFilePath(), dirsList);
	}
}

QStringList Utils::filmsPaths() const
{
	QStringList list;

	// Foreach film
	foreach (Film *film, _filmsList)
	{
		// Get film path
		QString path(film->path());

		// If path isn't already in list, append to it
		if ( ! path.isEmpty() && ! list.contains(path))
			list.append(path);
	}

	return list;
}

QList<Film*> Utils::filmsList(int filters) const
{
	QList<Film*> filmsList;

	// If filters ask for all films with parts included, add entire QList
	if ((filters & Fk::All) && (filters & Fk::Parts))
		filmsList.append(_filmsList);

	// Else if only parts are asked, append all parts
	else if (filters == Fk::Parts)
	{
		foreach (Film* film, _filmsList)
			if (film->isPart())
				filmsList.append(film);
	}

	// Else, we have to deal with filters
	else
	{
		foreach (Film* film, _filmsList)
		{
			// Parts will be dealed outside this foreach, so don't bother with it now
			if (film->isPart())
				continue;

			// If filters is all, add all films
			else if (filters & Fk::All)
				filmsList.append(film);

			// Else deal with filters: append film only if a film doesn't mismatch all filters wanted
			else if ( ! (((filters & Fk::Selected)        && ! film->isSelected())     ||
						 ((filters & Fk::Visible)         && film->isHidden())         ||
						 ((filters & Fk::Seen)            && ! film->isSeen())         ||
						 ((filters & Fk::NotSeen)         && film->isSeen())           ||
						 ((filters & Fk::Complete)        && ! film->isComplete())     ||
						 ((filters & Fk::Uncomplete)      && film->isComplete())       ||
						 ((filters & Fk::ContainingParts) && film->idNextPart() == -1) ||
						 ((filters & Fk::Checked)         && ! film->isChecked())      ||
						 ((filters & Fk::Unchecked)       && film->isChecked())))
				filmsList.append(film);
		}

		// Add parts if asked (only parts of taken films)
		if (filters & Fk::Parts)
			foreach (Film *film, filmsList)
				filmsList.append(this->filmParts(film));
	}

	return filmsList;
}

// Get
MainWindow* Utils::window() const
{
	return _window;
}

Config* Utils::config() const
{
	return _config;
}

Database* Utils::db() const
{
	return _database;
}

Network* Utils::network() const
{
	return _network;
}

Parser* Utils::parser() const
{
	return _parser;
}

SearchBar* Utils::searchBar() const
{
	return _searchBar;
}

// Set
void Utils::setConfig(Config *config)
{
	_config = config;
}
