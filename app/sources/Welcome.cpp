
/**
 * QWidget displayed at opening of program
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

#include "Welcome.h"
#include "Config.h"
#include "MainWindow.h"
#include "Utils.h"

#include <QDebug>
#include <QGridLayout>
#include <QLabel>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Welcome::Welcome(Utils *utils)
	: _utils(utils)
{
	// Members
	_layout = new QGridLayout(this);
	_logo0  = new QLabel;
	_logo1  = new QLabel;
	_logo2  = new QLabel;
	_logo3  = new QLabel;
	_logo4  = new QLabel;
	_text1  = new QLabel;
	_text2  = new QLabel;
	_text3  = new QLabel;
	_text4  = new QLabel;
	_text5  = new QLabel;

	// Properties
	this->setStyleSheet("background-color: #FFFFFF");

	// Logos
	_logo0->setPixmap(_utils->scaleToHeight(QPixmap(":/images/logo"), Fk::LogoRatio));
	_logo1->setPixmap(_utils->scale(QPixmap(":/images/add"), Fk::WelcomeRatio));
	_logo2->setPixmap(_utils->scale(QPixmap(":/images/update"), Fk::WelcomeRatio));
	_logo3->setPixmap(_utils->scale(QPixmap(":/images/benefit"), Fk::WelcomeRatio));
	_logo4->setPixmap(_utils->scale(QPixmap(":/images/help"), Fk::WelcomeRatio));

	// Texts
	_text1->setText("<h2>" + QObject::tr("Add films to the collection") + "</h2>");
	_text2->setText("<h2>" + QObject::tr("Update sheets with internet") + "</h2>");
	_text3->setText("<h2>" + QObject::tr("Enjoy your collection") + "</h2>");
	_text4->setText("<h2>" + QObject::tr("Tools -> Help") + "</h2>");
	_text5->setText("<h3><a style='text-decoration:none; color: #5555CC' href='" + Fk::UrlHome + "'>" + Fk::UrlHome + "</a></h3>");
	_text5->setOpenExternalLinks(true);

	// Layouts
	_layout->addWidget(_logo0, 0, 0, 1, 3);
	_layout->addWidget(_logo1, 1, 0);
	_layout->addWidget(_logo2, 2, 0);
	_layout->addWidget(_logo3, 3, 0);
	_layout->addWidget(_logo4, 4, 0);
	_layout->addWidget(_text1, 1, 1);
	_layout->addWidget(_text2, 2, 1);
	_layout->addWidget(_text3, 3, 1);
	_layout->addWidget(_text4, 4, 1);
	_layout->addWidget(_text5, 5, 1);
	_layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
}

Welcome::~Welcome()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Welcome()";
	#endif
}
