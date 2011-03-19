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
 * File..........: Line.c
 * Description...: Implementation of the Line structure which holds
 *                 information about the layout of displayable lines
 *                 of text
 *
 * $Id: Line.c,v 1.5 1999/07/12 12:13:16 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include "Kino.h"
#include "KinoP.h"
#include "Visual.h"

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void LineStart (Line *line, Element *b, int y)
{
  line->x = ElementLeftMargin (b, y); 
/*   line->x = ElementTotalLeft (b); */
  line->y = y;
  line->totalWidth = ElementRightMargin (b, y) - line->x; 
/*   line->totalWidth = b->width - ElementTotalRight (b) - line->x; */
  line->width = 0;
  line->height = 0;
  line->base = 0; /* StyleFontAscent (b->font); XXX */
  line->numElements = 0;
  line->started = 1;
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

void LineFinish (Line *line, Element *b)
{
  int diff;
  int i;
  Node *p;

  switch (b->text_align)
  {
    case ALIGN_CENTER:
      diff = (line->totalWidth - line->width) / 2;
      break;
    case ALIGN_RIGHT:
      diff = line->totalWidth - line->width;
    default:
      diff = 0;
      break;
  }

  for (i = 0; i < line->numElements; i++)
  {
    p = line->element[i];
    if (p->type == TEXT_NODE || p->type == INSET_NODE)
    {
      ((VisibleNode *) p)->x += diff;
      ((VisibleNode *) p)->y += line->base;
      if (p->parent != NULL && p->parent->display == DISPLAY_INLINE)
      {
	((VisibleNode *) p)->y += ElementTotalTop (p->parent);
      }
    }
    line->height = max (line->height, p->parent->line_height.effective);
  }

  diff = line->y + line->height - (b->height - ElementTotalBottom (b));

  b->height += max (0, diff);
  line->y = line->y + line->height;
  line->started = 0;
  line->height = 0;
  line->base = 0;
  line->width = 0;
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

void LineAddElement (Line *line, Element *b, Node *p)
{
  int leftWidth, rightWidth;
  VisibleNode *v;

  if (line->numElements < MAX_LINE_ELEM &&
    (p->type == INSET_NODE || p->type == ELEMENT_NODE || p->type == TEXT_NODE))
  {
    line->element[line->numElements++] = p;

    v = (VisibleNode *) p;

    leftWidth = 0;
    rightWidth = 0;
    if (p->parent->display == DISPLAY_INLINE)
    {
      if (p == p->parent->contents)
      {
	leftWidth = ElementTotalLeft (p->parent);
      }
      if (p->next == NULL)
      {
	rightWidth = ElementTotalRight (p->parent);
      }
    }

    v->y = 0;
    switch (p->type)
    {
      case TEXT_NODE:
	line->base = max (line->base, VisualFontAscent (b->font));
	break;
      case ELEMENT_NODE:
	if (((Element *) p)->display == DISPLAY_INLINE)
	{
	  line->base += ElementTotalTop ((Element *) p);
	}
	break;
      case INSET_NODE:
	if (p->parent != NULL && p->parent->vertical_align.unit == UNIT_ENUM)
	{
	  if (p->parent->vertical_align.value == VALIGN_MIDDLE)
	  { 
	    line->base = max (line->base,
	      line->base + (v->height - line->base) / 2);
	  }
	  else if (p->parent->vertical_align.value == VALIGN_BOTTOM)
	  {
	    line->base = max (line->base, v->height);
	  }
	}
	line->height = max (line->height, v->height);
	break;
      default:
	break;
    }

    v->x = line->x + line->width + leftWidth;
    v->y += line->y;
    if (p->type == INSET_NODE)
    {
      NodePositionAbsolute (p);
    }
    line->width += v->width + leftWidth + rightWidth;
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

void LineMoveRight (Line *line, int offset)
{
  Node *p;
  int i;

  line->x += offset;
  for (i = 0; i < line->numElements; i++)
  {
    p = line->element[i];
    if (p->type == TEXT_NODE || p->type == INSET_NODE)
    {
      ((VisibleNode *) p)->x += offset;
    }
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

void LineSetWidth (Line *line, int width)
{
  line->totalWidth = width;
}

