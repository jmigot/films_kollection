
/**
 * Class to manage network
 * This class is common to main program and updater
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

#ifndef NETWORK_H
#define NETWORK_H

#include <QString>

class QNetworkAccessManager;
class QNetworkRequest;

class Network
{
	private:
		QNetworkAccessManager* _manager;
		QByteArray             _response;
		QString                _error;

	public:
		Network();
		~Network();

		bool get(const QString &urlAddress, int timeout, int nbRetry = 0);

		// Get
		QByteArray response() const;
		QString    error()    const;

	private:
		bool get(const QNetworkRequest &request, int timeout);
};

#endif
