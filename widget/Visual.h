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
 * $Id: Visual.h,v 1.5 1999/07/12 12:13:22 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __VISUAL_H
#define __VISUAL_H

/* global types */

typedef void *VisualWidget;

/* global functions */

void VisualInit (void *);
void VisualChangeManagedInsets (Element *);
KinoFont *VisualLoadFont (FontFamily, int, FontWeight, FontSlant, int);
void VisualUnloadFonts (void);
int VisualTextWidth (KinoFont *, char *);
int VisualFontHeight (KinoFont *);
int VisualFontAscent (KinoFont *);
int VisualFontDescent (KinoFont *);
void VisualTextExtents (KinoFont *, char *,
  int *, int *, int *, int *, int *);

void VisualWidgetRelayout (VisualWidget);

int InsetDataGetWidth (InsetData *);
int InsetDataGetHeight (InsetData *);
void InsetDataDestroy (InsetData *);
void InsetDataManage (InsetData *);
void InsetDataUnmanage (InsetData *);
void InsetDataMove (InsetData *, int, int);

InsetData *InsetDataButtonNew (VisualWidget, Element *);
void InsetDataButtonDelete (InsetData *);
void InsetDataButtonSetValue (InsetData *, char *);
char *InsetDataButtonGetValue (InsetData *);

InsetData *InsetDataTextareaNew (VisualWidget, Element *);
void InsetDataTextareaDelete (InsetData *);
void InsetDataTextareaSetValue (InsetData *, char *);
char *InsetDataTextareaGetValue (InsetData *);

InsetData *InsetDataInputNew (VisualWidget, Element *);
void InsetDataInputDelete (InsetData *);
void InsetDataInputSetValue (InsetData *, char *);
char *InsetDataInputGetValue (InsetData *);

#endif
