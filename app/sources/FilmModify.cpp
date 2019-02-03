
/**
 * QWidget to edit a film
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

#include "FilmModify.h"

#include "Config.h"
#include "Database.h"
#include "Dialog.h"
#include "Film.h"
#include "MainWindow.h"
#include "Network.h"
#include "SearchBar.h"
#include "Utils.h"

#include <QBuffer>
#include <QCloseEvent>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
FilmModify::FilmModify(Utils *utils, Film *film)
	: Widget(utils, tr("Modify a film"), QPixmap(":/images/modify"), true), _film(film)
{
	// Members
	_title                  = new QLineEdit(film->title());
	_originalTitle          = new QLineEdit(film->originalTitle());
	_informationsLayout     = new QHBoxLayout;
	_jacket                 = new QLabel;
	_jacketButtonsLayout    = new QHBoxLayout;
	_switchJacket           = new QPushButton(QIcon(":/images/switch"), tr("Switch"));
	_deleteJacket           = new QPushButton(QIcon(":/images/delete"), tr("Delete"));
	_informationsTextLayout = new QFormLayout;
	_countries              = new QLineEdit(film->countries());
	_genders                = new QLineEdit(film->genders());
	_duration               = new QLineEdit(QString::number(film->duration()));
	_year                   = new QLineEdit(film->year());
	_realisators            = new QLineEdit(film->realisators());
	_actors                 = new QLineEdit(film->actors());
	_synopsis               = new QTextEdit(film->synopsis());
	_informations2          = new QFormLayout;
	_comment                = new QLineEdit(film->comment());
	_note                   = new QDoubleSpinBox;
	_filenameLayout         = new QHBoxLayout;
	_filename               = new QLineEdit(film->filename());
	_browse                 = new QPushButton(tr(" Browse "));
	_trailerUrl             = new QLineEdit(film->trailerUrl());
	_buttonsLayout          = new QHBoxLayout;
	_apply                  = new QPushButton(QIcon(":/images/apply_green"), tr("Apply"));
	_reset                  = new QPushButton(QIcon(":/images/reset"), tr("Reset"));

	// Properties
	_title->setStyleSheet("font-weight: bold;color: #5555CC");
	_title->setAlignment(Qt::AlignCenter);

	// Limits
	_title->setMaxLength(Fk::TitleLimit);
	_originalTitle->setMaxLength(Fk::OriginalTitleLimit);
	_genders->setMaxLength(Fk::GendersLimit);
	_duration->setValidator(new QIntValidator(0, Fk::DurationLimit, _duration));
	_countries->setMaxLength(Fk::CountriesLimit);
	_year->setValidator(new QRegExpValidator(QRegExp("[0-9]{4}"), _year));
	_actors->setMaxLength(Fk::ActorsLimit);
	_realisators->setMaxLength(Fk::RealisatorsLimit);
	_filename->setMaxLength(Fk::FilenameLimit);
	_comment->setMaxLength(Fk::CommentLimit);
	_note->setRange(-1.0, 10.0);
	_trailerUrl->setMaxLength(Fk::TrailerUrlLimit);

	// Set note parameters
	_note->setDecimals(1);
	_note->setSingleStep(0.1);
	_note->setValue(film->note());

	// Jacket
	QPixmap jacket(film->jacket());

	if (jacket.isNull())
		this->removeJacket();

	else
	{
		QBuffer buffer(&_jacketBytes);

		if (buffer.open(QIODevice::WriteOnly) && jacket.save(&buffer, "PNG"))
			this->setJacket();
	}

	// Jacket menu
	QMenu *menu(new QMenu);
	menu->addAction(QIcon(":/images/url"), tr("By URL"), this, SLOT(switchJacketWithURL()));
	menu->addAction(QIcon(":/images/file"), tr("By file"), this, SLOT(switchJacketWithFile()));
	_switchJacket->setMenu(menu);

	this->installEventFilter(this);

	// Connects
	connect(_deleteJacket, SIGNAL(clicked()), this, SLOT(removeJacket()));
	connect(_browse, SIGNAL(clicked()), this, SLOT(browse()));
	connect(_apply, SIGNAL(clicked()), this, SLOT(saveModifiedFilm()));
	connect(_reset, SIGNAL(clicked()), this, SLOT(reset()));

	// Layouts
	_layout->addWidget(_title);
	_layout->addLayout(_informationsLayout);

	_informationsLayout->addWidget(_jacket);
	_informationsLayout->addLayout(_informationsTextLayout);

	_informationsTextLayout->addRow("<b>" + tr("Original title: ") + "</b>", _originalTitle);
	_informationsTextLayout->addRow("<b>" + tr("Countries: ")      + "</b>", _countries);
	_informationsTextLayout->addRow("<b>" + tr("Gender: ")         + "</b>", _genders);
	_informationsTextLayout->addRow("<b>" + tr("Duration: ")       + "</b>", _duration);
	_informationsTextLayout->addRow("<b>" + tr("Year: ")           + "</b>", _year);
	_informationsTextLayout->addRow(new QLabel());
	_informationsTextLayout->addRow("<b>" + tr("Realized by: ")    + "</b>", _realisators);
	_informationsTextLayout->addRow("<b>" + tr("With: ")           + "</b>", _actors);

	_layout->addLayout(_jacketButtonsLayout);

	_jacketButtonsLayout->addWidget(_switchJacket);
	_jacketButtonsLayout->addWidget(_deleteJacket);
	_jacketButtonsLayout->setAlignment(Qt::AlignLeft);

	_layout->addLayout(_informations2);

	_informations2->addRow("<b>" + tr("Synopsis: ") + "</b>", _synopsis);
	_informations2->addRow("<b>" + tr("Trailer: ")  + "</b>", _trailerUrl);
	_informations2->addRow("<b>" + tr("Comment: ")  + "</b>", _comment);
	_informations2->addRow("<b>" + tr("Note: ")     + "</b>", _note);
	_layout->addLayout(_filenameLayout);

	_filenameLayout->addWidget(new QLabel("<b>" + tr("File: ") + "</b>"));
	_filenameLayout->addWidget(_filename);
	_filenameLayout->addWidget(_browse);

	_layout->addLayout(_buttonsLayout);

	_buttonsLayout->addWidget(_apply);
	_buttonsLayout->addWidget(_reset);
	_buttonsLayout->addWidget(_close);
}

FilmModify::~FilmModify()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~FilmModify()";
	#endif
}

bool FilmModify::eventFilter(QObject *object, QEvent *event)
{
	QKeyEvent *keyEvent(dynamic_cast<QKeyEvent*>(event));

	// On Escape key press, close this window
	if (object == this && keyEvent != NULL && keyEvent->type() == QEvent::KeyPress && keyEvent->key() == Qt::Key_Escape)
	{
		this->close();
		return true;
	}

	return false;
}

/*--------------------------------------------------------------------*\
		Public slots
\*--------------------------------------------------------------------*/
void FilmModify::switchJacketWithURL()
{
	// Ask for link
	Dialog dialog(_utils, tr("Switch jacket by url"));
	dialog.addLineEdit(tr("Link: "), "http://.+\\..+");

	if (dialog.exec() == QDialog::Accepted && ! dialog.lineEditText().isEmpty())
	{
		// Get jacket from http
		if (_utils->network()->get(dialog.lineEditText(), Fk::JacketTimeout, Fk::JacketRetry))
		{
			_jacketBytes = _utils->network()->response();
			this->setJacket();
		}

		else
			QMessageBox::critical(this, tr("Error"), tr("Impossible to download the jacket (%1)").arg(_utils->network()->error()));
	}
}

void FilmModify::switchJacketWithFile()
{
	// Ask for filename
	QString fileName(QFileDialog::getOpenFileName(this, tr("Switch jacket"), _utils->config()->homeLocation()));

	if ( ! fileName.isEmpty())
	{
		// Get jacket bytes from filename
		QPixmap jacket(fileName);
		QBuffer buffer(&_jacketBytes);

		if (buffer.open(QIODevice::WriteOnly) && jacket.save(&buffer, "PNG"))
			this->setJacket();
	}
}

void FilmModify::setJacket()
{
	// Load jacket from _jacketBytes
	QPixmap jacket;
	jacket.loadFromData(_jacketBytes);
	_jacket->setPixmap(_utils->scaleToHeight(jacket, Fk::JacketRatio));
}

void FilmModify::removeJacket()
{
	// Put default jacket, and remove _jacketBytes
	_jacket->setPixmap(_utils->scaleToHeight(QPixmap(":/images/defaultJacket"), Fk::JacketRatio));
	_jacketBytes.clear();
}

void FilmModify::browse()
{
	// Ask for filemane
	QString filename(QFileDialog::getOpenFileName(this, tr("Choose filename"), _film->path()));

	if ( ! filename.isEmpty())
		_filename->setText(filename);
}

void FilmModify::saveModifiedFilm()
{
	// Note
	float note = _note->value();

	if (note < 0.0)
		note = -1.0;

	// Set all fileds
	_film->setTitle(_title->text().simplified());
	_film->setOriginalTitle(_originalTitle->text().simplified());
	_film->setCountries(_countries->text().simplified());
	_film->setGenders(_genders->text().simplified());
	_film->setDuration(_duration->text().toInt());
	_film->setYear(_year->text().simplified());
	_film->setActors(_actors->text().simplified());
	_film->setRealisators(_realisators->text().simplified());
	_film->setSynopsis(_synopsis->toPlainText().simplified());
	_film->setTrailerUrl(_trailerUrl->text().simplified());
	_film->setComment(_comment->text().simplified());
	_film->setNote(note);
	_film->setFilename(_filename->text()); // No simplified() on fileName as it can contains more than one space at once

	// Check if complete before db update, update db and reload searchBar (this will close this widget by reloading a film)
	_film->checkIfComplete(_jacketBytes.isEmpty());
	_utils->db()->updateFilm(_film, true, _jacketBytes);
	_utils->searchBar()->reload();
}

void FilmModify::reset()
{
	// Reset all fields (except filename)
	_title->setText(_film->titleFromFilename());
	_originalTitle->clear();
	this->removeJacket();
	_countries->clear();
	_genders->clear();
	_duration->clear();
	_year->clear();
	_realisators->clear();
	_actors->clear();
	_synopsis->clear();
	_comment->clear();
	_note->setValue(-1);
	_trailerUrl->clear();
}

/*--------------------------------------------------------------------*\
		Protected
\*--------------------------------------------------------------------*/
void FilmModify::closeEvent(QCloseEvent *event)
{
	// Reload current film
	_utils->window()->reloadCurrentFilm();
	event->accept();
}
