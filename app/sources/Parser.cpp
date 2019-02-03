
/**
 * Class to manage parsing of internet results
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

#include "Parser.h"
#include "Choice.h"
#include "Config.h"
#include "Database.h"
#include "Film.h"
#include "MainWindow.h"
#include "Network.h"
#include "Progress.h"
#include "Utils.h"

#include <QDebug>
#include <QMessageBox>

#include <algorithm>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
Parser::Parser(Utils *utils)
	: _utils(utils)
{
}

Parser::~Parser()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~Parser()";
	#endif
}

bool Parser::complete(Film *film)
{
	// Here we can add more sites by combining them in the return statement with ||
	return this->completeWithAllocine(film);
}

void Parser::updateWithAllocineUrl(Film *film, const QString &url)
{
	// Parse Allocine film id in url
	QRegExp regex(QObject::tr("http://www\\.screenrush\\.co\\.uk/films/film-([0-9]+)/"));

	// If url contains the film id
	if (url.contains(regex))
	{
		Progress *progress = new Progress(QObject::tr("Update"), QObject::tr("Updating %1...").arg("<b>" + film->titleFromFilename() + "</b>"), 0);

		// Get film informations
		if (_utils->network()->get(QObject::tr("http://api.screenrush.co.uk/xml/movie?partner=YW5kcm9pZC12Mg&profile=large&code=") + regex.cap(1), Fk::FilmTimeout, Fk::FilmRetry))
		{
			// Parse response
			this->parseAllocineResponse(film, _utils->network()->response());

			delete progress;

			// Reload film to show informations
			_utils->window()->reloadCurrentFilm();
			QMessageBox::information(_utils->window(), QObject::tr("Ok"), QObject::tr("%1 has been updated successfully.").arg("<b>" + film->titleFromFilename() + "</b>"));
		}

		else
		{
			delete progress;

			// Reload film to show informations
			_utils->window()->reloadCurrentFilm();
			QMessageBox::critical(_utils->window(), QObject::tr("Error"), QObject::tr("Impossible to update %1.").arg("<b>" + film->titleFromFilename() + "</b>"));
		}
	}
}

bool Parser::allChoices(QString title, Film *film, QList<Choice*> &choicesList, bool updateMode)
{
	// Vars
	QString response;
	QRegExp regex("<table class=.totalwidth noborder purehtml.>(.*)</table>");
	regex.setMinimal(true);
	bool result = false;
	int level   = Fk::None;

	// Try to get some choices
	while ( ! result && level <= Fk::High)
	{
		// If network error
		if ( ! _utils->network()->get(QObject::tr("http://www.screenrush.co.uk/search/1/?q=") + this->clean(title, level), Fk::AllChoicesTimeout, Fk::AllChoicesRetry))
			return false;

		// Else, get response
		response = _utils->network()->response();

		// If some choices have been found
		if ( ! response.contains(QRegExp(QObject::tr("0 result found in film titles"))) && response.contains(regex))
		{
			// Save search title for Levenshtein algorithm comparaisons
			title = this->clean(title, level);
			result = true;
		}

		// Else, take an higher level (x2)
		else
			level <<= 1;
	}

	// If 0 results, return true
	if ( ! result)
		return true;

	// Get and parse response
	response = regex.cap(1).simplified();
	regex.setPattern("<img src='(.*)' alt='.*' />.*<a href='(.*)'>.*(.*)</a>(.*)<span class=.fs11.>(.*)<br />(.*)<br />(.*)<br />");

	// Take results counter
	int i = 0;

	// Parse each result
	while (response.contains(regex))
	{
		QPixmap jacket;

		// If no update mode, get the mini-jacket
		if ( ! updateMode)
		{
			if (regex.cap(1).simplified() != QObject::tr("http://images.screenrush.co.uk/r_75_106/commons/emptymedia/emptysearchposter.gif"))
				if (_utils->network()->get(regex.cap(1).simplified(), Fk::MiniJacketTimout, Fk::MiniJacketRetry))
					jacket.loadFromData(_utils->network()->response());

			if (jacket.isNull())
				jacket.load(":/images/defaultJacket");
		}

		// Parse original title
		QRegExp regexOriginalTitle("\\((.+)\\)");
		QString originalTitle;

		if (regex.cap(4).contains(regexOriginalTitle))
			originalTitle = regexOriginalTitle.cap(1);

		// Create choice
		Choice *choice = new Choice(_utils, film,
									title,													// Search title
									regex.cap(3).remove("<b>").remove("</b>").simplified(), // Title
									originalTitle,											// Original title
									regex.cap(5).remove("<b>").remove("</b>").simplified(), // Year
									regex.cap(7).remove("<b>").remove("</b>").simplified(), // Actors
									regex.cap(6).remove("<b>").remove("</b>").simplified(), // Realisators
									jacket,													// Jacket
									QObject::tr("http://www.screenrush.co.uk") + regex.cap(2).simplified());
		choicesList.append(choice);

		// Remove current choice from response (to prevent for re-matching it)
		response.remove(QRegExp("<img src='" + QRegExp::escape(regex.cap(1)) + "' alt='.*' />.*<a href='" + QRegExp::escape(regex.cap(2)) + "'>" + QRegExp::escape(regex.cap(3)) + "</a>"));

		// Results counter
		i++;

		// Take only the 10 first results for update mode
		if (updateMode && i >= 10)
			break;

		// Else take 15 results
		else if (i >= 20)
			break;
	}

	return true;
}

/*--------------------------------------------------------------------*\
		Private
\*--------------------------------------------------------------------*/
int Parser::levenshtein(const QString &string1, const QString &string2) const
{
	// Vars
	int len1 = string1.count();
	int len2 = string2.count();
	int distance[len1 + 1][len2 + 1];

	// Init
	for (int i = 0; i <= len1; i++)
		distance[i][0] = i;

	for (int j = 0; j <= len2; j++)
		distance[0][j] = j;

	// Algorithm (insert, move, delete)
	for (int i = 1; i <= len1; i++)
		for (int j = 1; j <= len2; j++)
			distance[i][j] = std::min(std::min(distance[i-1][j] + 1, distance[i][j-1] + 1), distance[i-1][j-1] + (string1[i-1] != string2[j-1]));

	// Return distance between string1 and string2
	return distance[len1][len2];
}

QString Parser::clean(QString title, int level)
{
	// Low: remove CD1, CD2 etc..., some keywords like vost (case insensitive)
	if (level & Fk::Low)
		return title.remove(QRegExp("CD[1-9]|vost|vostfr|xvid|divx|dvdrip|unrated|collector|screener", Qt::CaseInsensitive))
					.simplified();

	// Medium: same as Low + replace "." with " " (space), and remove the "vo" which remains at end of title, and some others smaller keywords (all still case insensitive)
	else if(level & Fk::Medium)
		return this->clean(title, Fk::Low).replace(".", " ")
										  .remove(QRegExp("[ .-](720p|1080p|hd|dvd|tv|fr|vo|en|us)", Qt::CaseInsensitive))
										  .remove(QRegExp("(720p|1080p|hd|dvd|tv|fr|vo|en|us)[ .-]", Qt::CaseInsensitive))
										  .simplified();

	// High: same as Low and Medium + remove numbers and special characters, then replace I with 1, II with 2, etc...
	else if (level & Fk::High)
	{
		QString result(clean(title, Fk::Medium).remove(QRegExp("[0-9]"))
											   .remove(QRegExp("[" + QRegExp::escape("\"&~#{(|[]-_^@)}=+$%*,?;:!") + "]"))
											   .simplified());

		// Replace I => 1, II => 2, III => 3...
		QRegExp regex("[ .-](I+)");

		if (result.contains(regex))
			result = result.replace(regex.cap(0), QString::number(regex.cap(1).count()));

		return result;
	}

	// No change
	return title;
}

bool Parser::completeWithAllocine(Film *film)
{
	QList<Choice*> choices;

	// Get all choices
	if (this->allChoices(film->title(), film, choices, true) && choices.count() > 0)
	{
		// Find best choice with Levenshtein algorithm

		// Better choice is the first
		Choice *better = choices.at(0);

		// Compare with title and original title
		int lev        = this->levenshtein(better->searchTitle(), better->title());
		int levTemp    = this->levenshtein(better->searchTitle(), better->originalTitle());

		if (levTemp < lev)
			lev = levTemp;

		// If there is at least one more film and the current best choice isn't a perfect match (= to 0)
		if (lev > 0 && choices.count() > 1)
		{
			// Foreach others choices
			for (int i = 1; i < choices.count(); i++)
			{
				// Compare title
				levTemp = this->levenshtein(choices.at(i)->searchTitle(), choices.at(i)->title());

				if (levTemp < lev)
				{
					lev = levTemp;
					better = choices.at(i);
				}

				// Compare original title if the best choice isn't a perfect match yet (= to 0)
				if (lev > 0)
				{
					levTemp = this->levenshtein(choices.at(i)->searchTitle(), choices.at(i)->originalTitle());

					if (levTemp < lev)
					{
						lev = levTemp;
						better = choices.at(i);
					}
				}
			}
		}

		// Match code in best choice url
		QRegExp regex(QObject::tr("http://www\\.screenrush\\.co\\.uk/films/film-([0-9]+)/"));
		QString code;

		if (better->url().contains(regex))
			code = regex.cap(1);

		// Delete choices instances
		foreach (Choice *choice, choices)
			if (choice != NULL)
				delete choice;

		// Get film infos of the best choice
		if ( ! code.isEmpty())
		{
			if ( ! _utils->network()->get(QObject::tr("http://api.screenrush.co.uk/xml/movie?partner=YW5kcm9pZC12Mg&profile=large&code=") + code, Fk::FilmTimeout, Fk::FilmRetry))
				return false;

			// Parse response
			this->parseAllocineResponse(film, _utils->network()->response());
			return true;
		}
	}

	return false;
}

void Parser::parseAllocineResponse(Film *film, const QString &response) const
{
	QByteArray jacket;
	QRegExp regex;
	regex.setMinimal(true);
	regex.setCaseSensitivity(Qt::CaseInsensitive);

	// Title
	regex.setPattern("<title>(.+)</title>");

	if (response.contains(regex))
		film->setTitle(regex.cap(1).replace("&amp;", "&").simplified());

	// Original title
	regex.setPattern("<originalTitle>(.+)</originalTitle>");

	if (response.contains(regex))
		film->setOriginalTitle(regex.cap(1).replace("&amp;", "&").simplified());

	// Genders
	regex.setPattern("<genreList>(.+)</genreList>");

	if (response.contains(regex))
	{
		QString genders;
		QString tempGenders(regex.cap(1).simplified());
		regex.setPattern("<genre code=.[0-9]+.>(.+)</genre>");

		while (tempGenders.contains(regex))
		{
			genders += regex.cap(1).simplified() + ", ";
			tempGenders.remove(regex.cap(0));
		}

		genders.chop(2);
		film->setGenders(genders);
	}

	// Year
	regex.setPattern("<productionYear>([0-9]{4})</productionYear>");

	if (response.contains(regex))
		film->setYear(regex.cap(1));

	// Duration
	regex.setPattern("<runtime>([0-9]+)</runtime>");

	if (response.contains(regex))
		film->setDuration(regex.cap(1).toInt() / 60);

	// Country
	regex.setPattern("<nationalityList>(.+)</nationalityList>");

	if (response.contains(regex))
	{
		QString countries;
		QString tempCountries(regex.cap(1).simplified());
		regex.setPattern("<nationality code=.[0-9]+.>(.+)</nationality>");

		while (tempCountries.contains(regex))
		{
			countries += regex.cap(1).simplified() + ", ";
			tempCountries.remove(regex.cap(0));
		}

		countries.chop(2);
		film->setCountries(countries);
	}

	// Casting
	regex.setPattern( "<casting>(.+)</casting>" );

	if( response.contains( regex ) )
	{
		QString actors;
		QString realisators;
		QString casting(regex.cap(1).simplified());

		// Realisators
		regex.setPattern("<castMember><person code=.[0-9]+.><name>(.+)</name></person>.*<activity code=.8002.>.*</activity>.*</castMember>");

		while (casting.contains(regex))
		{
			realisators += regex.cap(1).simplified() + ", ";
			casting.remove(regex.cap(0));
		}

		realisators.chop(2);
		film->setRealisators(realisators);

		// Actors
		regex.setPattern("<castMember><person code=.[0-9]+.><name>(.+)</name></person>.*<activity code=.8001.>.*</activity>.*</castMember>");
		int i = 0;

		// 20 actors maximum
		while (i < 20 && casting.contains(regex))
		{
			actors += regex.cap(1).simplified() + ", ";
			casting.remove(regex.cap(0));
			i++;
		}

		actors.chop(2);
		film->setActors(actors.replace("&amp;quot;", "\""));
	}

	// Synopsis
	regex.setPattern("<synopsis>(.+)</synopsis>");

	if (response.contains(regex))
	{
		QString synopsis(regex.cap(1));

		// Replace "<b>", "<i>", "</b>", "</i>" and "<br />" xml encoded and html balises with "[balise-name]"
		synopsis = synopsis.replace(QRegExp("&lt;(b|i|/b|/i|br */|br|/br)&gt;"), "[\\1]")
						   .replace(QRegExp("<(b|i|/b|/i|br */|br|/br)>"), "[\\1]");

		// Fusion [br][/br] in [br]
		synopsis = synopsis.replace("[br][/br]", "[br]");

		// Delete all others xml encoded and html balises
		synopsis = synopsis.remove(QRegExp("&lt;[^&]*&gt;"))
						   .remove(QRegExp("<[^<]*>"));

		// Replace &quot; and &amp; with normal caracters
		synopsis = synopsis.replace("&amp;", "&")
							.replace("&quot;", "\"");

		// Set film synopsis (in plain text)
		film->setSynopsis(synopsis);
	}

	// Jacket
	regex.setPattern("<poster path=.* href=.(.+).></poster>");

	if (response.contains(regex))
	{
		QString urlJacket(regex.cap(1));

		if (urlJacket != QObject::tr("http://images.screenrush.co.uk/r_160_214/b_1_cfd7e1/commons/emptymedia/emptyposter.gif") && _utils->network()->get(urlJacket, Fk::JacketTimeout, Fk::JacketRetry))
			jacket = _utils->network()->response();
	}

	// Trailer URL
	regex.setPattern("<trailer code=.[0-9]+. href=.(.*).></trailer>");

	if (response.contains(regex))
		film->setTrailerUrl(regex.cap(1));

	// Users note
	regex.setPattern("<userRating>(.+)</userRating>");

	if (response.contains(regex))
		film->setNote(regex.cap(1).toFloat() * 2.0);

	film->checkIfComplete(jacket.isEmpty());
	_utils->db()->updateFilm(film, true, jacket);
}
