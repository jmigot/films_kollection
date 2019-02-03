
/**
 * Class to manage database
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

#include "Database.h"
#include "Film.h"
#include "Progress.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Database::Database(Utils *utils)
	: _utils(utils)
{
	// Load SQLite driver
	_db    = QSqlDatabase::addDatabase("QSQLITE");
	_query = NULL;
}

Database::~Database()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Database()";
	#endif

	// Close connection
	this->clear();
}

bool Database::compact()
{
	// Clean erased data from database (which remains on disk when deleted)
	bool result = this->exec("VACUUM");
	this->clear();
	return result;
}

bool Database::create()
{
	// Create table films if not exists
	bool result = this->exec("CREATE TABLE IF NOT EXISTS films("
							 "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
							 "title          TEXT,"
							 "originalTitle  TEXT,"
							 "countries      TEXT,"
							 "genders        TEXT,"
							 "duration       INTEGER DEFAULT 0,"
							 "year           TEXT,"
							 "actors         TEXT,"
							 "realisators    TEXT,"
							 "synopsis       TEXT,"
							 "jacket         NONE,"
							 "trailerUrl     TEXT,"
							 "note           NUMERIC DEFAULT -1,"
							 "comment        TEXT,"
							 "isComplete     INTEGER DEFAULT 0,"
							 "isSeen         INTEGER DEFAULT 0,"
							 "dateTimeAdd    TEXT,"
							 "filename       TEXT,"
							 "isPart         INTEGER DEFAULT 0,"
							 "idNextPart     INTEGER DEFAULT -1)");
	this->clear();
	return result;
}

bool Database::loadFilms(QList<Film*> &filmsList)
{
	bool result = false;

	// Fetch number of films
	int nbFilms = 0;

	if (this->exec("SELECT COUNT(rowid) AS nbFilms FROM films") && _query->next())
		nbFilms = _query->value(0).toInt();

	this->clear();

	// Select all films
	if (this->exec("SELECT id, title, originalTitle, countries, genders, duration, year, actors, realisators, synopsis, trailerUrl, note,"
				   "comment, isComplete, isSeen, dateTimeAdd, filename, isPart, idNextPart FROM films ORDER BY title"))
	{
		// Progress bar for waiting
		Progress progress(QObject::tr("Open collection"), QObject::tr("Opening collection..."), nbFilms);

		// Browse all results
		while (_query->next())
		{
			// Add a new Film() to filmsList
			filmsList.append(new Film(_utils,
									  _query->value(0).toInt(),
									  _query->value(1).toString(),
									  _query->value(2).toString(),
									  _query->value(3).toString(),
									  _query->value(4).toString(),
									  _query->value(5).toInt(),
									  _query->value(6).toString(),
									  _query->value(7).toString(),
									  _query->value(8).toString(),
									  _query->value(9).toString(),
									  _query->value(10).toString(),
									  _query->value(11).toFloat(),
									  _query->value(12).toString(),
									  _query->value(13).toBool(),
									  _query->value(14).toBool(),
									  _query->value(15).toString(),
									  _query->value(16).toString(),
									  _query->value(17).toBool(),
									  _query->value(18).toInt()));

			progress.incrementValue();
		}

		result = true;
	}

	this->clear();
	return result;
}

QByteArray Database::jacket(int id)
{
	QByteArray result;

	// Select jacket by id
	QVariantList vars;
	vars << id;

	if (this->exec("SELECT jacket FROM films WHERE id = ?", vars))
	{
		if (_query->next())
			result = _query->value(0).toByteArray();

		else
			_error = _query->lastError().text();
	}

	this->clear();
	return result;
}

bool Database::insertFilm(Film *film)
{
	bool result = false;

	// Create vars list for prepared query
	QVariantList vars;
	vars << film->title() << film->dateTimeAdd() << film->filename();

	// Insert film
	if (this->exec("INSERT INTO films(title, dateTimeAdd, filename) VALUES(?, ?, ?)", vars))
	{
		QVariant id(_query->lastInsertId());

		if (id.isValid())
		{
			film->setId(id.toInt());
			result = true;
		}
	}

	this->clear();
	return result;
}

bool Database::updateFilm(Film *film, bool updateJacket, const QByteArray &jacket)
{
	// Create vars list for prepared query
	int isComplete = (int)film->isComplete();
	int isSeen     = (int)film->isSeen();
	int isPart     = (int)film->isPart();

	QVariantList vars;
	vars << film->title()
		 << film->originalTitle()
		 << film->countries()
		 << film->genders()
		 << film->duration()
		 << film->year()
		 << film->actors()
		 << film->realisators()
		 << film->synopsis();

	if (updateJacket)
		vars << jacket;

	vars << film->trailerUrl()
		 << film->note()
		 << film->comment()
		 << isComplete
		 << isSeen
		 << film->dateTimeAdd()
		 << film->filename()
		 << isPart
		 << film->idNextPart()
		 << film->id();

	// Update film
	QString query("UPDATE films SET "
				  "title = ?,"
				  "originalTitle = ?,"
				  "countries = ?,"
				  "genders = ?,"
				  "duration = ?,"
				  "year = ?,"
				  "actors = ?,"
				  "realisators = ?,"
				  "synopsis = ?,");

	if (updateJacket)
		query += "jacket = ?,";

	query += "trailerUrl = ?,"
			 "note = ?,"
			 "comment = ?,"
			 "isComplete = ?,"
			 "isSeen = ?,"
			 "dateTimeAdd = ?,"
			 "filename = ?,"
			 "isPart = ?,"
			 "idNextPart = ? "
			 "WHERE id = ?";

	// Execute query
	bool result = this->exec(query, vars);
	this->clear();
	return result;
}

bool Database::removeFilm(Film *film)
{
	QVariantList vars;
	vars << film->id();

	bool result = this->exec("DELETE FROM films WHERE id = ?", vars);
	this->clear();
	return result;
}

// Get
QString Database::error() const
{
	return _error;
}

QString Database::dbname() const
{
	return _db.databaseName();
}

// Set
void Database::setDatabaseName(const QString &filename)
{
	this->clear();
	_db.setDatabaseName(filename);
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
bool Database::open()
{
	_error.clear();

	// If db is already opened, close connection
	if (_db.isOpen())
		this->clear();

	// Try opening db
	if ( ! _db.open())
	{
		_error = _db.lastError().text();
		return false;
	}

	return true;
}

bool Database::exec(const QString &query)
{
	if (this->open())
	{
		_query = new QSqlQuery;

		// This is very important for select queries
		_query->setForwardOnly(true);

		// Exec query
		if (_query->exec(query))
			return true;

		_error = _query->lastError().text();
	}

	return false;
}

bool Database::exec(const QString &query, const QVariantList &vars)
{
	if (this->open())
	{
		_query = new QSqlQuery;

		// This is very important for select queries
		_query->setForwardOnly(true);

		// Prepare query
		if ( ! _query->prepare(query))
			_error = _query->lastError().text();

		else
		{
			// Bind vars
			foreach (QVariant var, vars)
				_query->addBindValue(var);

			// Exec query
			if (_query->exec())
				return true;

			_error = _query->lastError().text();
		}
	}

	return false;
}

void Database::clear()
{
	if (_query != NULL)
	{
		// Clear and finish query
		_query->finish();
		_query->clear();

		// Delete query and set it to NULL for further comparaisons
		delete _query;
		_query = NULL;
	}

	_db.close();
}
