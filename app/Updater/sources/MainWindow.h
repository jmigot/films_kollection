
/**
 * QWidget
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>

class Config;
class Network;

class QLabel;
class QProgressBar;
class QVBoxLayout;

class MainWindow: public QWidget
{
	Q_OBJECT

	private:
		Network*      _network;
		float         _version;
		QVBoxLayout*  _layout;
		QLabel*       _text;
		QProgressBar* _progressBar;

	public:
		MainWindow(float version);
		~MainWindow();

		void run();
		void update();

	protected:
		void closeEvent(QCloseEvent *event);

	private:
		void launchProgram();
};

#endif
