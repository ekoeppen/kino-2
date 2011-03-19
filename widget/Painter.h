/*-------------------------------------------------------------------------
 * kinowidget - an XML display widget, part of the Kino XML/CSS processor
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
 * File..........: Painter.h
 * Description...: Declaration of the Painter structure which realizes
 *                 the drawing of the lines and other associated tasks
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: Painter.h,v 1.3 1999/04/14 14:34:11 koeppen Exp $
 *
 * $Log: Painter.h,v $
 * Revision 1.3  1999/04/14 14:34:11  koeppen
 * renamed startBox and box in the parser structure, added LGPL comments
 *
 * Revision 1.2  1999/02/17 16:33:16  koeppen
 * changed from PData to DOM functions
 *
 * Revision 1.1.1.1  1998/12/21 17:16:45  koeppen
 * initial checkin
 *
 * Revision 1.3  1998/02/18 13:03:20  koeppen
 * Xk... entfernt
 *
 * Revision 1.2  1998/01/22 13:02:48  koeppen
 * Kommentar
 *
 * Revision 1.1  1998/01/14 16:07:39  koeppen
 * Initial revision
 *-----------------------------------------------------------------------*/

#ifndef __PAINTER_H
#define __PAINTER_H

/* global types */

typedef struct			/* the Painter subpart data */
{
  GC gc;			/* graphics context used for drawing */
} Painter;

/* global functions */

void PainterInit (KinoWidget);
void PainterReset (KinoWidget);
void PainterDelete (KinoWidget);
void PainterShow (KinoWidget, LargeRegion, Boolean);
void PainterRepositionInsets (KinoWidget);
Node *PainterFindNode (KinoWidget, int, int);

#endif
