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
 * File..........: Entity.h
 * Description...: Declaration of the Entity structure which holds
 *                 Entity references
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: Entity.h,v 1.2 1999/03/29 14:34:09 koeppen Exp $
 *
 * $Log: Entity.h,v $
 * Revision 1.2  1999/03/29 14:34:09  koeppen
 * added LGPL text
 *
 * Revision 1.1.1.1  1998/12/21 17:16:43  koeppen
 * initial checkin
 *
 * Revision 1.2  1998/12/01 20:49:42  koeppen
 * Vorbereitung für Komponenten
 *
 * Revision 1.1  1998/06/01 17:21:59  koeppen
 * Initial revision
 *
 *-----------------------------------------------------------------------*/

#ifndef __ENTITY_H
#define __ENTITY_H

/* global types */

typedef struct _Entity		/* the entity structure */
{
  char *name;			/* name of the attribute */
  char *value;			/* value, might be NULL */
  struct _Entity *next;
} Entity;

char* EntityLookup (Entity *, char *);
void EntityDelete (Entity *);

#endif
