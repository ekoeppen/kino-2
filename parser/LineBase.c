/*-------------------------------------------------------------------------
 * kinoparser - an XML/CSS parser, part of the Kino XML/CSS processor
 *
 * Copyright (c) 1996-1999 Eckhart Köppen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * File..........: Line.c
 * Description...: Implementation of the Line structure which holds
 *                 information about the layout of displayable lines
 *                 of text
 *
 * $Id: LineBase.c,v 1.4 1999/03/29 14:34:17 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/*-------------------------------------------------------------------------
 * Function......: LineDelete
 * Description...: 
 *                 
 * In............: Line *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void LineDelete (Line *line)
{
  XkFree (line);
}
