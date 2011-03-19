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
 * File..........: Painter.c
 * Description...: Implementation of the Painter structure which realizes
 *                 the drawing of the lines and other associated tasks
 *
 * $Id: Painter.c,v 1.4 1999/07/12 12:13:19 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include "Kino.h"
#include "KinoP.h"

/* #define DEBUG */

/* local functions */

static void changeGC (KinoWidget, XID, KinoColor, KinoColor, int);
static void drawWord (KinoWidget, Element *,
  Text *, int, int, LargeRegion);
static void drawBox (KinoWidget, Element *, int, int, LargeRegion);
static void drawHRule (KinoWidget, Element *, int, int, int);
static Pixel loadColor (Display *display, int r, int g, int b);

/*-------------------------------------------------------------------------
 * Function......: PainterInit
 * Description...: Initializes the painter structure in a Kino widget
 *                 by deleting, reallocating and resetting the painter
 *
 * In............: KinoWidget: widget containing the painter
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the painter is allocated and reset
 *-----------------------------------------------------------------------*/

void PainterInit (KinoWidget w)
{
  Painter *p;

  p = XkNew (Painter);
  w->kino.painter = p;
  w->kino.painter->gc = NULL;
}

/*-------------------------------------------------------------------------
 * Function......: PainterReset
 * Description...: Resets the painter subpart of the Kino widget
 *
 * In............: KinoWidget: the Kino widget containing the painter
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the painter subpart is reset
 *-----------------------------------------------------------------------*/

void PainterReset (KinoWidget w)
{
  if (XtWindow (w) != None)
  {
    /*    XClearWindow (XtWindow (w)); */
    XUndefineCursor (XtDisplay (w), XtWindow (w));
  }
}

/*-------------------------------------------------------------------------
 * Function......: PainterDelete
 * Description...: Resets the painter subpart of the Kino widget
 *
 * In............: KinoWidget: the Kino widget containing the painter
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the painter subpart is reset
 *-----------------------------------------------------------------------*/

void PainterDelete (KinoWidget w)
{
  if (w->kino.painter != NULL)
  {
    if (w->kino.painter->gc != NULL)
    {
      XFreeGC (XtDisplay (w), w->kino.painter->gc);
    }
    XkFree ((char *) w->kino.painter);
    w->kino.painter = NULL;
  }
}


/*-------------------------------------------------------------------------
 * Function......: PainterRepositionInsets
 * Description...: Move all insets to their correct position
 *
 * In............: KinoWidget: widget containing the insets
 * Out...........: -
 * Precondition..: -
 * Postcondition.: all insets have been moved to their correct position
 *-----------------------------------------------------------------------*/

void PainterRepositionInsets (KinoWidget k)
{
}

/*-------------------------------------------------------------------------
 * Function......: PainterShow
 * Description...: Redisplay the visible region of the widget
 *
 * In............: KinoWidget: the Kino widget
 *                 LargeRegion: region to be displayed
 *                 Boolean: move the insets?
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void PainterShow (KinoWidget w, LargeRegion r, Boolean move)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s: (%d|%d)-(%dx%d)\n", __PRETTY_FUNCTION__,
    r.x, r.y, r.width, r.height);
#endif

  if (XtWindow (w) != None && w->kino.document && w->kino.painterEnable)
  {
    /* move all insets to their new places */
  
    if (move) PainterRepositionInsets (w);

    /* clear the exposed area */

    XClearArea (XtDisplay (w), XtWindow (w),
      r.x, r.y, r.width, r.height, False);

    /* draw all Node elements */

    drawBox (w, w->kino.document->root, 0, 0, r);
  }
}

/*-------------------------------------------------------------------------
 * Function......: PainterFindNode
 * Description...: Locate the Node element under a given screen position
 *
 * In............: KinoWidget: the Kino widget
 *                 int: horizontal screen position
 *                 int: vertical screen position
 * Out...........: Node *: found Node element or NULL
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Node *PainterFindNode (KinoWidget w, int px, int py)
{
  return NULL;
}

/*-------------------------------------------------------------------------
 * Function......: drawBox
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void drawBox (KinoWidget w, Element *box, int x, int y, LargeRegion r)
{
  Node *p;
  Region regExpose, regBox, empty, intersect;
  XRectangle re, rb;

  if (box != NULL && box->display != DISPLAY_NONE &&
    (box->display == DISPLAY_INLINE ||
    !(x + box->x + box->width < r.x || x + box->x > r.x + r.width ||
      y + box->y + box->height < r.y || y + box->y > r.y + r.height)))
  {
    x = x + box->x;
    y = y + box->y;

    if (box->display == DISPLAY_BLOCK)
    {
      if (box->background_color.alpha == 0)
      {
	changeGC (w, ((XFontStruct *) box->font)->fid,
	  box->background_color, box->background_color, 0);

	rb.x = x + ElementTotalLeft (box);
	rb.y = y + ElementTotalTop (box);
	rb.width = box->width -
	  ElementTotalLeft (box) - ElementTotalRight (box);
	rb.height = box->height -
	  ElementTotalTop (box) - ElementTotalBottom (box);

	re.x = r.x; re.y = r.y; re.width = r.width; re.height = r.height;

	empty = XCreateRegion ();
	regBox = XCreateRegion ();
	XUnionRectWithRegion (&rb, empty, regBox);
	regExpose = XCreateRegion ();
	XUnionRectWithRegion (&re, empty, regExpose);
	intersect = XCreateRegion ();
	XIntersectRegion (regBox, regExpose, intersect);

	XSetRegion (XtDisplay (w), w->kino.painter->gc, intersect);
	XFillRectangle (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
	  r.x, r.y, r.width, r.height); 
	
	XDestroyRegion (regBox);
	XDestroyRegion (regExpose);
	XDestroyRegion (empty);
	XDestroyRegion (intersect);
      }

      if (box->border_top_color.alpha == 0 &&
	box->border_top_width.effective > 0)
      {
	changeGC (w, ((XFontStruct *) box->font)->fid,
	  box->border_top_color, box->background_color,
	  box->border_top_width.effective);
	XDrawLine (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
	  x + box->margin_left.effective, 
	  y + box->margin_top.effective,
	  x + box->width - box->margin_right.effective,
	  y + box->margin_top.effective);
      }

      if (box->border_bottom_color.alpha == 0 &&
	box->border_bottom_width.effective > 0)
      {
	changeGC (w, ((XFontStruct *) box->font)->fid,
	  box->border_bottom_color, box->background_color,
	  box->border_bottom_width.effective);
	XDrawLine (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
	  x + box->margin_left.effective, 
	  y + box->height - box->margin_bottom.effective,
	  x + box->width - box->margin_right.effective,
	  y + box->height - box->margin_bottom.effective);
      }

      if (box->border_left_color.alpha == 0 &&
	box->border_left_width.effective > 0)
      {
	changeGC (w, ((XFontStruct *) box->font)->fid,
	  box->border_left_color, box->background_color,
	  box->border_left_width.effective);
	XDrawLine (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
	  x + box->margin_left.effective, 
	  y + box->margin_top.effective,
	  x + box->margin_left.effective, 
	  y + box->height - box->margin_bottom.effective);
      }

      if (box->border_right_color.alpha == 0 &&
	box->border_right_width.effective > 0)
      {
	changeGC (w, ((XFontStruct *) box->font)->fid,
	  box->border_right_color, box->background_color,
	  box->border_right_width.effective);
	XDrawLine (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
	  x + box->width - box->margin_right.effective, 
	  y + box->margin_top.effective,
	  x + box->width - box->margin_right.effective, 
	  y + box->height - box->margin_bottom.effective);
      }
    }

    changeGC (w, ((XFontStruct *) box->font)->fid,
      box->color, box->background_color, 1);

    p = box->contents;
    while (p != NULL)
    {
      switch (p->type)
      {
	case TEXT_NODE:
	  drawWord (w, box, (Text *) p, x, y, r);
	  break;
	case ELEMENT_NODE:
	  drawBox (w, (Element *) p, x, y, r);
	  changeGC (w, ((XFontStruct *) box->font)->fid,
	    box->color, box->background_color, 1);
	  break;
	default: 
	  break;
      }
      p = p->next;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: changeGC
 * Description...: Change the current graphics context according to a
 *                 given style
 *
 * In............: KinoWidget: the Kino widget containing the GC
 *                 Box *: box containing the new style
 * Out...........: -
 * Precondition..: the style must contain valid font data and the
 *                 graphics context must be valid
 * Postcondition.: the graphics context has been changed
 *-----------------------------------------------------------------------*/

void changeGC (KinoWidget w, XID font,
  KinoColor f, KinoColor b, int lineWidth)
{
  XGCValues values;
  Pixel fp, bp;

  fp = loadColor (XtDisplay ((Widget) w), f.red, f.green, f.blue);
  bp = loadColor (XtDisplay ((Widget) w), b.red, b.green, b.blue);
  if (w->kino.painter->gc == NULL)
  {
    /* if there was no previous GC, allocate a new one */

    w->kino.painter->gc = XCreateGC (XtDisplay (w), XtWindow (w), 0, NULL);
  }

  assert (w->kino.painter->gc != NULL);
  values.font = font;
  values.foreground = fp;
  values.background = bp;
  values.line_width = lineWidth;
  values.clip_mask = None;
  XChangeGC (XtDisplay (w),
    w->kino.painter->gc,
    GCFont | GCForeground | GCBackground | GCLineWidth | GCClipMask,
    &values);
  assert (w->kino.painter->gc != NULL);
}

/*-------------------------------------------------------------------------
 * Function......: drawHRule
 * Description...: Draw a horzontal rule with shadows
 *
 * In............: KinoWidget: the Kino widget
 *                 Style *: style for the rule (color)
 *                 int: horizontal position
 *                 int: vertical position
 *                 int: length of the rule
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void drawHRule (KinoWidget w, Element *box, int x, int y, int len)
{
  XSetForeground (XtDisplay (w), w->kino.painter->gc,
    loadColor (XtDisplay (w), 26, 26, 26));
  XDrawLine (XtDisplay (w), XtWindow (w),
    w->kino.painter->gc, x, y, x + len, y);
  XDrawLine (XtDisplay (w), XtWindow (w),
    w->kino.painter->gc, x, y, x, y + 1);

  XSetForeground (XtDisplay (w), w->kino.painter->gc,
    loadColor (XtDisplay (w), 242, 242, 242));
  XDrawLine (XtDisplay (w), XtWindow (w),
    w->kino.painter->gc, x + 1, y + 1, x + len, y + 1);
  XDrawLine (XtDisplay (w), XtWindow (w),
    w->kino.painter->gc, x + len, y, x + len, y + 1);
  XSetForeground (XtDisplay (w), w->kino.painter->gc,
    loadColor (XtDisplay (w), box->color.red, box->color.green,
      box->color.blue));
}

/*-------------------------------------------------------------------------
 * Function......: drawWord
 * Description...: Draw a word
 *
 * In............: KinoWidget: the Kino widget
 *                 Element *: 
 *                 Text *: 
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void drawWord (KinoWidget w, Element *box,
  Text *word, int x, int y, LargeRegion r)
{
  x += word->x;
  y += word->y;
  if (!(x + word->width < r.x || x > r.x + r.width ||
    y - VisualFontAscent (box->font) + word->height < r.y ||
    y - VisualFontAscent (box->font) > r.y + r.height))
  {
    XDrawString (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
      x, y, word->text, strlen (word->text));
    if (box->underline)
    {
      XDrawLine (XtDisplay (w), XtWindow (w), w->kino.painter->gc,
	x, y + 1, x + word->width, y + 1); 
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: loadColor
 * Description...: 
 *                 
 * In............: int 
 *                 int 
 *                 int 
 * Out...........: Pixel 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Pixel loadColor (Display *display, int r, int g, int b)
{
  XColor color;

  color.red = r;
  color.green = g;
  color.blue = b;
  if (display)
  {
    if (XAllocColor (display,
      DefaultColormap (display, DefaultScreen (display)), &color))
    {
      return color.pixel;
    }
    else
    {
      return BlackPixel (display, DefaultScreen (display));
    }
  }
  else
  {
    return 0;
  }
}
