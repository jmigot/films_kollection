
/**
 * QWidget to show all internet results for a film
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

#include "AllChoices.h"
#include "Choice.h"
#include "Config.h"
#include "Film.h"
#include "MainWindow.h"
#include "Network.h"
#include "Parser.h"
#include "Utils.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
AllChoices::AllChoices(Utils *utils, Film *film)
	: Widget(utils, tr("All choices"), QPixmap(":/images/allChoices")), _film(film)
{
	// Members
	_text         = new QLabel;
	_progressBar  = new QProgressBar;
	_searchLayout = new QHBoxLayout;
	_searchEdit   = new QLineEdit;
	_search       = new QPushButton(tr("Search"));
	_scrollArea   = new QScrollArea;

	// Properties
	_text->setWordWrap(true);
	_searchEdit->setText(_film->titleFromFilename());

	// To force scroll area to fit the entire widget size
	_scrollArea->setWidgetResizable(true);

	// Connects
	connect(_search, SIGNAL(clicked()), this, SLOT(search()));
	connect(_searchEdit, SIGNAL(returnPressed()), this, SLOT(search()));

	// Layouts
	_layout->addWidget(_text);
	_layout->addWidget(_progressBar);
	_layout->addLayout(_searchLayout);

	_searchLayout->addWidget(_searchEdit);
	_searchLayout->addWidget(_search);

	_layout->addWidget(_scrollArea);
	_layout->addWidget(_close, 0, Qt::AlignHCenter);

	this->display(false, _utils->window()->width() / 2, _utils->window()->height() * 3 / 4);
	this->search();
}

AllChoices::~AllChoices()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~AllChoices()";
	#endif

	// Delete all choices created
	foreach (Choice *choice, _choices)
		delete choice;
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
void AllChoices::search()
{
	// Set progress bar to progressing mode and disable buttons
	_close->setDisabled(true);
	_searchEdit->setEnabled(false);
	_search->setEnabled(false);
	_text->setText("<h3>" + tr("Searching...") + "</h3>");
	_progressBar->setRange(0, 0);

	// Delete old choices from list, and clear list (this is important)
	foreach (Choice *choice, _choices)
		delete choice;

	_choices.clear();

	// Get all choices corresponding to search text
	if ( ! _utils->parser()->allChoices(_searchEdit->text(), _film, _choices))
		_text->setText("<h3>" + tr("Impossible to get list of films") + " (" + _utils->network()->error() + ")</h3>");

	else
		_text->setText("<h3>" + tr("%n film(s) found", "", _choices.count()) + "</h3>");

	// New layout
	QWidget *widget(new QWidget);
	QGridLayout *gridLayout(new QGridLayout(widget));

	// If some choices have been found, add them to widget
	for (int i = 0; i < _choices.count(); i++)
	{
		// Jacket
		QLabel *jacketLabel(new QLabel);
		jacketLabel->setPixmap(_utils->scaleToHeight(_choices.at(i)->jacket(), Fk::MiniJacketRatio));

		// Original title
		QString originalTitle;

		if ( ! _choices.at(i)->originalTitle().isEmpty())
			originalTitle = " (<b>" + _choices.at(i)->originalTitle() + "</b>)";

		// Add infos
		QLabel *infos(new QLabel("<b>" + _choices.at(i)->title() + "</b>" + originalTitle + "<br />" +
										 _choices.at(i)->year() + "<br />" +
										 _choices.at(i)->realisators() + "<br />" +
										 _choices.at(i)->actors()));
		infos->setWordWrap(true);

		// Add choose button
		QPushButton *button(new QPushButton(QIcon(":/images/refresh"), tr("Choose")));
		connect(button, SIGNAL(clicked()), this, SLOT(close()));
		connect(button, SIGNAL(clicked()), _choices.at(i), SLOT(choose()));

		// Layouts
		gridLayout->addWidget(jacketLabel, 2 * i + 2, 0, Qt::AlignTop | Qt::AlignLeft);
		gridLayout->addWidget(infos, 2 * i + 2, 1, Qt::AlignTop | Qt::AlignLeft);
		gridLayout->addWidget(button, 2 * i + 2, 2, Qt::AlignTop | Qt::AlignRight);
		gridLayout->addWidget(new QLabel("<hr />"), 2 * i + 3, 0, 1, 3, Qt::AlignTop);
	}

	// Add new widget (old widget is deleted with this call)
	_scrollArea->setWidget(widget);

	// Set progress bar to 100% and enable buttons
	_progressBar->setRange(0, 1);
	_progressBar->setValue(1);
	_searchEdit->setEnabled(true);
	_search->setEnabled(true);
	_close->setEnabled(true);
	_searchEdit->setFocus();
}
