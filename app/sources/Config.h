
/**
 * Class to manage configuration
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

#ifndef CONFIG_H
#define CONFIG_H

#include <QVariant>

/*--------------------------------------------------------------------*\
		Private config of program
\*--------------------------------------------------------------------*/
namespace Fk
{
	// User preferences index keys
	enum ConfigKey
	{
		DefaultCollection,
		NbActorsToDisplay,
		MinimumFileSize,
		ShowFullPathName,
		LaunchRandomFilm,
		Extensions,
		Language,
		OtherDirPath
	};

	// Scale ratios for jackets
	enum ScaleRatio
	{
		JacketRatio,
		MiniJacketRatio,
		LogoRatio,
		NoteRatio       = 20,
		IconRatio       = 30,
		WelcomeRatio    = 50
	};

	// Global vars (all files)
	const QString ApplicationTitle    = "FilmsKollection";
	const float   Version             = 0.97;
	const QString UrlHome             = "http://www.filmskollection.org";
	const QString UrlCheckVersion     = "http://www.filmskollection.org/downloads/version.txt";
	const QString EmailContact        = "contact@filmskollection.org";
	const QString CollectionDir       = "collections/";
	const QString CollectionExtension = ".db3";
	const QString ConfigFilename      = "Config.cfg";

	// Max len limits (Preferences.cpp)
	const int TitleLimit              = 128;
	const int OriginalTitleLimit      = 128;
	const int GendersLimit            = 256;
	const int DurationLimit           = 1500; // This is based on the longest film in the world: 24 hours so 24*60 = 1440 minutes
	const int CountriesLimit          = 256;
	const int ActorsLimit             = 600; // 20 actors maximum can be taken, this let 30 chars by actor
	const int RealisatorsLimit        = 256;
	const int FilenameLimit           = 255; // URLs can't be composed of more than 255 chars
	const int CommentLimit            = 150;
	const int TrailerUrlLimit         = 255; // URLs can't be composed of more than 255 chars

	// Network timeouts (Network.cpp)
	const int JacketTimeout           = 8;
	const int NewVersionTimeout       = 2;
	const int AllChoicesTimeout       = 5;
	const int MiniJacketTimout        = 3;
	const int FilmTimeout             = 4;

	// Network retries (Network.cpp)
	const int JacketRetry             = 1;
	const int AllChoicesRetry         = 2;
	const int MiniJacketRetry         = 1;
	const int FilmRetry               = 1;
}

class ConfigValue
{
	private:
		QString        _name;
		QVariant::Type _type;
		QVariant       _value;
		QVariant       _default;
		QVariant       _minimum;
		QVariant       _maximum;

	public:
		ConfigValue(const QString &name, QVariant::Type type, const QVariant &value);
		ConfigValue(const QString &name, QVariant::Type type, const QVariant &value, QVariant minimum, QVariant maximum);
		~ConfigValue();

		// Get
		QString        name()         const;
		QVariant::Type type()         const;
		QVariant       value()        const;
		QVariant       defaultValue() const;
		QVariant       minimum()      const;
		QVariant       maximum()      const;

		// Set
		void setValue(const QString &value);
};

class Config
{
	public:


	private:
		QString                           _dataLocation;
		QString                           _homeLocation;
		QMap<Fk::ConfigKey, ConfigValue*> _configs;

	public:
		Config();
		~Config();

		void set(Fk::ConfigKey keyName, const QString &value);
		void readConfigFile()                                 const;
		void writeConfigFile()                                const;

		// Get
		QString  dataLocation()                      const;
		QString  homeLocation()                      const;
		QVariant value(Fk::ConfigKey keyName)        const;
		QVariant defaultValue(Fk::ConfigKey keyName) const;
		QVariant min(Fk::ConfigKey keyName)          const;
		QVariant max(Fk::ConfigKey keyName)          const;
};

#endif
