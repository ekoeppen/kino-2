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
 * File..........: KinoP.h
 * Description...: Declaration of the private part of the Kino widget
 *
 * $Id: KinoP.h,v 1.5 1999/04/14 14:34:02 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __KINOP_H
#define __KINOP_H

#include <X11/Xmu/Converters.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Core.h>
#include <X11/Constraint.h>
#include <stdio.h>
#include <stdlib.h>

#include "KinoParser.h"
#include "DOM.h"
#include "Event.h"
#include "Layouter.h"
#include "Line.h"
#include "Painter.h"
#include "Style.h"
#include "Visual.h"

#ifdef MOTIF
#include "Xm/ManagerP.h"
#endif

/* private types */

typedef struct _KinoClassPart	/* additional class data */
{
  int empty;			/* unused */
} KinoClassPart;

typedef struct _KinoConstraintsRec /* constraint data for the widget's
				      children */
{
#ifdef MOTIF
  XmManagerConstraintPart manager;
#endif
  int dummy;			/* unused */
} KinoConstraintsRec, *KinoConstraints;

typedef struct _KinoClassRec	/* class record */
{
  CoreClassPart core_class;
  CompositeClassPart composite_class;
  ConstraintClassPart constraint_class;	/* this is here for Motif,
					   since the Manager class is
					   a subclass of Constraint! */
#ifdef MOTIF
  XmManagerClassPart manager_class;
#endif
  KinoClassPart kino_class;
} KinoClassRec;

typedef struct			/* the actual Kino widget data */
{
  char *text;			/* source text */
  int textLength;		/* length of source text */
  Document *document;		/* parsed data (document root) */

  Layouter *layouter;		/* the Layouter subpart */
  Painter *painter;		/* the Painter subpart */
  Boolean inputPending;		/* is there still text to be appended? */
  int oldWidth;			/* old width of the widget */

  Boolean painterEnable;	/* is the Painter enabled? */
  Boolean layouterEnable;	/* is the Layouter enabled? */
  Boolean inSetValues;		/* are we in the SetValues function? */
  Boolean inParser;		/* are we currenty parsing? */

  int totalLines;		/* total number of lines (in pixels) */
  int totalColumns;		/* total number of columns (in pixels) */
  XtCallbackList scriptCallbacks;   /* script handler callback */
  XtCallbackList eventCallbacks; /* event occured at an element */
  XtCallbackList linkCallbacks; /* fetch new data */
  XtCallbackList tagCallbacks;	/* handle tags */
  FontFamily defaultFontFamily;	/* default font family to use */
  int defaultFontSize;		/* base font size to use*/
  Boolean fontScaling;		/* allow font scaling? */
  Pixel foreground;		/* current foreground color */
  Dimension defaultLMargin;	/* left and right margins */
  Dimension defaultRMargin;
  Boolean defaultAutoFormat;	/* default parsing mode */
  Boolean autoFormat;	        /* current parsing mode */
  String title;			/* parsed title */
  Element *elementUnderCursor;	/* element currently under the cursor */
  Cursor linkCursor;		/* cursor to be used while on anchors */
  Pixel linkColor;		/* colors to be used for anchor text */
  Pixel aLinkColor;
  Pixel vLinkColor;
  Boolean useHTMLStyles;	/* use predefined CSS styles for HTML tags? */
  String *version;		/* current version */
} KinoPart;

typedef struct _KinoRec		/* the Kino widget data */
{
  CorePart core;
  CompositePart composite;
  ConstraintPart constraint;
#ifdef MOTIF
  XmManagerPart manager;
#endif    
  KinoPart kino;
} KinoRec;

void KinoTagHandler (HandlerInfo *);
void KinoLinkHandler (HandlerInfo *);
void KinoScriptHandler (HandlerInfo *);
void KinoEventHandler (HandlerInfo *);

#endif
