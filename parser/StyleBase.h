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
 * File..........: StyleBase.h
 * Description...: Declaration of the Style structure which holds
 *                 information about text styles (color, font, etc)
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: StyleBase.h,v 1.3 1999/03/29 14:34:24 koeppen Exp $
 *
 * $Log: StyleBase.h,v $
 * Revision 1.3  1999/03/29 14:34:24  koeppen
 * added LGPL text
 *
 * Revision 1.2  1999/02/17 16:31:03  koeppen
 * changed from PData to DOM functions
 *
 * Revision 1.1.1.1  1998/12/21 17:16:43  koeppen
 * initial checkin
 *
 *-----------------------------------------------------------------------*/

#ifndef __STYLEBASE_H
#define __STYLEBASE_H

/* global types */

typedef void (PropertySetHandler)(Element *, char *);
typedef char *(PropertyGetHandler)(Element *);

typedef struct			/* a complete property handler element */
{
  char *name;			/* name of handled property */
  PropertySetHandler *setHandler; /* handlers for the property */
  PropertyGetHandler *getHandler;
} Property;

typedef struct
{
  char *selector;
  char *declaration;
} PredefinedRule;

typedef struct _RuleSet
{
  char *selector;
  char *declaration;
  struct _RuleSet *next;
} RuleSet;

/* exported functions */

void RuleSetDelete (RuleSet *);

void StyleBaseSetBoxDisplay (Element *);
int StyleBaseParsePropertyName (char **, char *);
int StyleBaseParsePropertyValue (char **, char *);
void StyleBaseGetBoxStyles (Element *, char *);
void StyleBaseConvertFloating (Float *, char *);
void StyleBaseConvertDisplay (DisplayStyle *, char *);
void StyleBasePrintProperties (Element *);

/* global variables */

extern PredefinedRule StylePredefinedRules[];

#endif
