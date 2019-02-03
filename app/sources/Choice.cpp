
/**
 * Film choice with basic infos
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

#include "Choice.h"
#include "Parser.h"
#include "SearchBar.h"
#include "Utils.h"

#include <QDebug>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Choice::Choice(Utils *utils, Film *film,
			   const QString &searchTitle,
			   const QString &title,
			   const QString &originalTitle,
			   const QString &year,
			   const QString &actors,
			   const QString &realisators,
			   const QPixmap &jacket,
			   const QString &url)
	: _utils(utils), _film(film), _searchTitle(searchTitle), _title(title), _originalTitle(originalTitle), _year(year), _actors(actors), _realisators(realisators), _jacket(jacket), _url(url)
{
}

Choice::~Choice()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Choice()";
	#endif
}

// Get
QString Choice::searchTitle() const
{
	return _searchTitle;
}

QString Choice::title() const
{
	return _title;
}

QString Choice::originalTitle() const
{
	return _originalTitle;
}

QString Choice::year() const
{
	return _year;
}

QString Choice::actors() const
{
	return _actors;
}

QString Choice::realisators() const
{
	return _realisators;
}

QPixmap Choice::jacket() const
{
	return _jacket;
}

QString Choice::url() const
{
	return _url;
}

/*--------------------------------------------------------------------*\
		Public slots
\*--------------------------------------------------------------------*/
void Choice::choose() const
{
	// Update film with corresponding code and reload search engine
	_utils->parser()->updateWithAllocineUrl(_film, _url);
	_utils->searchBar()->reload();
}

