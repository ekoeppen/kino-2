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
 * $Id: Property.h,v 1.3 1999/06/30 17:30:52 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __PROPERTY_H
#define __PROPERTY_H

/* global types */

typedef enum
{
  UNIT_ENUM,
  UNIT_PIXEL,
  UNIT_PERCENT,
  UNIT_POINT,
  UNIT_MM,
  UNIT_EX,
  UNIT_EM,
  UNIT_NONE
} Unit;

typedef struct
{
  int effective;
  float value;
  Unit unit;
} Length;

typedef struct
{
  int red;
  int green;
  int blue;
  int alpha;
} KinoColor;

typedef enum
{
  ALIGN_LEFT,
  ALIGN_RIGHT,
  ALIGN_CENTER,
  ALIGN_JUSTIFY,
  ALIGN_NONE
} Align;

typedef enum
{
  BGATTACH_FIXED,
  BGATTACH_SCROLL
} BGAttachment;

typedef struct
{
  int x;
  int y;
} BGPosition;

typedef enum
{
  BGREPEAT, 
  BGREPEAT_X,
  BGREPEAT_Y,
  BGREPEAT_NONE
} BGRepeat;

typedef enum
{
  CLEAR_NONE,
  CLEAR_LEFT,
  CLEAR_RIGHT,
  CLEAR_BOTH
} Clear;

typedef enum
{
  DISPLAY_BLOCK,
  DISPLAY_LIST_ITEM,
  DISPLAY_INLINE,
  DISPLAY_NONE
} DisplayStyle;

typedef enum
{
  FLOAT_NONE,
  FLOAT_LEFT,
  FLOAT_RIGHT
} Float;

typedef enum
{
  LINE_NONE,
  LINE_DOTTED,
  LINE_DASHED,
  LINE_SOLID,
  LINE_DOUBLE,
  LINE_GROVE,
  LINE_RIDGE,
  LINE_INSET,
  LINE_OUTSET
} LineStyle;

typedef enum
{
  LIST_INSIDE,
  LIST_OUTSIDE
} ListPosition;

typedef enum
{
  LIST_DISC,
  LIST_CIRCLE,
  LIST_SQUARE,
  LIST_DECIMAL,
  LIST_LOWER_ROMAN,
  LIST_UPPER_ROMAN,
  LIST_LOWER_ALPHA,
  LIST_UPPER_ALPHA
} ListStyle;

typedef enum
{
  TRANS_NONE,
  TRANS_UPPERCASE,
  TRANS_LOWERCASE,
  TRANS_CAPITALIZE
} Transform;

typedef enum
{
  VALIGN_BASELINE,
  VALIGN_TOP,
  VALIGN_SUB,
  VALIGN_SUPER,
  VALIGN_TEXT_TOP,
  VALIGN_MIDDLE,
  VALIGN_BOTTOM,
  VALIGN_TEXT_BOTTOM
} VAlign;

typedef enum
{
  WHITESPACE_NORMAL,
  WHITESPACE_PRE,
  WHITESPACE_NOWRAP
} Whitespace;

typedef enum
{
  FONT_FAMILY_TIMES = 0,
  FONT_FAMILY_COURIER,
  FONT_FAMILY_HELVETICA,
  FONT_FAMILY_UTOPIA,
  FONT_FAMILY_SYMBOL
} FontFamily;

typedef enum
{
 FONT_WEIGHT_MEDIUM = 0,
 FONT_WEIGHT_BOLD
} FontWeight;

typedef enum
{
 FONT_SLANT_ROMAN = 0,
 FONT_SLANT_ITALIC,
 FONT_SLANT_OBLIQUE
} FontSlant;

/* global functions */

#endif
