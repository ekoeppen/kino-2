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
 * $Id$
 *-----------------------------------------------------------------------*/

#include <limits.h>
#include <gtk/gtk.h>
#include "gtkkino.h"
#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"

/* local types */

typedef struct _FontInfo	/* data for cached fonts */
{
  int references;		/* reference count */
  char *name;			/* full font name */
  GdkFont *font;		/* allocated font structure */
  struct _FontInfo *next;
} FontInfo;

static FontInfo *lookupFontInfo (char *);
static FontInfo *addFontInfo (char *);
static void buttonCallback (GtkWidget *, gpointer);
static void entryFocusOutCallback (GtkWidget *, GdkEvent *, gpointer);
static void textFocusOutCallback (GtkWidget *, GdkEvent *, gpointer);

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
static GdkVisual *display = NULL;

int InsetDataGetWidth (InsetData *i)
{
  return ((GtkWidget *) i)->allocation.width;
}

int InsetDataGetHeight (InsetData *i)
{
  return ((GtkWidget *) i)->allocation.height;
}

void InsetDataDestroy (InsetData *i)
{
/*   XtDestroyWidget ((Widget) i); */
}

void InsetDataManage (InsetData *i)
{
  gtk_widget_show ((GtkWidget *) i);
}

void InsetDataUnmanage (InsetData *i)
{
  gtk_widget_hide ((GtkWidget *) i);
}

void InsetDataMove (InsetData *i, int x, int y)
{
  GtkWidget *w;

  w = (GtkWidget *) i;
  gtk_kino_move (GTK_KINO (w->parent), w, x, y);
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
 * In............: GdkVisual *: current X11 display
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
    f = addFontInfo (fontname); /* allocate new cache entry */
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
/*       XFreeFont (display, f->font); */
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
    return gdk_string_width ((GdkFont *) font, t);
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
    return ((GdkFont *) font)->ascent + ((GdkFont *) font)->descent;
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
    return ((GdkFont *) font)->ascent;
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
    return ((GdkFont *) font)->descent;
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
  if (font)
  {
    gdk_string_extents ((GdkFont *) font, text,
      lbearing, rbearing, width, ascent, descent);
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
  GtkWidget *w;

  w = gtk_button_new_with_label ("Button");
  gtk_signal_connect (GTK_OBJECT (w), "clicked", buttonCallback, parent);
  gtk_kino_put (GTK_KINO (widget), w, 0, 0);
  if (GTK_KINO (widget)->document == parent->document &&
    GTK_WIDGET_VISIBLE (widget))
  {
    gtk_widget_show (w);
  }

  return (InsetData *) w;
}

void InsetDataButtonDelete (InsetData *inset)
{
  gtk_widget_destroy (GTK_WIDGET (inset));
}

void InsetDataButtonSetValue (InsetData *inset, char *value)
{
  gtk_label_set (GTK_LABEL (GTK_BIN (inset)->child), value);
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
  GtkWidget *w;

  w = gtk_text_new (NULL, NULL);
/*   gtk_signal_connect (GTK_OBJECT (w), "focus_out_event", */
/*     textFocusOutCallback, parent); */
  gtk_text_set_editable (GTK_TEXT (w), TRUE);
  gtk_kino_put (GTK_KINO (widget), w, 0, 0);
  if (GTK_KINO (widget)->document == parent->document &&
    GTK_WIDGET_VISIBLE (widget))
  {
    gtk_widget_show (w);
  }

  return (InsetData *) w;
}

void InsetDataTextareaDelete (InsetData *inset)
{
  gtk_widget_destroy (GTK_WIDGET (inset));
}

void InsetDataTextareaSetValue (InsetData *inset, char *value)
{
}

char *InsetDataTextareaGetValue (InsetData *inset)
{
  return GTK_TEXT (inset)->text.ch;
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
  GtkWidget *w;

  w = gtk_entry_new ();
/*   gtk_signal_connect (GTK_OBJECT (w), "focus_out_event", */
/*     entryFocusOutCallback, parent); */
  gtk_kino_put (GTK_KINO (widget), w, 0, 0);
  if (GTK_KINO (widget)->document == parent->document &&
    GTK_WIDGET_VISIBLE (widget))
  {
    gtk_widget_show (w);
  }

  return (InsetData *) w;
}

void InsetDataInputDelete (InsetData *inset)
{
  gtk_widget_destroy (GTK_WIDGET (inset));
}

void InsetDataInputSetValue (InsetData *inset, char *value)
{
  gtk_entry_set_text (GTK_ENTRY (inset), value);
}

char *InsetDataInputGetValue (InsetData *inset)
{
  return gtk_entry_get_text (GTK_ENTRY (inset));
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
 * In............: GdkVisual *: current X11 display
 *                 char *: full name of the font
 * Out...........: FontInfo *: font data for the font
 * Precondition..: -
 * Postcondition.: the font is allocated and cached
 *-----------------------------------------------------------------------*/

FontInfo *addFontInfo (char *name)
{
  FontInfo *r;

  r = NULL;
  r = XkNew (FontInfo);
  r->name = XkNewString (name);
  r->next = fontCache;
  r->references = 1;
  r->font = gdk_font_load (r->name);
  if (r->font == NULL)
  {
    /* if the font does not exist replace it with a standard font */
      
    r->font = gdk_font_load ("fixed"); /*** FIXME -> cache this! ***/
  }
    
  assert (r->font != NULL);
    
  fontCache = r;

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: buttonCallback
 * Description...: 
 *                 
 * In............: GtkWidget *
 *                 gpointer 
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void buttonCallback (GtkWidget *w, gpointer data)
{
  EventHandlerInfo info;

  info.event = NULL;
  info.reason = EE_CLICK;
  info.source = (Node *) data;
  DocumentCallEventHandler (GTK_KINO (w->parent)->document, &info);
}

/*-------------------------------------------------------------------------
 * Function......: entryFocusOutCallback
 * Description...: 
 *                 
 * In............: GtkWidget *
 *                 gpointer
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void entryFocusOutCallback (GtkWidget *w, GdkEvent *event, gpointer data)
{
  ElementSetAttribute ((Element *) data, "value",
    gtk_entry_get_text (GTK_ENTRY (w)));
}

/*-------------------------------------------------------------------------
 * Function......: textFocusOutCallback
 * Description...: 
 *                 
 * In............: GtkWidget *
 *                 gpointer
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void textFocusOutCallback (GtkWidget *w, GdkEvent *event, gpointer data)
{
  ElementSetAttribute ((Element *) data, "value", GTK_TEXT (w)->text.ch);
}

