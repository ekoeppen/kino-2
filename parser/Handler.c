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
 * $Id: Handler.c,v 1.5 1999/04/14 14:32:14 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/*-------------------------------------------------------------------------
 * Function......: ...HandlerInfoNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: ...HandlerInfo* 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

TagHandlerInfo* TagHandlerInfoNew (void)
{
  TagHandlerInfo *i;

  i = XkNew (TagHandlerInfo);
  i->name = NULL;
  i->namespace = NULL;
  i->attributes = NULL;
  i->empty = 0;
  i->parser = NULL;

  return i;
}

LinkHandlerInfo* LinkHandlerInfoNew (void)
{
  LinkHandlerInfo *i;

  i = XkNew (LinkHandlerInfo);
  i->element = NULL;
  i->parser = NULL;

  return i;
}

EventHandlerInfo* EventHandlerInfoNew (void)
{
  EventHandlerInfo *i;

  i = XkNew (EventHandlerInfo);
  i->reason = EE_NONE;
  i->source = NULL;
  i->event = NULL;
  i->parser = NULL;

  return i;
}

ScriptHandlerInfo* ScriptHandlerInfoNew (void)
{
  ScriptHandlerInfo *i;

  i = XkNew (ScriptHandlerInfo);
  i->script = "";
  i->params = "";
  i->element = NULL;
  i->caller = NULL;
  i->event = NULL;
  i->reason = EE_NONE;
  i->ret = NULL;
  i->MIMEType = NULL;

  return i;
}

HandlerInfo* HandlerInfoNew (void)
{
  HandlerInfo *i;

  i = XkNew (HandlerInfo);
  i->document = NULL;
  i->userData = NULL;
  memset (&i->info, 0, sizeof (i->info));

  return i;
}

Handler* HandlerNew (void)
{
  Handler *h;

  h = XkNew (Handler);
  h->handler = NULL;
  h->userData = NULL;
  h->next = NULL;

  return h;
}

/*-------------------------------------------------------------------------
 * Function......: ...HandlerInfoDelete
 * Description...: 
 *                 
 * In............: ...HandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void TagHandlerInfoDelete (TagHandlerInfo *i)
{
  if (i != NULL)
  {
    if (i->name != NULL) XkFree (i->name);
    if (i->namespace != NULL) XkFree (i->namespace);
    AttributeDelete (i->attributes);
    XkFree (i);
  }
}

void LinkHandlerInfoDelete (LinkHandlerInfo *i)
{
  if (i != NULL) XkFree (i);
}

void EventHandlerInfoDelete (EventHandlerInfo *i)
{
  if (i != NULL) XkFree (i);
}

void ScriptHandlerInfoDelete (ScriptHandlerInfo *i)
{
  if (i != NULL)
  {
    XkFree (i);
  }
}

void HandlerInfoDelete (HandlerInfo *i)
{
  if (i != NULL) XkFree (i);
}

void HandlerDelete (Handler *h)
{
  Handler *current;

  while (h != NULL)
  {
    current = h;
    h = h->next;
    XkFree (current);
  }
}


