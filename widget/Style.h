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
 * File..........: Style.h
 * Description...: Declaration of the Style structure which holds
 *                 information about text styles (color, font, etc)
 *
 * $Id: Style.h,v 1.3 1999/04/14 14:34:13 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __STYLE_H
#define __STYLE_H

/* exported functions */

void ElementSetCSSDisplay (Element *);
void ElementSetCSSFont (Element *);
void ElementSetCSSProperties (Element *);
void ElementSetCSSProperty (Element *, char *, char *);
char *ElementGetCSSProperty (Element *, char *);

int StyleConvertLength (KinoFont *, Length *, char *);
int StyleConvertVAlign (KinoFont *, Length *, char *);
int StyleConvertTextAlign (Align *, char *);
void StyleConvertColor (KinoColor *, char *);
void StyleConvertFloating (Float *, char *);
void StyleConvertDisplay (DisplayStyle *, char *);

#endif
