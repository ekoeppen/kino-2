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
 * File..........: Event.c
 * Description...: 
 *
 * $Id: Event.c,v 1.3 1999/04/14 14:33:58 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Kino.h"
#include "KinoP.h"

/* #define DEBUG */

/*-------------------------------------------------------------------------
 * Function......: EventReset
 * Description...: 
 *                 
 * In............: KinoWidget :
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void EventReset (KinoWidget k)
{
  k->kino.elementUnderCursor = NULL;
}

/*-------------------------------------------------------------------------
 * Function......: EventHandler
 * Description...: 
 *                 
 * In............: Widget :
 *                 XEvent *:
 *                 String *:
 *                 Cardinal *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void EventHandler (Widget w, XEvent *event, String *params, 
  Cardinal *numParams)
{
  Element *box;
  EventHandlerInfo info;
  KinoWidget k;

#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  k = (KinoWidget) w;
  if (k->kino.document)
  {
    box = NULL;
    info.event = (struct _KinoEvent *) event;
    switch (event->type)
    {
      case ButtonRelease:
	info.reason = EE_CLICK;
	box = LayouterFindBox (k->kino.document->root,
	  ((XButtonEvent *) event)->x, ((XButtonEvent *) event)->y);
	break;
      case KeyPress:
	info.reason = EE_KEY;
	box = LayouterFindBox (k->kino.document->root,
	  ((XKeyEvent *) event)->x, ((XKeyEvent *) event)->y);
	break;
      case MotionNotify: 
	info.reason = EE_MOTION;
	box = LayouterFindBox (k->kino.document->root,
	((XMotionEvent *) event)->x, ((XMotionEvent *) event)->y);
	break;
    }
    
    info.source = (Node *) box;
    if (box != NULL)
    {
      DocumentCallEventHandler (k->kino.document, &info);
    }

#ifdef FIXME
    if (event->type == MotionNotify)
    {
      box = NULL;
      info.event = (struct _KinoEvent *) event;
      info.reason = EE_MOTION;
      box = LayouterFindBox (k->kino.document->root,
	((XMotionEvent *) event)->x, ((XMotionEvent *) event)->y);

      if (box != k->kino.elementUnderCursor)
      {
	info.event = NULL;
	if (k->kino.elementUnderCursor != NULL)
	{
	  info.reason = EE_LEAVE;
	  info.source = k->kino.elementUnderCursor;
	  DocumentCallEventHandler (k->kino.document, &info);
	}
	if (box != NULL)
	{
	  info.reason = EE_ENTER;
	  info.source = box;
	  DocumentCallEventHandler (k->kino.document, &info);
	}
	k->kino.elementUnderCursor = box;
      }
    }
#endif
  }
}

