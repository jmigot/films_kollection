
/**
 * QWidget to manage added/removed films from files
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

#include "ScanDirs.h"
#include "Database.h"
#include "Dialog.h"
#include "Film.h"
#include "MainWindow.h"
#include "Progress.h"
#include "SearchBar.h"
#include "Utils.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
ScanDirs::ScanDirs(Utils *utils)
	: Widget(utils, tr("Rescan dirs of collection"), QPixmap(":/images/rescan"))
{
	// Members
	_gridLayout     = new QGridLayout;
	_newFilmsList   = new QListWidget;
	_oldFilmsList   = new QListWidget;
	_addNewFilms    = new QPushButton(QIcon(":/images/add"), tr("Add"));
	_removeOldFilms = new QPushButton(QIcon(":/images/remove"), tr("Remove"));
	_progressLabel  = new QLabel(tr("Scan of dirs..."));
	_progress       = new QProgressBar;

	// Properties
	_newFilmsList->installEventFilter(this);
	_oldFilmsList->installEventFilter(this);

	// Extended selection allow multiple selection of items (with CTRL and SHIFT keys)
	_newFilmsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_oldFilmsList->setSelectionMode(QAbstractItemView::ExtendedSelection);

	// Connects
	connect(_addNewFilms, SIGNAL(clicked()), this, SLOT(addNewFilms()));
	connect(_removeOldFilms, SIGNAL(clicked()), this, SLOT(removeOldFilms()));

	// Layouts
	_layout->addLayout(_gridLayout);

	_gridLayout->addWidget(new QLabel(tr("New films detected")), 0, 0);
	_gridLayout->addWidget(new QLabel(tr("Old films detected")), 0, 1);
	_gridLayout->addWidget(_newFilmsList, 1, 0);
	_gridLayout->addWidget(_oldFilmsList, 1, 1);
	_gridLayout->addWidget(_addNewFilms, 2, 0);
	_gridLayout->addWidget(_removeOldFilms, 2, 1);

	_layout->addWidget(_progressLabel);
	_layout->addWidget(_progress);
	_layout->addWidget(_close, 0, Qt::AlignHCenter);

	this->display();
	this->scan();
}

ScanDirs::~ScanDirs()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~ScanDirs()";
	#endif
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
void ScanDirs::scan()
{
	// Search for new films
	// Get all paths
	QStringList dirs(_utils->filmsPaths());
	QString dateTime(QLocale::system().toString(QDateTime::currentDateTime()));

	_progress->setRange(0, dirs.count() + 1);
	_progress->setValue(0);

	// Foreach path detected
	foreach (QString dir, dirs)
	{
		// Get all films in path
		QStringList filmsList;
		_utils->scanDirectory(dir, filmsList);

		// Add film to list
		foreach (QString film, filmsList)
			if ( ! this->contains(film))
				_newFilmsList->addItem(new Film(_utils, film, dateTime));

		_progress->setValue(_progress->value() + 1);
	}

	/*
	   Search for old films
	   Foreach film, if filename doesn't exist, add a MiniFilm film to list (which contains a pointer to real film)
	   MiniFilm prevent film to being deleted when this list is cleared
	 */
	foreach (Film *film, _utils->filmsList(Fk::All | Fk::Parts))
		if ( ! film->filename().isEmpty() && _utils->searchFilename(film->filename()).isEmpty())
			_oldFilmsList->addItem(new MiniFilm(film));

	_progressLabel->setText(tr("Scan of dirs terminated"));
	_progress->setValue(_progress->maximum());

	this->updateVisible();
}

bool ScanDirs::contains(QString title)
{
	for (int i = 0; i < _newFilmsList->count(); i++)
		if (_newFilmsList->item(i)->text() == _utils->titleFromFilename(title))
			return true;

	return false;
}

void ScanDirs::updateVisible()
{
	// Enable/Disable lists and buttons
	_newFilmsList->setEnabled(_newFilmsList->count() > 0);
	_addNewFilms->setEnabled(_newFilmsList->count() > 0);
	_oldFilmsList->setEnabled(_oldFilmsList->count() > 0);
	_removeOldFilms->setEnabled(_oldFilmsList->count() > 0);
	_close->setEnabled(true);
}

bool ScanDirs::eventFilter(QObject *object, QEvent *event)
{
	QKeyEvent *keyEvent(dynamic_cast<QKeyEvent*>(event));

	// If Delete key is pressed
	if (keyEvent != NULL && keyEvent->type() == QEvent::KeyPress && keyEvent->key() == Qt::Key_Delete)
	{
		// For new films list
		if (object == _newFilmsList && QMessageBox::question(this, tr("Confirm"), tr("Do you really want to remove selection from list ?")) == QMessageBox::Ok)
			foreach(QListWidgetItem *item, _newFilmsList->selectedItems())
				delete item;

		// For old films list
		else if (object == _oldFilmsList && QMessageBox::question(this, tr("Confirm"), tr("Do you really want to remove selection from list ?")) == QMessageBox::Ok)
			foreach(QListWidgetItem *item, _oldFilmsList->selectedItems())
				delete item;
	}

	return false;
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
void ScanDirs::addNewFilms()
{
	// Prevent for re-clicking on button
	_addNewFilms->setEnabled(false);
	_close->setEnabled(false);

	_progressLabel->setText(tr("Adding new films..."));
	_progress->setRange(0, _newFilmsList->count());
	_progress->setValue(0);

	// Foreach film in new films list, add it to collection
	for (int i = 0; i < _newFilmsList->count(); i++)
	{
		Film *film = dynamic_cast<Film*>(_newFilmsList->item(i));

		if (film != NULL)
			_utils->window()->addFilm(film->filename(), film->dateTimeAdd());

		_progress->setValue(_progress->value() + 1);
	}

	// Clear list and update display
	_newFilmsList->clear();
	this->updateVisible();

	_utils->searchBar()->reload();
}

void ScanDirs::removeOldFilms()
{
	// Prevent for re-clicking on button
	_removeOldFilms->setEnabled(false);
	_close->setEnabled(false);

	QList<Film*> filmsList;

	// Retrieve a real filmsList with film pointers from MiniFilm list
	for (int i = 0; i < _oldFilmsList->count(); i++)
	{
		MiniFilm *miniFilm = dynamic_cast<MiniFilm*>(_oldFilmsList->item(i));

		if (miniFilm != NULL)
			filmsList.append(miniFilm->film());
	}

	// If filmList contains only 1 film
	if (filmsList.count() == 1)
	{
		// Confirm dialog
		QString text(tr("Remove %1 from collection ?").arg("<b>" + filmsList.at(0)->title() + "</b>"));
		int nbParts = _utils->filmParts(filmsList.at(0)).count();

		if (nbParts > 0)
			text += "<br /><br />" + tr("Warning: this film is composed of %n part(s) which will be deleted as well.", "", nbParts);

		Dialog dialog(_utils, tr("Remove a film"));
		dialog.addText(text + "<br />");

		// Exec confirm dialog
		if (dialog.exec())
		{
			_progressLabel->setText(tr("Removing film..."));
			_utils->window()->removeFilm(filmsList.at(0), dialog.isRemoveFileChecked());

			// Optimization of collection
			_progressLabel->setText(tr("Optimizing collection..."));
			_progress->setMaximum(0);
			_utils->db()->compact();
			_progress->setRange(0, 1);
			_progress->setValue(1);
		}
	}

	// Else if more than 1 film has to be deleted
	else if (filmsList.count() > 1)
	{
		// Confirm dialog
		QString text(tr("Remove selected films from collection ?") + "<br /><br />" +
					 tr("Warning: all composed parts will be deleted as well."));

		Dialog dialog(_utils, tr("Remove films"));
		dialog.addText(text + "<br />");

		// Exec confirm dialog
		if (dialog.exec())
		{
			_progressLabel->setText(tr("Removing films..."));
			_progress->setRange(0, filmsList.count());
			_progress->setValue(0);

			// Delete all film
			foreach (Film* film, filmsList)
			{
				_utils->window()->removeFilm(film, dialog.isRemoveFileChecked());
				_progress->setValue(_progress->value() + 1);
			}

			// Optimization of collection
			_progressLabel->setText(tr("Optimizing collection..."));
			_progress->setMaximum(0);
			_utils->db()->compact();
			_progress->setRange(0, 1);
			_progress->setValue(1);
		}
	}

	// Clear list and update display
	_oldFilmsList->clear();
	this->updateVisible();
}

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
MiniFilm::MiniFilm(Film *film)
	: QListWidgetItem(film->icon(), film->text()), _film(film)
{
}

MiniFilm::~MiniFilm()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~MiniFilm()";
	#endif
}

Film* MiniFilm::film() const
{
	return _film;
}
