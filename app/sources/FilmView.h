
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

#ifndef FILM_VIEW_H
#define FILM_VIEW_H

#include <QWidget>
#include <QAction>

class Film;
class Utils;

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QTextEdit;
class QVBoxLayout;

namespace Fk
{
	enum LaunchType
	{
		LaunchTrailer,
		LaunchFilm
	};
}

class FilmView: public QWidget
{
	Q_OBJECT

	private:
		Utils*         _utils;
		QVBoxLayout*   _layout;
		QLabel*        _title;
		QGridLayout*   _informationsLayout;
		QLabel*        _jacketLabel;
		QTextEdit*      _informations;
		QHBoxLayout*   _noteLayout;
		QList<QLabel*> _notes;
		QLabel*        _note;
		QTextEdit*     _synopsis;
		QLabel*        _informations2;
		QHBoxLayout*   _buttonsLayout;
		QPushButton*   _switchSeen;
		QPushButton*   _launchTrailer;
		QPushButton*   _launchFilm;

	public:
		FilmView(Utils *utils);
		~FilmView();

		void setFilm(Film *film);

	private:
		bool eventFilter(QObject *object, QEvent *event);
};

class ActionFilm: public QAction
{
	private:
		Film* _film;

	public:
		ActionFilm(Utils *utils, QMenu *menu, const QIcon &icon, Film *film, int type);
		~ActionFilm();

		Film* film() const;
};

#endif
