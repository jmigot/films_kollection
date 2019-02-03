
/**
 * QWidget to show statistics of collection
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

#ifndef STATISTICS_H
#define STATISTICS_H

#include "Widget.h"

class Utils;

class QFormLayout;
class QGroupBox;

class Statistics: public Widget
{
	Q_OBJECT

	private:
		QGroupBox*     _boxSeenFilms;
		QGroupBox*     _boxDuration;
		QGroupBox*     _boxSize;
		QVBoxLayout*   _layoutSeenFilms;
		QFormLayout*   _layoutDuration;
		QFormLayout*   _layoutSize;
		QHBoxLayout*   _notesLayout;
		QList<QLabel*> _notes;

	public:
		Statistics(Utils *utils);
		~Statistics();

	private:
		QString duration(int duration) const;
};

#endif
