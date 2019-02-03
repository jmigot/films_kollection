
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

#include "Config.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
ConfigValue::ConfigValue(const QString &name, QVariant::Type type, const QVariant &value)
	: _name(name), _type(type), _value(value), _default(value)
{
}

ConfigValue::ConfigValue(const QString &name, QVariant::Type type, const QVariant &value, QVariant minimum, QVariant maximum)
	: _name(name), _type(type), _value(value), _default(value), _minimum(minimum), _maximum(maximum)
{
}

ConfigValue::~ConfigValue()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~ConfigValue()";
	#endif
}

// Get
QString ConfigValue::name() const
{
	return _name;
}

QVariant::Type ConfigValue::type() const
{
	return _type;
}

QVariant ConfigValue::value() const
{
	return _value;
}

QVariant ConfigValue::defaultValue() const
{
	return _default;
}

QVariant ConfigValue::minimum() const
{
	return _minimum;
}

QVariant ConfigValue::maximum() const
{
	return _maximum;
}

// Set
void ConfigValue::setValue(const QString &value)
{
	// If type is Int
	if (_type == QVariant::Int)
	{
		int v = value.toInt();

		// Check if value is between minimum and maximum
		if (v >= _minimum.toInt() && v <= _maximum.toInt())
			_value = v;
	}

	// If type is Bool
	else if (_type == QVariant::Bool)
		_value = (value == "true");

	// If type is StringList
	else if (_type == QVariant::StringList)
	{
		QStringList list;

		// Split values with ","
		foreach (QString v, value.split(","))
			list.append(v.simplified());

		_value = list;
	}

	else
		_value = value;
}

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Config::Config()
{
	QDir dir;

	// Create collection dir if doesn't exist
	if ( ! dir.exists(QApplication::applicationDirPath() + "/" + Fk::CollectionDir))
		dir.mkdir(QApplication::applicationDirPath() + "/" + Fk::CollectionDir);

	// Private config (these vars are for QFileDialog asks)
	_dataLocation = QApplication::applicationDirPath() + "/";
	_homeLocation = QApplication::applicationDirPath() + "/";

	// Home location
	if (QDir::home().exists())
		_homeLocation = QDir::home().absolutePath() + "/";

	// Extensions
	QStringList extensions;
	extensions << "*.avi" << "*.divx" << "*.mkv" << "*.xvid" << "*.wmv" << "*.mpg" << "*.mpeg" << "*.mov" << "*.movie" << "*.mp4";

	// Create configs
	_configs.insert(Fk::DefaultCollection, new ConfigValue("Default collection", QVariant::String, "Collection"));
	_configs.insert(Fk::NbActorsToDisplay, new ConfigValue("Number of actors to display", QVariant::Int, 10, 1, 20));
	_configs.insert(Fk::MinimumFileSize,   new ConfigValue("Minimum file size", QVariant::Int, 300, 50, 50000));
	_configs.insert(Fk::ShowFullPathName,  new ConfigValue("Show full pathname", QVariant::Bool, false));
	_configs.insert(Fk::LaunchRandomFilm,  new ConfigValue("Launch random film", QVariant::Bool, true));
	_configs.insert(Fk::Extensions,        new ConfigValue("Extensions", QVariant::StringList, extensions));
	_configs.insert(Fk::OtherDirPath,      new ConfigValue("Other dir path", QVariant::String, ""));
	_configs.insert(Fk::Language,          new ConfigValue("Language", QVariant::String, QLocale::system().name().section('_', 0, 0)));

	// Read config file, this will replace configs by new values
	this->readConfigFile();
}

Config::~Config()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Config()";
	#endif

	// Delete configs created
	foreach (ConfigValue *config, _configs)
		delete config;
}

void Config::set(Fk::ConfigKey keyName, const QString &value)
{
	_configs.value(keyName)->setValue(value);
}

void Config::readConfigFile() const
{
	// Open file in read-only mode
	QFile file(_dataLocation + Fk::ConfigFilename);

	if (file.exists() && file.open(QFile::ReadOnly | QFile::Text) && file.isReadable())
	{
		QString buffer(file.readAll());
		file.close();

		QRegExp regex;
		regex.setMinimal(true);

		// Match all configs strings and replace them in current config
		foreach (ConfigValue *config, _configs)
		{
			regex.setPattern(QRegExp::escape(config->name() + "=") + "(.+)\n");

			if (buffer.contains(regex))
				config->setValue(regex.cap(1).trimmed());
		}
	}
}

void Config::writeConfigFile() const
{
	// Add each config value to buffer
	QByteArray buffer;

	foreach (ConfigValue *config, _configs)
	{
		QByteArray value;

		// If type is StringList, make an apropriate string
		if (config->type() == QVariant::StringList)
		{
			foreach (QString s, config->value().toStringList())
				value += s + ", ";

			value.chop(2);
		}

		else
			value = config->value().toByteArray();

		buffer += config->name() + "=" + value + "\n";
	}

	// Open file in write mode
	QFile file(_dataLocation + Fk::ConfigFilename);

	if (file.open(QFile::WriteOnly | QFile::Text) && file.isWritable())
	{
		// Write file and close it
		file.write(buffer);
		file.close();
	}
}

// Get
QString Config::dataLocation() const
{
	return _dataLocation;
}

QString Config::homeLocation() const
{
	return _homeLocation;
}

QVariant Config::value(Fk::ConfigKey keyName) const
{
	return _configs.value(keyName)->value();
}

QVariant Config::defaultValue(Fk::ConfigKey keyName) const
{
	return _configs.value(keyName)->defaultValue();
}

QVariant Config::min(Fk::ConfigKey keyName) const
{
	return _configs.value(keyName)->minimum();
}

QVariant Config::max(Fk::ConfigKey keyName) const
{
	return _configs.value(keyName)->maximum();
}
