
/**
 * QMainWindow
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Config;
class Film;
class FilmView;
class Utils;

class QListWidget;
class QListWidgetItem;
class QSplitter;

class MainWindow: public QMainWindow
{
	Q_OBJECT

	private:
		Utils*       _utils;
		QSplitter*   _splitter;
		QListWidget* _listWidget;
		FilmView*    _filmView;
		Film*        _currentFilm;
		QMenu*       _collectionMenu;
		bool         _isCollectionOpened;

	public:
		MainWindow(Config *config);
		~MainWindow();

		void run();
		void updateVisible(bool enableInterface = true);
		void addFilm(const QString &fileName, const QString &dateTimeAdd, bool isFilename = true);
		void attach(Film *film);
		void clearListWidget();

	protected:
		void closeEvent(QCloseEvent *event);

	public slots:
		void reloadCurrentFilm();
		void switchFilm(QListWidgetItem *newItem, bool selectItem = false);
		void launchTrailer()                                               const;
		void launchFilm()                                                  const;
		void removeFilm(Film *film, bool removeFile);

	private slots:
		// Toolbar
		void downloadNewVersion();

		// Top menu
		void newCollection();
		void openCollection();
		void deleteCollection();
		void optimizeCollection() const;
		void addNewFilms();
		void runDirectoryScan();
		void rescan()             const;
		void updateFilms();
		void doubloons()          const;
		void renameFiles()        const;
		void statistics()         const;
		void help();
		void preferences()        const;
		void about();

		// Right-clic menu
		void displayMenu(QPoint position) const;
		void setSeen();
		void setNotSeen();
		void switchSeen();
		void setComplete();
		void setUncomplete();
		void setChecked();
		void setUnchecked();
		void switchComment();
		void switchDirPath();
		void fusion();
		void unfusion();
		void allChoices();
		void update();
		void modify();
		void remove();

	private:
		void openCollection(const QString &name);
		void switchWidget(QWidget *widget, bool enableInterface = true);
};

#endif
