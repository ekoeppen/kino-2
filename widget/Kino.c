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
 * File..........: Kino.c
 * Description...: Implementation of the main part of the Kino widget,
 *                 handling all Xt related functionality
 *
 * $Id: Kino.c,v 1.7 1999/07/12 12:13:14 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>
#include <X11/ConstrainP.h>
#include <X11/Constraint.h>
#include <X11/CoreP.h>
#include <X11/Xmu/Xmu.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "Kino.h"
#include "KinoP.h"

#ifdef MOTIF
#include <Xm/XmP.h>
#endif

/* local definitions */

#define AnyEventMask KeyPressMask | \
      KeyReleaseMask | \
      ButtonPressMask | \
      ButtonReleaseMask | \
      PointerMotionMask | \
      Button1MotionMask | \
      Button2MotionMask | \
      Button3MotionMask | \
      Button4MotionMask | \
      Button5MotionMask | \
      ButtonMotionMask

/* #define DEBUG */

/* local functions */

static void KinoClassInitialize (void);
static void KinoInitialize (Widget, Widget, ArgList, Cardinal *);
 void KinoDestroy (Widget);
static void KinoExpose (Widget,  XEvent *, Region);
static void KinoResize (Widget);
static void KinoInsertChild (Widget);
static void KinoDeleteChild (Widget);
static Boolean KinoSetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult KinoQueryGeometry (Widget, XtWidgetGeometry *,
  XtWidgetGeometry *);
static void KinoChangeManaged (Widget);
static XtGeometryResult KinoGeometryManager (Widget, XtWidgetGeometry *,
  XtWidgetGeometry *);
static void resize (KinoWidget, int, int);

static void KinoCvtStringToFontFamily (XrmValue *, Cardinal *, 
  XrmValue *, XrmValue *);

/* local variables */

static XtResource resources[] =
{
  {
    XtNtext,
    XtCString,
    XtRString,
    sizeof (String),
    XtOffset (KinoWidget, kino.text),
    XtRString,
    NULL
  },
  {
    XtNhtmlTitle,
    XtCReadOnly,
    XtRString,
    sizeof (String),
    XtOffset (KinoWidget, kino.title),
    XtRString,
    NULL
  },
  {
    XtNscriptCallback,
    XtCCallback,
    XtRCallback,
    sizeof (XtCallbackList),
    XtOffset (KinoWidget, kino.scriptCallbacks),
    XtRCallback,
    NULL
  },
  {
    XtNtagCallback,
    XtCCallback,
    XtRCallback,
    sizeof (XtCallbackList),
    XtOffset (KinoWidget, kino.tagCallbacks),
    XtRCallback,
    NULL
  },
  {
    XtNeventCallback,
    XtCCallback,
    XtRCallback,
    sizeof (XtCallbackList),
    XtOffset (KinoWidget, kino.eventCallbacks),
    XtRCallback,
    NULL
  },
  {
    XtNlinkCallback,
    XtCCallback,
    XtRCallback,
    sizeof (XtCallbackList),
    XtOffset (KinoWidget, kino.linkCallbacks),
    XtRCallback,
    NULL
  },
  {
    XtNfontFamily,
    XtCValue,
    XtRFontFamily,
    sizeof (int),
    XtOffset (KinoWidget, kino.defaultFontFamily),
    XtRString,
    FONT_FAMILY_TIMES
  },
  {
    XtNfontSize,
    XtCValue,
    XtRInt,
    sizeof (int),
    XtOffset (KinoWidget, kino.defaultFontSize),
    XtRImmediate,
    (XtPointer) 12
  },
  {
    XtNforeground,
    XtCForeground,
    XtRPixel,
    sizeof (Pixel),
    XtOffset (KinoWidget, kino.foreground),
    XtRString,
    "Black"
  },
  {
    XtNbackground,
    XtCBackground,
    XtRPixel,
    sizeof (Pixel),
    XtOffset (KinoWidget, core.background_pixel),
    XtRString,
    "White"
  },
  {
    XtNdefaultLMargin,
    XtCWidth,
    XtRDimension,
    sizeof (Dimension),
    XtOffset (KinoWidget, kino.defaultLMargin),
    XtRImmediate,
    (XtPointer) 20
  },
  {
    XtNdefaultRMargin,
    XtCWidth,
    XtRDimension,
    sizeof (Dimension),
    XtOffset (KinoWidget, kino.defaultRMargin),
    XtRImmediate,
    (XtPointer) 20
  },
  {
    XtNtotalLines,
    XtCReadOnly,
    XtRInt,
    sizeof (int),
    XtOffset (KinoWidget, kino.totalLines),
    XtRImmediate,
    (XtPointer) 0
  },
  {
    XtNtotalColumns,
    XtCReadOnly,
    XtRInt,
    sizeof (int),
    XtOffset (KinoWidget, kino.totalColumns),
    XtRImmediate,
    (XtPointer) 0
  },
  {
    XtNdefaultAutoFormat,
    XtCBoolean,
    XtRBoolean,
    sizeof (Boolean),
    XtOffset (KinoWidget, kino.defaultAutoFormat),
    XtRImmediate,
    (XtPointer) True
  },
  {
    XtNautoFormat,
    XtCBoolean,
    XtRBoolean,
    sizeof (Boolean),
    XtOffset (KinoWidget, kino.autoFormat),
    XtRImmediate,
    (XtPointer) True
  },
  {
    XtNuseHTMLStyles,
    XtCBoolean,
    XtRBoolean,
    sizeof (Boolean),
    XtOffset (KinoWidget, kino.useHTMLStyles),
    XtRImmediate,
    (XtPointer) True
  },
  {
    XtNfontScaling,
    XtCBoolean,
    XtRBoolean,
    sizeof (Boolean),
    XtOffset (KinoWidget, kino.fontScaling),
    XtRImmediate,
    (XtPointer) False
  },
  {
    XtNpainterEnable,
    XtCBoolean,
    XtRBoolean,
    sizeof (Boolean),
    XtOffset (KinoWidget, kino.painterEnable),
    XtRImmediate,
    (XtPointer) True
  },
  {
    XtNlayouterEnable,
    XtCBoolean,
    XtRBoolean,
    sizeof (Boolean),
    XtOffset (KinoWidget, kino.layouterEnable),
    XtRImmediate,
    (XtPointer) True
  },
  { 
    XtNwidth,
    XtCWidth,
    XtRDimension,
    sizeof (Dimension),
    XtOffset (KinoWidget, core.width),
    XtRImmediate,
    (XtPointer) 600
  },
  { 
    XtNheight,
    XtCHeight,
    XtRDimension,
    sizeof (Dimension),
    XtOffset (KinoWidget, core.height),
    XtRImmediate,
    (XtPointer) 400
  },
  {
    XtNlinkColor,
    XtCForeground,
    XtRPixel,
    sizeof (Pixel),
    XtOffset (KinoWidget, kino.linkColor),
    XtRString,
    "Blue",
  },
  {
    XtNaLinkColor,
    XtCForeground,
    XtRPixel,
    sizeof (Pixel),
    XtOffset (KinoWidget, kino.aLinkColor),
    XtRString,
    "Red",
  },
  {
    XtNvLinkColor,
    XtCForeground,
    XtRPixel,
    sizeof (Pixel),
    XtOffset (KinoWidget, kino.vLinkColor),
    XtRString,
    "Blue",
  },
  {
    XtNlinkCursor,
    XtCCursor,
    XtRCursor,
    sizeof (Cursor),
    XtOffset (KinoWidget, kino.linkCursor),
    XtRString,
    "hand2"
  },
  {
    XtNversion,
    XtCReadOnly,
    XtRString,
    sizeof (String),
    XtOffset (KinoWidget, kino.version),
    XtRImmediate,
    (String) "2.14.1"
  }
};

static CompositeClassExtensionRec extension = /* used to allow also
						 objects as children */
{
  NULL,
  NULLQUARK,
  XtCompositeExtensionVersion,
  sizeof (CompositeClassExtensionRec),
  TRUE,				/* allow objects as children */
  TRUE
};

static char translations[] =
  "<BtnUp>:         event-handler() \n\
   <BtnDown>:       event-handler() \n\
   <Key>:           event-handler() \n\
   <Motion>Normal:  event-handler()        ";

static XtActionsRec actions[] =
{
  {"event-handler",   EventHandler}
};

#ifdef MOTIF
#define superClass ((ConstraintWidgetClass)&xmManagerClassRec)
#else
#define superClass ((ConstraintWidgetClass)&constraintClassRec)
#endif

KinoClassRec kinoClassRec =	/* class data initialisation */
{
  {
    (WidgetClass) superClass,
    "Kino",
    sizeof (KinoRec),
    KinoClassInitialize,
    NULL,
    False,
    KinoInitialize,
    NULL,
    XtInheritRealize,
    actions,
    XtNumber (actions),
    resources,
    XtNumber (resources),
    NULLQUARK,
    False,
    True,
    True,
    False,
    KinoDestroy,
    KinoResize,
    KinoExpose,
    KinoSetValues,
    NULL,
    XtInheritSetValuesAlmost,
    NULL,
    NULL,
    XtVersion,
    NULL,
    translations,
    KinoQueryGeometry,
    XtInheritDisplayAccelerator,
    NULL
  },
  {
    KinoGeometryManager,
    KinoChangeManaged,
    KinoInsertChild,
    KinoDeleteChild,
    &extension
  },
  {
    NULL,
    0,
    sizeof(KinoConstraintsRec),
    NULL,
    NULL,
    NULL,
    NULL
  },
#ifdef MOTIF
  {
    XtInheritTranslations,                /* translations           */
    NULL,                                 /* syn resources          */
    0,                                    /* num syn_resources      */
    NULL,                                 /* get_cont_resources     */
    0,                                    /* num_get_cont_resources */
    XmInheritParentProcess,               /* parent_process       */
    NULL,                                 /* extension              */
  },
#endif
  {
    0
  }
};

WidgetClass kinoWidgetClass = (WidgetClass) &kinoClassRec;

#ifdef DEBUG
static int setValueDepth = 0;
#endif

/*-------------------------------------------------------------------------
 * Function......: KinoClassInitialize
 * Description...: Initialize the Kino widget class by registering a
 *                 resource converter for the font family resource
 *
 * In............: -
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the font family resource converter is registererd
 *-----------------------------------------------------------------------*/

void KinoClassInitialize (void)
{
#ifdef WAFE_AUS_EK
  extern XtAppContext wafeAppContext;

  XtAppAddActions(wafeAppContext, actions, XtNumber (actions));
#endif

  XtAddConverter (XtRString, XtRFontFamily, KinoCvtStringToFontFamily,
    NULL, 0);
}

/*-------------------------------------------------------------------------
 * Function......: KinoInitialize
 * Description...: Initialize the Kino widget and reset all data
 *                 fields, gets called by the intrinsics
 *
 * In............: Widget: requested widget (ATTENTION: this Widget
 *                 is allocated only on the caller's stack!)
 *                 Widget: new widget
 *                 ArgList, *Cardinal: additional creation-time arguments
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Kino widget is initialized
 *-----------------------------------------------------------------------*/

void KinoInitialize (Widget request, Widget new, ArgList args, Cardinal *num)
{
  KinoWidget w;

  w = (KinoWidget) new;

  /* reset all data */

  w->kino.document = NULL;
  w->kino.layouter = NULL;
  w->kino.painter = NULL;
  w->kino.inputPending = False;
  w->kino.totalLines = 0;
  w->kino.totalColumns = 0;
  w->kino.text = NULL;
  w->kino.title = NULL;
  w->kino.inSetValues = False;
  w->kino.inParser = False;
  w->kino.oldWidth = w->core.width;
  w->kino.elementUnderCursor = NULL;

  /* initialize the subparts */

  VisualInit ((Widget) w);
  w->kino.layouter = LayouterNew ();
  PainterInit (w);
}

/*-------------------------------------------------------------------------
 * Function......: KinoDestroy
 * Description...: Free all memory and delete the subparts, gets
 *                 called by the Intrinsics
 *
 * In............: Widget: the Kino widget to be destroyed
 * Out...........: -
 * Precondition..: -
 * Postcondition.: all memory is released
 *-----------------------------------------------------------------------*/

void KinoDestroy (Widget w)
{
  KinoWidget h;

  h = (KinoWidget) w;
  LayouterDelete (h->kino.layouter);
  PainterDelete (h);
  if (h->kino.title != NULL)
  {
    XkFree (h->kino.title);
  }
  if (h->kino.text != NULL)
  {
    XkFree (h->kino.text);
  }
  /*  StyleUnloadFonts (XtDisplay (w)); FIXME */
}

/*-------------------------------------------------------------------------
 * Function......: KinoExpose
 * Description...: Handle all expose events for the widget
 *
 * In............: Widget: the Kino widget
 *                 XEvent *: the expose event
 *                 Region: the exposed region
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void KinoExpose (Widget w,  XEvent *e, Region region)
{
  LargeRegion rect;
  KinoWidget k;

  k = (KinoWidget) w;

  /* remap the screen coordinates to logical coordinates */

  rect.x = e->xexpose.x;
  rect.y = e->xexpose.y;
  rect.width = e->xexpose.width;
  rect.height = e->xexpose.height;

#ifdef DEBUG
  fprintf(stderr,"+++ %s: (%d|%d)-(%dx%d)\n", __PRETTY_FUNCTION__,
    rect.x, rect.y, rect.width, rect.height);
#endif

  /* tell the Painter to update the exposed region */
  
  PainterShow (k, rect, False);
  
#ifdef MOTIF
  if (XtIsRealized (w) && w->core.visible) 
  {
    /* tell gadgets to redisplay themselves */
    
    _XmRedisplayGadgets (w, e, region);
  }
#endif
}

/*-------------------------------------------------------------------------
 * Function......: KinoInsertChild
 * Description...: Insert a child widget or also object into the widget's
 *                 child list. Gets called by the Intrinsics
 *                 Note: this code is pulled out of the original
 *                 Composite widget's code!
 *
 * In............: Widget: the widget/object to be inserted
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the widget is added to the child list
 *-----------------------------------------------------------------------*/

void KinoInsertChild (Widget w)
{
  Cardinal position;
  Cardinal i;
  CompositeWidget cw;
  WidgetList children;

  cw = (CompositeWidget) w->core.parent;
  children = cw->composite.children;
  
  if (cw->composite.insert_position != NULL)
  {
    position = (*(cw->composite.insert_position)) (w);
  }
  else
  {
    position = cw->composite.num_children;
  }
    
  if (cw->composite.num_children == cw->composite.num_slots)
  {
    cw->composite.num_slots +=  (cw->composite.num_slots / 2) + 2;
    cw->composite.children = children = 
      (WidgetList) XtRealloc((XtPointer) children,
	(unsigned) (cw->composite.num_slots) * sizeof(Widget));
  }
  for (i = cw->composite.num_children; i > position; i--)
  {
    children[i] = children[i-1];
  }
  children[position] = w;
  cw->composite.num_children++;
}

/*-------------------------------------------------------------------------
 * Function......: KinoDeleteChild
 * Description...: Delete a child widget or object from the Kino widget's
 *                 child list. Gets called by the Intrinsics
 *                 Note: this code is pulled out of the original
 *                 Composite widget's code!
 *
 * In............: Widget: the widget/object to be deleted
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the widget has been removed from the child list
 *-----------------------------------------------------------------------*/

void KinoDeleteChild (Widget w)
{
  Cardinal position;
  Cardinal i;
  CompositeWidget cw;

#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  cw = (CompositeWidget) w->core.parent;

  for (position = 0; position < cw->composite.num_children; position++)
  {
    if (cw->composite.children[position] == w)
    {
      break;
    }
  }
  if (position == cw->composite.num_children) return;
  
  /* Ripple children down one space from "position" */

  cw->composite.num_children--;
  for (i = position; i < cw->composite.num_children; i++)
  {
    cw->composite.children[i] = cw->composite.children[i+1];
  }
}

/*-------------------------------------------------------------------------
 * Function......: KinoResize
 * Description...: Handles resize requests, gets called by the Intrinsics
 *
 * In............: Widget: the Kino widget to be resized
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the widget is resized
 *-----------------------------------------------------------------------*/

void KinoResize (Widget w)
{
  KinoWidget k;
  LargeRegion rect;

  k = (KinoWidget) w;

#ifdef DEBUG
    fprintf (stderr, "+++ %s:  %dx%d\n", __PRETTY_FUNCTION__,
      w->core.width, w->core.height);
#endif

  if (k->kino.oldWidth != k->core.width)
  {
    /* relayout only of the widget's width changes */

    k->kino.oldWidth = k->core.width;

    /* reset the lines, the embedded graphics and the Layouter */
    
    rect = LayouterReformat (k->kino.layouter); /* do a new layout */
    PainterShow (k, rect, True); /* redisplay the widget's contents */
    resize (k, rect.width, rect.height);
      
    /* call the resize callback */
    
    /* XtCallCallbacks ((Widget) k, XtNsizeCallback, NULL);*/
  }
}

/*-------------------------------------------------------------------------
 * Function......: KinoSetValues
 * Description...: Handles SetValue requests, gets called by the Intrinsics
 *
 * In............: Widget: the current Kino widget
 *                 Widget: the requested Kino widget
 *                 Widget: the resulting Kino widget
 *                 ArgList, Cardinal *: additional arguments
 * Out...........: Boolean: True, if an expose event should be generated
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Boolean KinoSetValues (Widget current, Widget request, Widget new,
  ArgList args, Cardinal *num)
{
  KinoWidget k, c;
  Boolean redisplay;
  Boolean reparse;
  Boolean relayout;
  Parser *parser;
  Document *document;

  c = (KinoWidget) current;
  k = (KinoWidget) new;
  redisplay = False;
  reparse = False;
  relayout = False;

  k->kino.inSetValues = True;

#ifdef DEBUG
  setValueDepth++;
  fprintf (stderr,
    "+++ (%d start) %s: layouter: %d painter: %d\n",
    setValueDepth, __PRETTY_FUNCTION__, k->kino.layouterEnable,
    k->kino.painterEnable);
#endif

  if (c->kino.text != k->kino.text)
  {
    k->kino.inputPending = False;
    if (c->kino.text != NULL)
    {
      XkFree (c->kino.text);
    }
    k->kino.text = XkNewString (k->kino.text);

    reparse = True;
    relayout = True;
  }
  if (c->kino.defaultFontFamily != k->kino.defaultFontFamily ||
    c->kino.defaultFontSize != k->kino.defaultFontSize)
  {
    relayout = True;
  }
  if (c->kino.defaultLMargin != k->kino.defaultLMargin ||
    c->kino.defaultRMargin != k->kino.defaultRMargin ||
    c->core.width != k->core.width ||
    c->core.height != k->core.height)
  {
    relayout = True;
  }

  if (reparse)
  {
    /* 'destructive' values have been changed, so that the source */
    /* has to be parsed */

    EventReset (k);
    NodeDelete ((Node *) k->kino.document);

    parser = ParserNew ();

    document = DocumentNew ();
    DocumentAddTagHandler (document, KinoTagHandler, k);
    DocumentAddLinkHandler (document, KinoLinkHandler, k);
    DocumentAddEventHandler (document, KinoEventHandler, k);

    k->kino.document = document;

    parser->handling = HANDLE_REPLACE;
    parser->document = document;
    parser->startElement = ParserStartElement (parser, NULL);
    parser->startElement->display = DISPLAY_BLOCK;
    parser->startElement->floating = FLOAT_NONE;
    parser->startElement->anonymous = ANON_BOX;
    document->root = parser->startElement;

    ParserProcessData (parser, "text/html", k->kino.text);
  }

  if (k->kino.layouterEnable && relayout)
  {
    KinoRelayout ((Widget) k);

    if (XtIsManaged ((Widget) k))
    {
      redisplay = True;
    }
  }

#ifdef DEBUG
  fprintf (stderr, "+++ (%d end) %s: redisplay: %d\n", setValueDepth,
    __PRETTY_FUNCTION__, redisplay);
  setValueDepth--;
#endif

  k->kino.inSetValues = False;

  return redisplay;
}

/*-------------------------------------------------------------------------
 * Function......: KinoQueryGeometry
 * Description...: Handle geometry requests, gets called by the Intrinsics
 *
 * In............: Widget: the Kino widget
 *                 XtWidgetGeometry *: requested geometry
 *                 XtWidgetGeometry *: preferred geometry
 * Out...........: XtGeometryResult: how much change did we allow?
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

XtGeometryResult KinoQueryGeometry (Widget w, XtWidgetGeometry *request,
  XtWidgetGeometry *preferred)
{
  return XtGeometryYes;		/* all changes are allowed */
}

/*-------------------------------------------------------------------------
 * Function......: KinoGeometryManager
 * Description...: Handle geometry requests issued by a child, gets
 *                 called by the Intrinsics
 *
 * In............: Widget: the child widget
 *                 XtWidgetGeometry *: requested geometry
 *                 XtWidgetGeometry *: resulting geometry
 * Out...........: XtGeometryResult: how much change did we allow?
 * Precondition..: -
 * Postcondition.: - 
 *-----------------------------------------------------------------------*/

XtGeometryResult KinoGeometryManager (Widget w, XtWidgetGeometry *request,
  XtWidgetGeometry *reply)
{
  XtGeometryResult r;
  Boolean pos;
  Boolean size;
  KinoWidget parent;
  LargeRegion rect;

#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  parent = (KinoWidget) w->core.parent;
  pos = False;
  size = False;

  /* do not allow position requests */

  if (request->request_mode & CWX)
  {
    reply->x = w->core.x;
    pos = True;
  }
  if (request->request_mode & CWY)
  {
    reply->y = w->core.y;
    pos = True;
  }

  /* allow any size-changing request */

  if (request->request_mode & CWWidth && w->core.width != request->width)
  {
    w->core.width = request->width;
    size = True;
  }
  if (request->request_mode & CWHeight && w->core.height != request->height)
  {
    w->core.height = request->height;
    size = True;
  }
  if (request->request_mode & CWBorderWidth && 
    w->core.border_width != request->border_width)
  {
    w->core.border_width = request->border_width;
    size = True;
  }

  /* set the result flag according to our allowed changes */

  if (pos && !size) r = XtGeometryNo;
  if (!pos && size) r = XtGeometryYes;
  if (pos && size) r = XtGeometryAlmost;
  if (!pos && !size) r = XtGeometryYes;

  if (!parent->kino.inSetValues && !parent->kino.inParser && size)
  {
    /* the size of a child has changed, so we need to relayout */
    /* and redisplay */

    rect = LayouterReformat (parent->kino.layouter);
    resize (parent, rect.width, rect.height);
    PainterShow (parent, rect, True);
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: KinoChangeManaged
 * Description...: Handle the change of a child, gets called by the
 *                 Intrinsics
 *
 * In............: Widget: the Kino widget
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void KinoChangeManaged (Widget w)
{
#if 0
  KinoWidget k;
  LargeRegion rect;
  
  k = (KinoWidget) w;

#ifdef DEBUG
  fprintf (stderr, "+++ %s", __PRETTY_FUNCTION__);
#endif

  rect = LayouterReformat (k->kino.layouter);
  resize (k, r.width, r.height);
  PainterShow (k, rect, True);
  
#ifdef DEBUG
  fprintf (stderr, "\n");
#endif
#endif

#ifdef MOTIF
  _XmNavigChangeManaged (w);        
#endif
}

/*-------------------------------------------------------------------------
 * Function......: KinoCvtStringToFontFamily
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void KinoCvtStringToFontFamily (XrmValue *args, Cardinal *numArgs, 
  XrmValue *from, XrmValue *to)
{
  static int fontFamily;

  fontFamily = FONT_FAMILY_TIMES;
  if (XkStrEqual ((String) from->addr, "helvetica"))
  {
    fontFamily = FONT_FAMILY_HELVETICA;
  }
  else if (XkStrEqual ((String) from->addr, "courier"))
  {
    fontFamily = FONT_FAMILY_COURIER;
  }
  else if (XkStrEqual ((String) from->addr, "utopia"))
  {
    fontFamily = FONT_FAMILY_UTOPIA;
  }
  to->size = sizeof (FontFamily);
  to->addr = (XtPointer) &fontFamily;
}

/*-------------------------------------------------------------------------
 * Function......: KinoRefresh
 * Description...: Redisplay the whole visible region of the widget
 *
 * In............: Widget: the Kino widget
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void KinoRefresh (Widget k)
{
  LargeRegion r;

#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  r.x = 0;
  r.y = 0;
  r.width = ((KinoWidget) k)->core.width;
  r.height = ((KinoWidget) k)->kino.totalLines;
  PainterShow ((KinoWidget) k, r, True);
}

/*-------------------------------------------------------------------------
 * Function......: KinoRelayout
 * Description...: Relayout the source text
 *
 * In............: Widget: the Kino widget
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void KinoRelayout (Widget w)
{
  LargeRegion r;
  KinoWidget k;

#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  k = (KinoWidget) w;
  /*  EventReset (k); FIXME */
  k->kino.painterEnable = True;
  k->kino.layouter->document = k->kino.document;
  r = LayouterReformat (k->kino.layouter);
  resize (k, r.width, r.height);
  r.x = 0;
  r.y = 0;
  r.width = w->core.width;
  r.height = w->core.height;
  PainterShow (k, r, True);
}

/*-------------------------------------------------------------------------
 * Function......: resize
 * Description...: Update the width and height of the Kino widget
 *
 * In............: KinoWidget: the Kino widget containing the Layouter
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void resize (KinoWidget w, int width, int height)
{
  XtWidgetGeometry request, answer;

#ifdef DEBUG
  fprintf (stderr, "+++ %s: %dx%d\n", __PRETTY_FUNCTION__,
    w->kino.totalColumns, w->kino.totalLines);
#endif

  if (width == 0) width = 1;
  if (height == 0) height = 1;

  request.request_mode = CWHeight;
  if (height < SHRT_MAX)
  {
    request.height = max (height, w->core.parent->core.height);
  }
  else
  {
    request.height = SHRT_MAX;
  }

  if (width > w->core.width)
  {
    request.request_mode |= CWWidth;
    request.width = width;
  }

  if (XtMakeGeometryRequest ((Widget) w,
    &request, &answer) == XtGeometryAlmost)
  {
    request.width = answer.width;
    request.height = answer.height;
    XtMakeGeometryRequest ((Widget) w, &request, &answer);
  }
}

/*-------------------------------------------------------------------------
 * Function......: KinoTagHandler
 * Description...: 
 *                 
 * In............: HandlerInfo *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void KinoTagHandler (HandlerInfo *i)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  XtCallCallbacks ((Widget) i->userData, XtNtagCallback, i);
}

void KinoLinkHandler (HandlerInfo *i)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  XtCallCallbacks ((Widget) i->userData, XtNlinkCallback, i);
}

void KinoEventHandler (HandlerInfo *i)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  XtCallCallbacks ((Widget) i->userData, XtNeventCallback, i);
}

void KinoScriptHandler (HandlerInfo *i)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s\n", __PRETTY_FUNCTION__);
#endif

  XtCallCallbacks ((Widget) i->userData, XtNscriptCallback, i);
}

