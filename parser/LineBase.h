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
 * File..........: Line.h
 * Description...: Declaration of the Line structure which holds
 *                 information about the layout of displayable lines
 *                 of text
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: LineBase.h,v 1.3 1999/03/29 14:34:18 koeppen Exp $
 *
 * $Log: LineBase.h,v $
 * Revision 1.3  1999/03/29 14:34:18  koeppen
 * added LGPL text
 *
 * Revision 1.2  1999/02/17 16:30:54  koeppen
 * changed from PData to DOM functions
 *
 * Revision 1.1.1.1  1998/12/21 17:16:43  koeppen
 * initial checkin
 *
 * Revision 1.3  1998/12/01 20:49:42  koeppen
 * Vorbereitung für Komponenten
 *
 * Revision 1.2  1998/01/22 13:02:48  koeppen
 * Kommentar
 *
 * Revision 1.1  1998/01/14 16:05:18  koeppen
 * Initial revision
 *-----------------------------------------------------------------------*/

#ifndef __LINEBASE_H
#define __LINEBASE_H

/* global definitions */

#define MAX_LINE_ELEM 1024

/* global types */

typedef struct _Line		/* the line data */
{
  int x;			/* position and size */
  int y;
  int height;
  int width;
  int totalWidth;		/* width from margin to margin */
  int base;			/* baseline of the text */
  Node *element[MAX_LINE_ELEM];	/* line elements */
  int numElements;
  int started;
} Line;

/* global functions */

void LineDelete (Line *);

#endif

