
/**
 * QWidget to edit configuration
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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "Widget.h"

class Config;
class Utils;

class QCheckBox;
class QComboBox;
class QFormLayout;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class QTextEdit;

class Preferences: public Widget
{
	Q_OBJECT

	private:
		Config*      _config;
		QGroupBox*   _box;
		QFormLayout* _optionsLayout;
		QComboBox*   _language;
		QComboBox*   _defaultCollection;
		QSpinBox*    _nbActorsToDisplay;
		QSpinBox*    _minimumFileSize;
		QCheckBox*   _showFullPathName;
		QCheckBox*   _launchRandomFilm;
		QLineEdit*   _otherDirPath;
		QTextEdit*   _extensions;
		QHBoxLayout* _buttonsLayout;
		QPushButton* _save;
		QPushButton* _resetToDefaultValues;

	public:
		Preferences(Utils *utils);
		~Preferences();

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void save();
		void resetDefaultValues();
		void preview();
};

#endif
