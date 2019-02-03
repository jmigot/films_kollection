
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

#include "RenameFiles.h"
#include "Database.h"
#include "Film.h"
#include "MainWindow.h"
#include "SearchBar.h"
#include "Utils.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
RenameFiles::RenameFiles(Utils *utils)
	: Widget(utils, tr("Rename files"), QPixmap(":/images/rename"))
{
	// Members
	_gridLayout1                 = new QGridLayout;
	_groupFilms                  = new QButtonGroup;
	_allFilms                    = new QRadioButton(tr("All films"));
	_selectedFilms               = new QRadioButton(tr("Selected films"));
	_groupTitle                  = new QButtonGroup;
	_title                       = new QRadioButton(tr("Title"));
	_originalTitle               = new QRadioButton(tr("Original title"));
	_prefix                      = new QLineEdit;
	_suffix                      = new QLineEdit;
	_takeTitleIfOriginalEmpty    = new QCheckBox(tr("Take title if original title is empty"));
	_handleWindowsForbiddenChars = new QCheckBox(tr("Handle Windows forbidden characters"));
	_progressBar                 = new QProgressBar;
	_gridLayout2                 = new QGridLayout;
	_oldTitles                   = new QListWidget;
	_newTitles                   = new QListWidget;
	_buttonsLayout               = new QHBoxLayout;
	_rename                      = new QPushButton(QIcon(":/images/apply_green"), tr("Rename"));

	// Properties
	_groupFilms->addButton(_allFilms);
	_groupFilms->addButton(_selectedFilms);
	_allFilms->setChecked(true);

	_groupTitle->addButton(_title);
	_groupTitle->addButton(_originalTitle);
	_title->setChecked(true);

	_prefix->setValidator(new QRegExpValidator(QRegExp("[^/]*"), _prefix));
	_suffix->setValidator(new QRegExpValidator(QRegExp("[^/]*"), _suffix));

	_takeTitleIfOriginalEmpty->setChecked(true);
	_handleWindowsForbiddenChars->setChecked(true);

	_progressBar->setRange(0, 1);
	_progressBar->setValue(0);

	// Connects
	connect(_originalTitle, SIGNAL(toggled(bool)), this, SLOT(updateVisible()));
	connect(_oldTitles, SIGNAL(currentRowChanged(int)), this, SLOT(changeRow(int)));
	connect(_newTitles, SIGNAL(currentRowChanged(int)), this, SLOT(changeRow(int)));
	connect(_rename, SIGNAL(clicked()), this, SLOT(run()));

	// Layouts
	_layout->addLayout(_gridLayout1);

	_gridLayout1->addWidget(new QLabel(tr("What list should be used ?")), 0, 0);
	_gridLayout1->addWidget(new QLabel(tr("What field should be used ?")), 0, 1);
	_gridLayout1->addWidget(_allFilms, 1, 0);
	_gridLayout1->addWidget(_selectedFilms, 2, 0);
	_gridLayout1->addWidget(_title, 1, 1);
	_gridLayout1->addWidget(_originalTitle, 2, 1);
	_gridLayout1->addWidget(new QLabel("<br />"), 3, 0);
	_gridLayout1->addWidget(new QLabel(tr("Prefix: ")), 4, 0);
	_gridLayout1->addWidget(_prefix, 5, 0);
	_gridLayout1->addWidget(new QLabel(tr("Suffix: ")), 4, 1);
	_gridLayout1->addWidget(_suffix, 5, 1);
	_gridLayout1->addWidget(_takeTitleIfOriginalEmpty, 6, 0);
	_gridLayout1->addWidget(_handleWindowsForbiddenChars, 7, 0);

	_layout->addWidget(_progressBar);
	_layout->addLayout(_gridLayout2);

	_gridLayout2->addWidget(new QLabel(tr("Old names")), 0, 0);
	_gridLayout2->addWidget(new QLabel(tr("New names")), 0, 1);
	_gridLayout2->addWidget(_oldTitles, 1, 0);
	_gridLayout2->addWidget(_newTitles, 1, 1);

	_layout->addLayout(_buttonsLayout);

	_buttonsLayout->addWidget(_rename);
	_buttonsLayout->addWidget(_close);

	this->updateVisible();
	this->display();
}

RenameFiles::~RenameFiles()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~RenameFiles()";
	#endif
}

/*--------------------------------------------------------------------*\
		Protected
\*--------------------------------------------------------------------*/
void RenameFiles::closeEvent(QCloseEvent *event)
{
	_utils->searchBar()->reload();
	event->accept();
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
void RenameFiles::changeRow(int row)
{
	_newTitles->setCurrentRow(row);
	_oldTitles->setCurrentRow(row);
}

void RenameFiles::updateVisible()
{
	_takeTitleIfOriginalEmpty->setEnabled(_originalTitle->isChecked());
}

void RenameFiles::run()
{
	_oldTitles->clear();
	_newTitles->clear();

	// Get complete films, and all films or selected films depending on user's choice
	int flags = Fk::Complete;

	if (_selectedFilms->isChecked())
		flags |= Fk::Selected;

	QList<Film*> filmsList(_utils->filmsList(flags));

	// If there is at least one film
	if (filmsList.count() > 0)
	{
		QFile file;

		_progressBar->setRange(0, filmsList.count());
		_progressBar->setValue(0);

		foreach (Film *film, filmsList)
		{
			// Set filename to QFile
			file.setFileName(film->filename());

			// If file exists (i.e if filename isn't empty)
			if (file.exists())
			{
				// Save old title, create new title
				QString oldTitle(film->titleFromFilename());
				QString newTitle((_title->isChecked() ? film->title() : film->originalTitle()));

				// If user decided to take title instead of original title if this one is empty
				if (_originalTitle->isChecked() && newTitle.isEmpty() && _takeTitleIfOriginalEmpty->isChecked())
					newTitle = film->title();

				// Prefix and suffix
				if ( ! _prefix->text().isEmpty())
					newTitle.prepend(_prefix->text());

				if ( ! _suffix->text().isEmpty())
					newTitle.append(_suffix->text());

				/*
				   Windows forbidden filenames characters: /\"*?<>|:
				   Unix forbidden characters: / (included in Windows replaces)

				   Replace :|/ with :
				   Replace <> with space
				   Remove "*?\
				*/

				// If user decided to handle Windows forbidden characters
				if (_handleWindowsForbiddenChars->isChecked())
					newTitle = newTitle.replace(QRegExp("[:\\|]"), "-").replace(QRegExp("[<>]"), " ").remove(QRegExp("[\"*?\\\\]"));

				// Forbidden characters (replace / with -)
				newTitle = newTitle.replace("/", "-");

				QListWidgetItem *item(new QListWidgetItem);
				QFont font;

				// If newTitle is empty (if originalTitle is empty in fact) we don't do anything
				if (newTitle.isEmpty())
				{
					item->setText(tr("New title is empty"));
					font.setBold(true);
				}

				// If the file already has the good name
				else if (newTitle == oldTitle)
				{
					item->setText(tr("Already good"));
					font.setItalic(true);
				}

				// If the rename of file failed
				else if ( ! file.rename(film->path() + newTitle + film->extension()))
				{
					item->setText(tr("Error (%1)").arg(file.errorString()));
					font.setBold(true);
				}

				// Else
				else
				{
					film->setFilename(film->path() + newTitle + film->extension());
					_utils->db()->updateFilm(film, false);
					item->setText(newTitle);
				}

				item->setFont(font);

				_oldTitles->addItem(oldTitle);
				_newTitles->addItem(item);
			}

			_progressBar->setValue(_progressBar->value() + 1);
		}
	}
}
