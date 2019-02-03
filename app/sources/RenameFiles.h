
/**
 * QWidget to rename all film files using title() or originalTitle()
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

#ifndef RENAME_FILES_H
#define RENAME_FILES_H

#include "Widget.h"

class Utils;

class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QLineEdit;
class QListWidget;
class QProgressBar;
class QRadioButton;

class RenameFiles: public Widget
{
	Q_OBJECT

	private:
		QGridLayout*  _gridLayout1;
		QButtonGroup* _groupFilms;
		QRadioButton* _allFilms;
		QRadioButton* _selectedFilms;
		QButtonGroup* _groupTitle;
		QRadioButton* _title;
		QRadioButton* _originalTitle;
		QLineEdit*    _prefix;
		QLineEdit*    _suffix;
		QCheckBox*    _takeTitleIfOriginalEmpty;
		QCheckBox*    _handleWindowsForbiddenChars;
		QProgressBar* _progressBar;
		QGridLayout*  _gridLayout2;
		QListWidget*  _oldTitles;
		QListWidget*  _newTitles;
		QHBoxLayout*  _buttonsLayout;
		QPushButton*  _rename;

	public:
		RenameFiles(Utils *utils);
		~RenameFiles();

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void changeRow(int row);
		void updateVisible();
		void run();
};

#endif
