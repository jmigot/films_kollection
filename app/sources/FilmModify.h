
/**
 * QWidget to edit a film
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


#ifndef FILM_MODIFY_H
#define FILM_MODIFY_H

#include "Widget.h"

class Film;
class Utils;

class QDoubleSpinBox;
class QFormLayout;
class QLineEdit;
class QTextEdit;

class FilmModify: public Widget
{
	Q_OBJECT

	private:
		Film*           _film;
		QLineEdit*      _title;
		QLineEdit*      _originalTitle;
		QHBoxLayout*    _informationsLayout;
		QLabel*         _jacket;
		QHBoxLayout*    _jacketButtonsLayout;
		QByteArray      _jacketBytes;
		QPushButton*    _switchJacket;
		QPushButton*    _deleteJacket;
		QFormLayout*    _informationsTextLayout;
		QLineEdit*      _countries;
		QLineEdit*      _genders;
		QLineEdit*	    _duration;
		QLineEdit*      _year;
		QLineEdit*      _realisators;
		QLineEdit*      _actors;
		QTextEdit*      _synopsis;
		QFormLayout*    _informations2;
		QLineEdit*      _comment;
		QDoubleSpinBox* _note;
		QHBoxLayout*    _filenameLayout;
		QLineEdit*	    _filename;
		QPushButton*    _browse;
		QLineEdit*      _trailerUrl;
		QHBoxLayout*    _buttonsLayout;
		QPushButton*    _apply;
		QPushButton*    _reset;

	public:
		FilmModify(Utils *utils, Film *film);
		~FilmModify();

	private:
		bool eventFilter(QObject *object, QEvent *event);

	public slots:
		void switchJacketWithURL();
		void switchJacketWithFile();
		void setJacket();
		void removeJacket();
		void browse();
		void saveModifiedFilm();
		void reset();

	protected:
		void closeEvent(QCloseEvent *event);

};

#endif
