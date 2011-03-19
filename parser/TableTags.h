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
 * File..........: TableTag.h
 * Description...: Declaration of the TableTag object which handles the
 *                 TABLE tag
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: TableTags.h,v 1.2 1999/03/29 14:34:28 koeppen Exp $
 *
 * $Log: TableTags.h,v $
 * Revision 1.2  1999/03/29 14:34:28  koeppen
 * added LGPL text
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
 * Revision 1.1  1998/01/14 16:07:39  koeppen
 * Initial revision
 *-----------------------------------------------------------------------*/

#ifndef __TABLETAG_H
#define __TABLETAG_H

/* global functions */

int TableTagsHandler (Parser *, TagHandlerInfo *);

#endif
