
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

#ifndef PARSER_H
#define PARSER_H

#include <QString>

class Choice;
class Film;
class Utils;

namespace Fk
{
	enum ParserCleanLevel
	{
		None   = 0x01,
		Low    = 0x02,
		Medium = 0x04,
		High   = 0x08
	};
}

class Parser
{
	private:
		Utils* _utils;

	public:
		Parser(Utils *utils);
		~Parser();

		bool complete(Film *film);
		bool allChoices(QString title, Film *film, QList<Choice*> &choicesList, bool updateMode = false);
		void updateWithAllocineUrl(Film *film, const QString &code);

	private:
		int     levenshtein(const QString &chaine1, const QString &chaine2) const;
		QString clean(QString title, int level);
		bool    completeWithAllocine(Film *film);
		void    parseAllocineResponse(Film *film, const QString &response) const;
};

#endif
