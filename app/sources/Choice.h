
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

#ifndef CHOICE_H
#define CHOICE_H

#include <QPixmap>

class Utils;
class Film;

class Choice: public QObject
{
	Q_OBJECT

	private:
		Utils*  _utils;
		Film*   _film;
		QString _searchTitle;
		QString _title;
		QString _originalTitle;
		QString _year;
		QString _actors;
		QString _realisators;
		QPixmap _jacket;
		QString _url;

	public:
		Choice(      Utils   *utils,
					 Film    *film,
			   const QString &searchTitle,
			   const QString &title,
			   const QString &originalTitle,
			   const QString &year,
			   const QString &actors,
			   const QString &realisators,
			   const QPixmap &jacket,
			   const QString &url);
		~Choice();

		// Get
		QString searchTitle()   const;
		QString title()         const;
		QString originalTitle() const;
		QString year()          const;
		QString actors()        const;
		QString realisators()   const;
		QPixmap jacket()        const;
		QString url()           const;

	public slots:
		void choose() const;
};

#endif
