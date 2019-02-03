
/**
 * QWidget to create custom progress bars waiters
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

#ifndef PROGRESS_H
#define PROGRESS_H

#include <QWidget>

class QLabel;
class QProgressBar;
class QPushButton;
class QVBoxLayout;

class Progress: public QWidget
{
	Q_OBJECT

	private:
		QVBoxLayout*  _layout;
		QLabel*       _text;
		QProgressBar* _progressBar;
		QPushButton*  _stop;

	public:
		Progress(const QString &title, const QString &text, int nbElements);
		~Progress();

		void setMax(int max);
		void installStopButton();
		void setText(const QString &text);
		void incrementValue();

		// Get
		bool isStopped() const;

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void stop();
};

#endif
