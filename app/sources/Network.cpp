
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

#include "Network.h"

#include <QApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTime>
#include <QUrl>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Network::Network()
{
	_manager = new QNetworkAccessManager;
}

Network::~Network()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Network()";
	#endif

	// Delete manager as it has no QObject parent
	delete _manager;
}

bool Network::get(const QString &urlAddress, int timeout, int nbRetry)
{
	// Clear response and error texts
	_response.clear();
	_error.clear();

	// Create objects needed for a http request (url, networkmanager to make request and networkreply to store response)
	QUrl url(urlAddress, QUrl::TolerantMode);
	QNetworkRequest request(url);

	// Set User-Agent because some websites want it (Mozilla words are used to prevent from ban)
	request.setRawHeader("User-Agent", QObject::tr("Mozilla (en) Gecko/Firefox").toUtf8());

	bool result = false;
	int nbTries = 0;

	while ( ! result && nbTries <= nbRetry)
	{
		result = this->get(request, timeout);
		nbTries++;
	}

	return result;
}

// Get
QByteArray Network::response() const
{
	return _response;
}

QString Network::error() const
{
	return _error;
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
bool Network::get(const QNetworkRequest &request, int timeout)
{
	QNetworkReply *reply(_manager->get(request));

	// Create timeout period
	QTime dieTime(QTime::currentTime().addSecs(timeout));

	// Wait for response or for timeout (processEvents will refresh windows while waiting)
	while ( ! reply->isFinished() && QTime::currentTime() < dieTime)
		QApplication::processEvents();

	// If timeout
	if ( ! reply->isFinished())
	{
		_error = QObject::tr("Timeout");
		reply->abort();
		reply->deleteLater();
		return false;
	}

	// If no error
	else if (reply->error() == QNetworkReply::NoError)
	{
		_response = reply->readAll();
		reply->deleteLater();
		return true;
	}

	else
	{
		// An error occured
		_error = reply->errorString();
		reply->deleteLater();
		return false;
	}
}
