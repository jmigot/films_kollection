
/**
 * QToolBar to propose a search engine for films informations
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

#ifndef SEARCH_BAR_H
#define SEARCH_BAR_H

#include <QToolBar>

class Utils;

class QCheckBox;
class QComboBox;
class QCompleter;
class QPushButton;
class QStringListModel;

class SearchBar: public QToolBar
{
	Q_OBJECT

	private:
		Utils*            _utils;
		QComboBox*        _searchType;
		QComboBox*        _searchEdit;
		QCompleter*       _completer;
		QStringListModel* _wordListModel;
		QPushButton*      _allFilms;
		QPushButton*      _randomFilm;
		QCheckBox*        _seenFilms;
		QCheckBox*        _notSeenFilms;
		QCheckBox*        _checkedFilms;

	public:
		SearchBar(Utils *utils);
		~SearchBar();

		void reload();
		void nextFilm() const;

	private:
		bool eventFilter(QObject *object, QEvent *event);

	public slots:
		void search();

	private slots:
		void switchCompleter(int type);
		void displayAllFilms();
		void showAllFilms() const;
		void displayRandomFilm() const;
};

#endif
