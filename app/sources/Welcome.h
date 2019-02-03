
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

#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>

class Utils;

class QGridLayout;
class QLabel;

class Welcome: public QWidget
{
	private:
		Utils*       _utils;
		QGridLayout* _layout;
		QLabel*      _logo0;
		QLabel*      _logo1;
		QLabel*      _logo2;
		QLabel*      _logo3;
		QLabel*      _logo4;
		QLabel*      _text1;
		QLabel*      _text2;
		QLabel*      _text3;
		QLabel*      _text4;
		QLabel*      _text5;

	public:
		Welcome(Utils *utils);
		~Welcome();
};

#endif
