
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

#include "Preferences.h"
#include "Config.h"
#include "MainWindow.h"
#include "Utils.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Preferences::Preferences(Utils *utils)
	: Widget(utils, tr("Preferences"), QPixmap(":/images/preferences"))
{
	// Members
	_config               = _utils->config();
	_box                  = new QGroupBox;
	_optionsLayout        = new QFormLayout(_box);
	_language             = new QComboBox;
	_defaultCollection    = new QComboBox;
	_nbActorsToDisplay    = new QSpinBox;
	_minimumFileSize      = new QSpinBox;
	_showFullPathName     = new QCheckBox(tr("Show full path of files"));
	_launchRandomFilm     = new QCheckBox(tr("Launch directly random film"));
	_otherDirPath         = new QLineEdit;
	_extensions           = new QTextEdit;
	_buttonsLayout        = new QHBoxLayout;
	_save                 = new QPushButton(QIcon(":/images/apply_green"), tr("Save"));
	_resetToDefaultValues = new QPushButton(QIcon(":/images/reset"), tr("Reset to defaults"));

	// Properties
	// Default collections
	QDir dir(_config->dataLocation() + Fk::CollectionDir);

	if (dir.exists())
	{
		dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

		// Foreach file, if it has a collection extension, add it to collections names
		foreach (QFileInfo itemInfo, dir.entryInfoList())
			if (itemInfo.fileName().endsWith(Fk::CollectionExtension, Qt::CaseInsensitive))
				_defaultCollection->addItem(itemInfo.fileName().left(itemInfo.fileName().count() - Fk::CollectionExtension.count()));

		// Set default collection according to config
		_defaultCollection->setCurrentIndex(_defaultCollection->findText(_config->value(Fk::DefaultCollection).toString()));
	}

	// Nb actors to display
	_nbActorsToDisplay->setRange(_config->min(Fk::NbActorsToDisplay).toInt(), _config->max(Fk::NbActorsToDisplay).toInt());
	_nbActorsToDisplay->setValue(_config->value(Fk::NbActorsToDisplay).toInt());

	// Minimum file size
	_minimumFileSize->setRange(_config->min(Fk::MinimumFileSize).toInt(), _config->max(Fk::MinimumFileSize).toInt());
	_minimumFileSize->setValue(_config->value(Fk::MinimumFileSize).toInt());

	// Show full path name
	_showFullPathName->setChecked(_config->value(Fk::ShowFullPathName).toBool());

	// Launch random film
	_launchRandomFilm->setChecked(_config->value(Fk::LaunchRandomFilm).toBool());

	// Extensions
	QString extensions;

	// Foreach extension, add it in a new line on QTextEdit
	foreach (QString ext, _config->value(Fk::Extensions).toStringList())
		extensions += ext.mid(2) + "\n";

	_extensions->setText(extensions);

	// Others dir paths
	_otherDirPath->setText(_config->value(Fk::OtherDirPath).toString());

	// Language
	_language->addItem(tr("English"));
	_language->addItem(tr("French"));

	if (_config->value(Fk::Language).toString() == "en")
		_language->setCurrentIndex(0);

	else if (_config->value(Fk::Language).toString() == "fr")
		_language->setCurrentIndex(1);

	// Connects
	connect(_save, SIGNAL(clicked()), this, SLOT(save()));
	connect(_resetToDefaultValues, SIGNAL(clicked()), this, SLOT(resetDefaultValues()));
	connect(_nbActorsToDisplay, SIGNAL(valueChanged(int)), this, SLOT(preview()));
	connect(_showFullPathName, SIGNAL(toggled(bool)), this, SLOT(preview()));
	connect(_otherDirPath, SIGNAL(editingFinished()), this, SLOT(preview()));

	// Layouts
	_layout->addWidget(_box);

	_optionsLayout->addRow(tr("Language: "), _language);
	_optionsLayout->addRow(tr("Collection by default: "), _defaultCollection);
	_optionsLayout->addRow(tr("Number of actors to display: "), _nbActorsToDisplay);
	_optionsLayout->addRow(tr("Minimum file size for scan (Mo): "), _minimumFileSize);

	_layout->addWidget(_showFullPathName);
	_layout->addWidget(_launchRandomFilm);

	_layout->addWidget(new QLabel(tr("Alternate dir path where to search films")));
	_layout->addWidget(_otherDirPath);

	_layout->addWidget(new QLabel(tr("Extensions to take when scanning dirs: ")));
	_layout->addWidget(_extensions);

	_layout->addLayout(_buttonsLayout);

	_buttonsLayout->addWidget(_save);
	_buttonsLayout->addWidget(_resetToDefaultValues);
	_buttonsLayout->addWidget(_close);

	this->display();
}

Preferences::~Preferences()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Preferences()";
	#endif
}

/*--------------------------------------------------------------------*\
		Protected
\*--------------------------------------------------------------------*/
void Preferences::closeEvent(QCloseEvent *event)
{
	_utils->window()->reloadCurrentFilm();
	event->accept();
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
void Preferences::save()
{
	// Set all config
	_config->set(Fk::DefaultCollection, _defaultCollection->currentText());
	_config->set(Fk::NbActorsToDisplay, QString::number(_nbActorsToDisplay->value()));
	_config->set(Fk::MinimumFileSize, QString::number(_minimumFileSize->value()));
	_config->set(Fk::ShowFullPathName, (_showFullPathName->isChecked() ? "true" : "false"));
	_config->set(Fk::LaunchRandomFilm, (_launchRandomFilm->isChecked() ? "true" : "false"));

	// Parse extensions and create the good format (*.ext1, *.ext2, ...)
	QString extensions;

	foreach (QString ext, _extensions->toPlainText().split("\n"))
		if (ext.simplified().contains(QRegExp("^[a-zA-Z0-9_-]+$")))
			extensions += "*." + ext.simplified() + ", ";

	extensions.chop(2);
	_config->set(Fk::Extensions, extensions);

	// Parse other dir path
	_config->set(Fk::OtherDirPath, _otherDirPath->text());

	// Language
	QString lang("en");

	if (_language->currentText() == tr("English"))
		lang = "en";

	else if (_language->currentText() == tr("French"))
		lang = "fr";

	// If language has changed, ask to restart program
	if (lang != _config->value(Fk::Language).toString())
		QMessageBox::information(this, tr("Restart program"), tr("You have to restart the program to reload the new language."));

	_config->set(Fk::Language, lang);

	// Write config file and close widget
	_config->writeConfigFile();
	this->close();
}

void Preferences::resetDefaultValues()
{
	// Set default values according to config
	_defaultCollection->setCurrentIndex(_defaultCollection->findText(_config->defaultValue(Fk::DefaultCollection).toString()));
	_nbActorsToDisplay->setValue(_config->defaultValue(Fk::NbActorsToDisplay).toInt());
	_minimumFileSize->setValue(_config->defaultValue(Fk::MinimumFileSize).toInt());
	_showFullPathName->setChecked(_config->defaultValue(Fk::ShowFullPathName).toBool());
	_launchRandomFilm->setChecked(_config->defaultValue(Fk::LaunchRandomFilm).toBool());

	// Extensions: one per line
	QString extensions;

	foreach (QString ext, _config->defaultValue(Fk::Extensions).toStringList())
		extensions += ext.mid(2) + "\n";

	_extensions->setText(extensions);

	// Others dir paths
	_otherDirPath->clear();

	// Language
	QString lang = _config->defaultValue(Fk::Language).toString();

	if (lang == "en")
		_language->setCurrentIndex(_language->findText(tr("English")));

	else if(lang == "fr")
		_language->setCurrentIndex(_language->findText(tr("French")));
}

void Preferences::preview()
{
	// Create a new Config
	Config *config = new Config;

	// Set instant preview properties
	config->set(Fk::NbActorsToDisplay, QString::number(_nbActorsToDisplay->value()));
	config->set(Fk::ShowFullPathName, (_showFullPathName->isChecked() ? "true" : "false"));
	config->set(Fk::OtherDirPath, _otherDirPath->text());

	// Set new config, reload film with this config, then restore original config
	_utils->setConfig(config);
	_utils->window()->reloadCurrentFilm();
	_utils->setConfig(_config);

	delete config;
}
