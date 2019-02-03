
/**
 * Class to create custom QDialog
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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Utils;

class QCheckBox;
class QComboBox;
class QFormLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QTextEdit;
class QVBoxLayout;

class Dialog: public QDialog
{
	Q_OBJECT

	private:
		Utils*       _utils;
		QVBoxLayout* _layout;
		QFormLayout* _formLayout;
		QLineEdit*   _lineEdit;
		QTextEdit*   _textEdit;
		QComboBox*   _comboBoxEdit;
		QLabel*      _label;
		QString      _homeLocation;
		QCheckBox*   _removeFile;
		QHBoxLayout* _buttonsLayout;
		QPushButton* _ok;
		QPushButton* _cancel;

	public:
		Dialog(Utils *utils, const QString &title);
		~Dialog();

		void installNewCollectionEdit();
		void installPathEdit();

		void addLineEdit(const QString &label, const QString &validator, const QString &text = QString());
		void addText(const QString &text = NULL);
		void addTextEdit(const QString &text);
		void addRemoveFile(bool fileExists, const QString &text);
		void addComboBox(QStringList values);

		// Get
		QString lineEditText()        const;
		QString comboBoxEditText()    const;
		QString textEditText()        const;
		bool    isRemoveFileChecked() const;

	public slots:
		void checkWindowsForbiddenChars(const QString &text);
		void browsePath();
};

#endif
