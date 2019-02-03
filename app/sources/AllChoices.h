
/**
 * QWidget to show all internet results for a film
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

#ifndef ALL_CHOICES_H
#define ALL_CHOICES_H

#include "Widget.h"

class Utils;
class Film;
class Choice;

class QLineEdit;
class QProgressBar;
class QScrollArea;

class AllChoices: public Widget
{
	Q_OBJECT

	private:
		Film*          _film;
		QScrollArea*   _scrollArea;
		QLabel*        _text;
		QProgressBar*  _progressBar;
		QHBoxLayout*   _searchLayout;
		QLineEdit*     _searchEdit;
		QPushButton*   _search;
		QList<Choice*> _choices;

	public:
		AllChoices(Utils *utils, Film *film);
		~AllChoices();

	private slots:
		void search();
};

#endif
