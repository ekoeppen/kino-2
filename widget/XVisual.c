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
 * File..........: XVisual.c
 * Description...: 
 *
 * $Id: XVisual.c,v 1.9 1999/07/12 12:13:23 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Xmu.h>
#include <limits.h>

#include "Kino.h"
#include "KinoP.h"

/* local types */

typedef struct _FontInfo	/* data for cached fonts */
{
  int references;		/* reference count */
  char *name;			/* full font name */
  XFontStruct *font;		/* allocated font structure */
  struct _FontInfo *next;
} FontInfo;

static FontInfo *lookupFontInfo (char *name);
static FontInfo *addFontInfo (Display *d, char *name);

/* local variables */

static char *fontFamily[] =
{
  "times",
  "courier",
  "helvetica",
  "utopia",
  "symbol"
};

static char *fontWeight[] =
{
  "medium",
  "bold"
};

static char *fontSlant[] =
{
  "r",
  "i",
  "o"
};

static int fontSizes[] =
{
  8, 10, 12, 14, 18, 24
};

static FontInfo *fontCache = NULL;
static Display *display = NULL;

int InsetDataGetWidth (InsetData *i)
{
  Widget w;

  w = (Widget) i;
  return w->core.width + 2 * w->core.border_width;
}

int InsetDataGetHeight (InsetData *i)
{
  Widget w;

  w = (Widget) i;
  return w->core.height + 2 * w->core.border_width;
}

void InsetDataDestroy (InsetData *i)
{
  XtDestroyWidget ((Widget) i);
}

void InsetDataManage (InsetData *i)
{
  XtManageChild ((Widget) i);
}

void InsetDataUnmanage (InsetData *i)
{
  XtUnmanageChild ((Widget) i);
}

void InsetDataMove (InsetData *i, int x, int y)
{
  XtUnmanageChild ((Widget) i);
  XtMoveWidget ((Widget) i, x, y);
  XtManageChild ((Widget) i);
}

/*-------------------------------------------------------------------------
 * Function......: VisualInit
 * Description...: 
 *                 
 * In............: Widget 
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void VisualInit (void *w)
{
  display = XtDisplay ((Widget) w);
}

/*-------------------------------------------------------------------------
 * Function......: VisualChangeManagedInsets
 * Description...: 
 *                 
 * In............: Element *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void VisualChangeManagedInsets (Element *box)
{
  Node *p;
  Element *parent;
  int managed;

  for (p = box->contents; p != NULL; p = p->next)
  {
    switch (p->type)
    {
      case ELEMENT_NODE:
	VisualChangeManagedInsets ((Element *) p);
	break;
      case INSET_NODE:
	managed = 1;
	for (parent = p->parent; parent != NULL; parent = parent->parent)
	{
	  if (parent->display == DISPLAY_NONE) managed = 0;
	}
	if (managed) InsetDataManage (((Inset *) p)->inset);
	else InsetDataUnmanage (((Inset *) p)->inset);
	break;
      case TEXT_NODE:
      case LINEBREAK_NODE:
      case DOCUMENT_NODE:
      case PROCESSING_INSTRUCTION_NODE:
      case ATTRIBUTE_NODE:
      case CDATA_SECTION_NODE:
      case ENTITY_REFERENCE_NODE:
      case ENTITY_NODE:
      case COMMENT_NODE:
      case DOCUMENT_TYPE_NODE:
      case DOCUMENT_FRAGMENT_NODE:
      case NOTATION_NODE:
	break;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: VisualLoadFont
 * Description...: Set the font data in a style using the font cache
 *
 * In............: Display *: current X11 display
 *                 Style *: style object where to set the font data
 *                 Boolean: allow font scaling
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the font data is set and possibly cached
 *-----------------------------------------------------------------------*/

KinoFont *VisualLoadFont (FontFamily family,
  int size,
  FontWeight weight,
  FontSlant slant,
  int scaling)
{
  char fontname[80];
  FontInfo *f;
  int i;
  int distance;
  int s;

  if (!scaling)
  {
    /* find the closest font size */

    distance = INT_MAX;
    s = size;
    for (i = 0; i < 6; i++)
    {
      if (abs (fontSizes[i] - size) < distance)
      {
	distance = abs (fontSizes[i] - size);
	s = fontSizes[i];
      }
    }
    size = s;
  }

  if (family != FONT_FAMILY_HELVETICA ||
    slant != FONT_SLANT_ITALIC)
  {
    /* construct the font name */

    sprintf (fontname, "-adobe-%s-%s-%s-*-*-%d-*-*-*-*-*-*-*",
      fontFamily[family],
      fontWeight[weight],
      fontSlant[slant], size);
  }
  else
  {
    /* construct the font name for Helvetica Oblique */

    sprintf (fontname, "-adobe-%s-%s-%s-*-*-%d-*-*-*-*-*-*-*",
      fontFamily[family],
      fontWeight[weight],
      fontSlant[FONT_SLANT_OBLIQUE], size);
  }
  f = lookupFontInfo (fontname); /* lookup font in the font cache */
  if (f != NULL)
  {
    f->references++;
  }
  else
  {
    f = addFontInfo (display, fontname); /* allocate new cache entry */
  }

  if (f)
  {
    return (KinoFont *) f->font;
  }
  else
  {
    return NULL;
  }
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void VisualUnloadFonts (void)
{
  FontInfo *f, *h;

  f = fontCache;
  while (f != NULL)
  {
    h = f->next;
    XkFree (f->name);
    if (display)
    {
      XFreeFont (display, f->font);
    }
    XkFree (f);
    f = h;
  }
}

/*-------------------------------------------------------------------------
 * Function......: VisualTextWidth
 * Description...: Returns the width of a given text using a given font
 *
 * In............: Style *: style object containing the font data
 *                 char *: text
 * Out...........: int: text width in pixels
 * Precondition..: -
 * Postcondition.: the style object must contain valid font data
 *-----------------------------------------------------------------------*/

int VisualTextWidth (KinoFont *font, char *t)
{
  if (font && t)
  {
    return XTextWidth ((XFontStruct *) font, t, strlen (t));
  }
  else
  {
    return 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: VisualFontHeight
 * Description...: Returns the maximum height of text using a given font
 *
 * In............: KinoFont *: style object containing the font data
 * Out...........: int: maximum text height
 * Precondition..: -
 * Postcondition.: the style object must contain valid font data
 *-----------------------------------------------------------------------*/

int VisualFontHeight (KinoFont *font)
{
  if (font)
  {
    return ((XFontStruct *) font)->max_bounds.ascent +
      ((XFontStruct *) font)->max_bounds.descent;
  }
  else
  {
    return 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: VisualFontAscent
 * Description...: Returns the maximum ascent of a font
 *
 * In............: KinoFont *: style object containing the font data
 * Out...........: int: maximum ascent
 * Precondition..: -
 * Postcondition.: the style object must contain valid font data
 *-----------------------------------------------------------------------*/

int VisualFontAscent (KinoFont *font)
{
  if (font)
  {
    return ((XFontStruct *) font)->max_bounds.ascent;
  }
  else
  {
    return 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: VisualFontDescent
 * Description...: Returns the maximum descent of a font
 *
 * In............: KinoFont *: style object containing the font data
 * Out...........: int: maximum descent
 * Precondition..: -
 * Postcondition.: the style object must contain valid font data
 *-----------------------------------------------------------------------*/

int VisualFontDescent (KinoFont *font)
{
  if (font)
  {
    return ((XFontStruct *) font)->max_bounds.descent;
  }
  else
  {
    return 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void VisualTextExtents (KinoFont *font, char *text,
  int *width, int *lbearing, int *rbearing, int *ascent, int *descent)
{
  XCharStruct x;
  int dummy;

  if (font)
  {
    XTextExtents ((XFontStruct *) font, text, strlen (text),
      &dummy, &dummy, &dummy, &x);
    *ascent = x.ascent;
    *descent = x.descent;
  }
  else
  {
    *width = 0;
    *lbearing = 0;
    *rbearing = 0;
    *ascent = 0;
    *descent = 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: InsetDataButtonNew
 * Description...: 
 *                 
 * In............: VisualWidget
 *                 Element *
 * Out...........: InsetData *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

InsetData *InsetDataButtonNew (VisualWidget widget, Element *parent)
{
  return NULL;
}

void InsetDataButtonDelete (InsetData *inset)
{
}

void InsetDataButtonSetValue (InsetData *inset, char *value)
{
}

char *InsetDataButtonGetValue (InsetData *inset)
{
  return NULL;
}

/*-------------------------------------------------------------------------
 * Function......: InsetDataTextareaNew
 * Description...: 
 *                 
 * In............: VisualWidget
 *                 Element *
 * Out...........: InsetData *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

InsetData *InsetDataTextareaNew (VisualWidget widget, Element *parent)
{
  return NULL;
}

void InsetDataTextareaDelete (InsetData *inset)
{
}

void InsetDataTextareaSetValue (InsetData *inset, char *value)
{
}

char *InsetDataTextareaGetValue (InsetData *inset)
{
  return NULL;
}

/*-------------------------------------------------------------------------
 * Function......: InsetDataInputNew
 * Description...: 
 *                 
 * In............: VisualWidget
 *                 Element *
 * Out...........: InsetData *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

InsetData *InsetDataInputNew (VisualWidget widget, Element *parent)
{
  return NULL;
}

void InsetDataInputDelete (InsetData *inset)
{
}

void InsetDataInputSetValue (InsetData *inset, char *value)
{
}

char *InsetDataInputGetValue (InsetData *inset)
{
  return NULL;
}

/*-------------------------------------------------------------------------
 * Function......: lookupFontInfo
 * Description...: Look for a named font in the font cache
 *
 * In............: char *: name of the font
 * Out...........: FontInfo *: font data or NULL, if the font does not
 *                 exist in the font cache
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

FontInfo *lookupFontInfo (char *name)
{
  FontInfo *r;

  r = fontCache;
  while (r != NULL && !XkStrEqual (r->name, name))
  {
    r = r->next;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: addFontInfo
 * Description...: Load and add a named font to the font cache
 *
 * In............: Display *: current X11 display
 *                 char *: full name of the font
 * Out...........: FontInfo *: font data for the font
 * Precondition..: -
 * Postcondition.: the font is allocated and cached
 *-----------------------------------------------------------------------*/

FontInfo *addFontInfo (Display *d, char *name)
{
  FontInfo *r;

  r = NULL;
  if (d)
  {
    r = XkNew (FontInfo);
    r->name = XkNewString (name);
    r->next = fontCache;
    r->references = 1;
    r->font = XLoadQueryFont (d, r->name);
    if (r->font == NULL)
    {
      /* if the font does not exist replace it with a standard font */
      
      r->font = XLoadQueryFont (d, "fixed"); /*** FIXME -> cache this! ***/
    }
    
    assert (r->font != NULL);
    
    fontCache = r;
  }

  return r;
}
