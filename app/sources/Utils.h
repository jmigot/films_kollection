
/**
 * Very important class on this program, it makes an access for all classes to all created classes (Network, Database, Config, Parser, MainWindow)
 * and propose some useful fonctions
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

#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>

class Config;
class Database;
class Film;
class MainWindow;
class Network;
class Parser;
class SearchBar;

namespace Fk
{
	enum FilmsListFilters
	{
		All             = 0x001,
		Parts           = 0x002,
		ContainingParts = 0x004,
		Selected        = 0x008,
		Visible         = 0x010,
		Seen            = 0x020,
		NotSeen         = 0x040,
		Complete        = 0x080,
		Uncomplete      = 0x100,
		Checked         = 0x200,
		Unchecked       = 0x400
	};
}

class Utils
{
	private:
		MainWindow*  _window;
		Config*      _config;
		Database*    _database;
		Network*     _network;
		Parser*      _parser;
		SearchBar*   _searchBar;
		QList<Film*> _filmsList;

	public:
		Utils(MainWindow *window, Config *config);
		~Utils();

		QString      titleFromFilename(QString filename)                           const;
		float        roundNote(float note)                                         const;
		QPixmap      scaleToHeight(const QPixmap &jacket, int scaleRatio)          const;
		QPixmap      scale(const QPixmap &note, int scaleRatio)                    const;
		void         appendFilm(Film *film);
		bool         isInFilmsList(const QString &filename)                        const;
		QString      searchFilename(const QString &filename)                       const;
		void         scanDirectory(const QString &dirName, QStringList &filmsList) const;
		void         allDirs(const QString &dirName, QStringList &dirsList)        const;
		bool         removeFile(Film *film)                                        const;
		void         removeFilm(Film *film);
		void         removeAllFilms();
		QStringList  filmsPaths()                                                  const;
		Film*        film(int id)                                                  const;
		QList<Film*> filmParts(Film *film)                                         const;
		QList<Film*> filmsList(int filters)                                        const;

		// Get
		MainWindow*  window()    const;
		Config*      config()    const;
		Database*    db()        const;
		Network*     network()   const;
		Parser*      parser()    const;
		SearchBar*   searchBar() const;

		// Set
		void setConfig(Config *config);
};

#endif
