
/**
 * QWidget to manage added/removed films from files
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

#ifndef SCAN_DIRS_H
#define SCAN_DIRS_H

#include "Widget.h"

#include <QListWidgetItem>

class Film;
class Utils;

class QGridLayout;
class QProgressBar;

class ScanDirs: public Widget
{
	Q_OBJECT

	private:
		QGridLayout*  _gridLayout;
		QListWidget*  _newFilmsList;
		QListWidget*  _oldFilmsList;
		QPushButton*  _addNewFilms;
		QPushButton*  _removeOldFilms;
		QLabel*       _progressLabel;
		QProgressBar* _progress;

	public:
		ScanDirs(Utils *utils);
		~ScanDirs();

	private:
		void scan();
		bool contains(QString title);
		void updateVisible();
		bool eventFilter(QObject *object, QEvent *event);

	private slots:
		void addNewFilms();
		void removeOldFilms();
};

class MiniFilm: public QListWidgetItem
{
	private:
		Film* _film;

	public:
		MiniFilm(Film *film);
		~MiniFilm();

		Film* film() const;
};

#endif
