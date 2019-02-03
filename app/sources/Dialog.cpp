
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

#include "Dialog.h"
#include "Utils.h"
#include "Config.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExpValidator>
#include <QTextEdit>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Dialog::Dialog(Utils *utils, const QString &title)
	: _utils(utils)
{
	// Members
	_layout        = new QVBoxLayout(this);
	_formLayout    = new QFormLayout;
	_lineEdit      = new QLineEdit;
	_textEdit      = new QTextEdit;
	_comboBoxEdit  = new QComboBox;
	_label         = new QLabel;
	_removeFile    = new QCheckBox;
	_buttonsLayout = new QHBoxLayout;
	_ok            = new QPushButton(QIcon(":/images/apply_green"), tr("Ok"));
	_cancel        = new QPushButton(QIcon(":/images/cross"), tr("Cancel"));

	// Properties
	this->setWindowTitle(title);
	this->setWindowIcon(QIcon(":/images/icon"));
	this->setWindowModality(Qt::ApplicationModal);

	_comboBoxEdit->setMaxVisibleItems(50);
	_comboBoxEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	_buttonsLayout->setAlignment(Qt::AlignRight);

	// Connects
	connect(_ok, SIGNAL(clicked()), this, SLOT(accept()));
	connect(_cancel, SIGNAL(clicked()), this, SLOT(reject()));

	// Layouts
	_layout->addLayout(_formLayout);
	_layout->addLayout(_buttonsLayout);

	_buttonsLayout->addWidget(_ok);
	_buttonsLayout->addWidget(_cancel);
}

Dialog::~Dialog()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Dialog()";
	#endif
}

void Dialog::installNewCollectionEdit()
{
	/*
	   Limit of caracters on a complete filepathname: 255
	   Calcul how many caracters are left to complete filename (255 - complete collection dir count - collection extension count)
	   Forbidden char for all systems: \
	*/
	int nbLeftCaracters = 255 - QString(QApplication::applicationDirPath() + "/" + Fk::CollectionDir + Fk::CollectionExtension).count();

	connect(_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkWindowsForbiddenChars(QString)));

	this->addLineEdit(tr("Name: "), "[^/\\\\]{1," + QString::number(nbLeftCaracters)+ "}");
	this->addText();
}

void Dialog::installPathEdit()
{
	// Special widget to install QGridLayout into QFormLayout
	QWidget     *widget(new QWidget);
	QGridLayout *layout(new QGridLayout(widget));
	QPushButton *browse(new QPushButton(tr("Browse")));

	// Properties
	_label->setText(tr("Path: "));

	_comboBoxEdit->setEditable(true);
	_comboBoxEdit->setValidator(new QRegExpValidator(QRegExp(".{0,255}"), _comboBoxEdit));
	_comboBoxEdit->setMinimumWidth(250);
	_comboBoxEdit->addItem(QString());
	_comboBoxEdit->addItems(_utils->filmsPaths());

	// Layouts
	_formLayout->addWidget(widget);

	layout->addWidget(_label, 0, 0);
	layout->addWidget(_comboBoxEdit, 0, 1);
	layout->addWidget(browse, 0, 2);

	_comboBoxEdit->setFocus();
}

void Dialog::addLineEdit(const QString &label, const QString &validator, const QString &text)
{
	_lineEdit->setValidator(new QRegExpValidator(QRegExp(validator), _lineEdit));
	_lineEdit->setText(text);
	_lineEdit->setMinimumWidth(250);
	_formLayout->addRow(label, _lineEdit);
	_lineEdit->setFocus();
}

void Dialog::addTextEdit(const QString &text)
{
	_label->setText(text);
	_formLayout->addWidget(_label);
	_formLayout->addWidget(_textEdit);
	_textEdit->setFocus();
}

void Dialog::addText(const QString &text)
{
	if ( ! text.isEmpty())
		_label->setText(text);

	_formLayout->addWidget(_label);
}

void Dialog::addRemoveFile(bool fileExists, const QString &text)
{
	_removeFile->setEnabled(fileExists);
	_removeFile->setText(text);
	_formLayout->addWidget(_removeFile);
}

void Dialog::addComboBox(QStringList values)
{
	foreach (QString value, values)
		_comboBoxEdit->addItem(value);

	_formLayout->addWidget(_comboBoxEdit);
}

// Get
QString Dialog::lineEditText() const
{
	return _lineEdit->text();
}

QString Dialog::comboBoxEditText() const
{
	return _comboBoxEdit->currentText();
}

QString Dialog::textEditText() const
{
	return _textEdit->toPlainText();
}

bool Dialog::isRemoveFileChecked() const
{
	return _removeFile->isChecked();
}

/*--------------------------------------------------------------------*\
		Public slots
\*--------------------------------------------------------------------*/
void Dialog::checkWindowsForbiddenChars(const QString &text)
{
	// Windows forbidden filenames characters: /\"*?<>|:
	// Unix forbidden caracters: /
	if (text.contains(QRegExp("[\"*?<>|:]")))
		_label->setText(tr("Warning: invalid filename under Windows systems"));

	else
		_label->clear();
}

void Dialog::browsePath()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("New path"), _homeLocation);

	if ( ! path.isEmpty())
		_comboBoxEdit->setEditText(path);
}
