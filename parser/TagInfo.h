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
 * File..........: TagInfo.h
 * Description...: Declaration of the TagInfo structure which holds
 *                 the name and attributes of a parsed tag
 *
 * $Id: TagInfo.h,v 1.3 1999/03/29 14:34:29 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __TAGINFO_H
#define __TAGINFO_H

/* global definitions */

/* reason codes for parser callback activation */

#define REASON_HANDLE_TAG    0
#define REASON_PARSER_RESET  1
#define REASON_PARSER_DELETE 2

/* maximum identifier lengths according to HTML DTD */

#define MAX_NAME_LENGTH      128
#define MAX_ATTR_LENGTH     1024

#define MAX_LITERAL_LENGTH 65536

/* global types */

struct _Parser;
struct _Element;

typedef struct _TagInfo
{
  int reason;			/* reason of callback activation */
#ifdef STANDALONE
  void *event;
#else
  XEvent *event;		/* event that caused activation  */
#endif
  char *name;			/* name of the tag */
  char *namespace;		/* namespace part */
  Attribute *attributes;	/* attribute list */
  Boolean empty;		/* empty tag? */
  struct _Parser *parser;	/* associated parser */
  struct _Element *newBox;	/* newly created box for this tag */
  struct _TagInfo *next;
} TagInfo;

#endif
