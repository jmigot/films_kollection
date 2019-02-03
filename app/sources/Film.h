
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

#ifndef FILM_H
#define FILM_H

#include <QListWidgetItem>

class Utils;

class Film: public QListWidgetItem
{
	private:
		Utils*  _utils;
		qint32  _id;
		QString _title;
		QString _originalTitle;
		QString _countries;
		QString _genders;
		qint16  _duration;
		QString _year;
		QString _actors;
		QString _realisators;
		QString _synopsis;
		QString _trailerUrl;
		float   _note;
		QString _comment;
		bool    _isComplete;
		bool    _isSeen;
		QString _dateTimeAdd;
		QString _filename;
		bool    _isPart;
		qint32  _idNextPart;
		bool    _isChecked;

	public:
		Film(Utils *utils, const QString &filename, const QString &dateTimeAdd, bool isFilename = true);
		Film(      Utils   *utils,
				   qint32  id,
			 const QString &title,
			 const QString &originalTitle,
			 const QString &countries,
			 const QString &genders,
				   qint16  duration,
			 const QString &year,
			 const QString &actors,
			 const QString &realisators,
			 const QString &synopsis,
			 const QString &trailerUrl,
				   float   note,
			 const QString &comment,
				   bool    isComplete,
				   bool    isSeen,
			 const QString &dateTimeAdd,
			 const QString &filename,
				   bool    isPart,
				   qint32  idNextPart);
		~Film();

		void checkIfComplete(bool jacketIsNull);

		// Get
		qint32  id()                const;
		QString title()             const;
		QString titleFromFilename() const;
		QString originalTitle()     const;
		QString countries()         const;
		QString genders()           const;
		qint16  duration()          const;
		QString year()              const;
		QString actors()            const;
		QString realisators()       const;
		QString synopsis()          const;
		QPixmap jacket()            const;
		QString trailerUrl()        const;
		float   note()              const;
		QString comment()           const;
		bool    isComplete()        const;
		bool    isSeen()            const;
		QString dateTimeAdd()       const;
		QString filename()          const;
		QString shortFilename()     const;
		QString extension()         const;
		QString path()              const;
		bool    isPart()            const;
		qint32  idNextPart()        const;
		bool    isChecked()         const;

		// Set
		void setId(int id);
		void setTitle(const QString &title);
		void setOriginalTitle(const QString &originalTitle);
		void setCountries(const QString &countries);
		void setGenders(const QString &genders);
		void setDuration(qint16 duration);
		void setYear(const QString &year);
		void setActors(const QString &actors);
		void setRealisators(const QString &realisators);
		void setSynopsis(const QString &synopsis);
		void setTrailerUrl(const QString &trailerUrl);
		void setNote(float note);
		void setComment(const QString &comment);
		void setIsComplete(bool isComplete);
		void setIsSeen(bool isSeen);
		void setFilename(const QString &filename);
		void setPath(QString path);
		void setIsPart(bool isPart);
		void setIdNextPart(int idNextPart);
		void setIsChecked(bool isChecked);
};

#endif
