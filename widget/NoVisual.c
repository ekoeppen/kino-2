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
 * File..........: 
 * Description...: 
 *
 * $Id$
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"

int InsetDataGetWidth (InsetData *i)
{
  return 0;
}

int InsetDataGetHeight (InsetData *i)
{
  return 0;
}

void InsetDataDestroy (InsetData *i)
{
}

void InsetDataManage (InsetData *i)
{
}

void InsetDataUnmanage (InsetData *i)
{
}

void InsetDataMove (InsetData *i, int x, int y)
{
}

void VisualInit (void *w)
{
}

void VisualChangeManagedInsets (Element *box)
{
}

KinoFont *VisualLoadFont (FontFamily family,
  int size,
  FontWeight weight,
  FontSlant slant,
  int scaling)
{
  return NULL;
}

void VisualUnloadFonts (void)
{
}

int VisualTextWidth (KinoFont *font, char *t)
{
  return 0;
}

int VisualFontHeight (KinoFont *font)
{
  return 0;
}

int VisualFontAscent (KinoFont *font)
{
  return 0;
}

int VisualFontDescent (KinoFont *font)
{
  return 0;
}

void VisualTextExtents (KinoFont *font, char *text,
  int *width, int *lbearing, int *rbearing, int *ascent, int *descent)
{
}

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

