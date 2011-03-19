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
 * File..........: 
 * Description...: 
 *
 * $Id: Handler.h,v 1.6 1999/06/30 17:29:22 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __HANDLER_H
#define __HANDLER_H

/* forward declarations */

struct _Element;
struct _Document;
struct _Parser;
struct _Event;
struct _KinoEvent;

/* global types */

/* information for a tag found during parsing */

typedef struct _TagHandlerInfo
{
  char *name;			/* name of the tag */
  char *namespace;		/* namespace part */
  Attribute *attributes;	/* attribute list */
  int empty;			/* empty tag? */
  struct _Parser *parser;	/* associated parser */
} TagHandlerInfo;

/* information for hyperlink references */

typedef struct _LinkHandlerInfo
{
  struct _Element *element;	/* element containing the link */
  struct _Parser *parser;	/* associated parser */
} LinkHandlerInfo;

/* information for IEM events */

typedef enum
{
  EE_NONE = 0,
  EE_CLICK = 1,
  EE_DBL_CLICK = 2,
  EE_MOTION = 4,
  EE_ENTER = 8,
  EE_LEAVE = 16,
  EE_KEY = 32,
  EE_LOAD = 64,
  EE_UNLOAD = 128,
  EE_EXTERNAL = 256,
  EE_MATCH = 512,
  EE_MOD_SUBTREE = 1024
} EventCode;

typedef struct _EventHandlerInfo
{
  EventCode reason;
  struct _KinoEvent *event;
  struct _Node *source;		/* source of the event */
  struct _Parser *parser;
} EventHandlerInfo;

/* information for embedded scripts */

typedef struct _ScriptHandlerInfo
{
  struct _Element *caller;	/* element calling the script */
  struct _Element *element;	/* element containing the script */
  char *script;			/* the script to be executed */
  char *params;			/* parameter string for the script */
  char *ret;			/* return value */
  char *MIMEType;		/* MIME type of the script */

  EventCode reason;		/* triggering event */
  struct _KinoEvent *event;
} ScriptHandlerInfo;

/* general information for all handler procedures */

typedef struct _HandlerInfo
{
  struct _Document *document;
  void *userData;

  union
  {
    TagHandlerInfo *tag;
    LinkHandlerInfo *link;
    EventHandlerInfo *event;
    ScriptHandlerInfo *script;
  } info;
} HandlerInfo;

/* the handler function type */

typedef void (*HandlerProc)(HandlerInfo *);

/* list of handler functions */

typedef struct _Handler
{
  HandlerProc handler;
  void *userData;
  struct _Handler *next;
} Handler;

/* global functions */

TagHandlerInfo* TagHandlerInfoNew (void);
LinkHandlerInfo* LinkHandlerInfoNew (void);
EventHandlerInfo* EventHandlerInfoNew (void);
ScriptHandlerInfo* ScriptHandlerInfoNew (void);
HandlerInfo* HandlerInfoNew (void);
Handler* HandlerNew (void);

void TagHandlerInfoDelete (TagHandlerInfo *);
void LinkHandlerInfoDelete (LinkHandlerInfo *);
void EventHandlerInfoDelete (EventHandlerInfo *);
void ScriptHandlerInfoDelete (ScriptHandlerInfo *);
void HandlerInfoDelete (HandlerInfo *);
void HandlerDelete (Handler *);

#endif
