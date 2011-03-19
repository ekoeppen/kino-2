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
 * File..........: Layouter.c
 * Description...: Implementation of the Layouter structure which is
 *                 responsible for arranging the parsed text into lines
 *                 which can be displayed. 
 *
 * $Id: Layouter.c,v 1.7 1999/07/12 12:13:15 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <assert.h>
#include <limits.h>
#include "KinoParser.h"
#include "Line.h"
#include "Visual.h"
#include "Style.h"
#include "Layouter.h"

/* #define DEBUG */

/* local functions */

static void reset (Element *);
static void handleWord (Layouter *, Text *);
static void handleInset (Layouter *, Inset *);
static void handleBox (Layouter *, Element *);
static void handleBoxContents (Layouter *, Node *);
static void addBoxToLine (Element *);
static void adjustParentHeight (Layouter *, Element *);
static void adjustParentLineHeight (Element *);
static void checkLineWidth (Element *, Node *);
static void calculateTableExtents (Element *);
static void calculateBoxExtent (Element *, Element *);
static Element *findBox (Element *, int, int, int, int);

/*-------------------------------------------------------------------------
 * Function......: LayouterNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Layouter *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Layouter *LayouterNew (void)
{
  Layouter *l;

  l = XkNew (Layouter);
  l->document = NULL;
  l->box = NULL;
  l->blockBox = NULL;
  l->totalLines = 1;
  l->totalColumns = 1;
  LayouterReset (l);
  return l;
}

/*-------------------------------------------------------------------------
 * Function......: LayouterReset
 * Description...: Reset the Layouter
 *
 * In............: 
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Layouter is reset
 *-----------------------------------------------------------------------*/

void LayouterReset (Layouter *layouter)
{
  layouter->totalColumns = 0;
  layouter->totalLines = 0;
  if (layouter->document != NULL)
  {
    layouter->box = layouter->document->root;
  }
  else
  {
    layouter->box = NULL;
  }
  calculateBoxExtent (layouter->box, layouter->box);
}

/*-------------------------------------------------------------------------
 * Function......: LayouterDelete
 * Description...: 
 *                 
 * In............: Layouter *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void LayouterDelete (Layouter *layouter)
{
  XkFree ((char *) layouter);
}

/*-------------------------------------------------------------------------
 * Function......: LayouterReformat
 * Description...: Reformat the remaining Node
 *
 * In............: 
 * Out...........: LargeRegion: the region the Painter has to display
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

LargeRegion LayouterReformat (Layouter *layouter)
{
  LargeRegion r = {0, 0, 0, 0};

  if (layouter->document)
  {
    LayouterReset (layouter);
    handleBox (layouter, layouter->document->root);
    r.width = layouter->totalColumns;
    r.height = layouter->totalLines;
  }
  return r;
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

void calculateBoxExtent (Element *box, Element *blockBox)
{
  Node *p;
  Element *b;
  Inset *i;
  Text *t;

  if (box != NULL)
  {
    ElementSetCSSProperties (box);
  }
  if (box != NULL && box->display != DISPLAY_NONE)
  {
    if (!box->document && box->parent != NULL) 
    {
      box->document = box->parent->document;
    }
    box->x = 0;
    box->y = 0;
    box->minWidth = ElementTotalLeft (box) + ElementTotalRight (box);
    box->maxWidth = ElementTotalLeft (box) + ElementTotalRight (box);
    box->height = ElementTotalTop (box) + ElementTotalBottom (box);
    box->positioned = 0;
    
    for (p = box->contents; p != NULL; p = p->next)
    {
      switch (p->type)
      {
	case TEXT_NODE:
	  t = (Text *) p;
	  t->x = 0;
	  t->y = 0;
	  t->width = VisualTextWidth (box->font, t->text);
	  t->height = VisualFontHeight (box->font);
	  blockBox->minWidth = max (blockBox->minWidth, t->width);
	  blockBox->maxWidth = blockBox->maxWidth + t->width;
	  break;
	case INSET_NODE:
	  i = (Inset *) p;
	  i->width = InsetDataGetWidth (i->inset);
	  i->height = InsetDataGetHeight (i->inset);
	  blockBox->minWidth = max (blockBox->minWidth, i->width);
	  blockBox->maxWidth = blockBox->maxWidth + i->width;
	  break;
	case ELEMENT_NODE:
	  b = (Element *) p;
	  if (((Element *) p)->display == DISPLAY_INLINE)
	  {
	    calculateBoxExtent (b, blockBox);
	  }
	  else
	  {
	    calculateBoxExtent (b, b);
	    blockBox->minWidth = max (blockBox->minWidth, b->minWidth);
	  }
	  break;
	case LINEBREAK_NODE:	/* XXX: reset maxWidth! */
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
}

/*-------------------------------------------------------------------------
 * Function......: handleWord
 * Description...: 
 *                 
 * In............: Layouter * 
 *                 Text *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void handleWord (Layouter *layouter, Text *word)
{
  Element *blockBox, *box;

  box = layouter->box;
  blockBox = layouter->blockBox;
  checkLineWidth (blockBox, (Node *) word);
  LineAddElement (blockBox->line, box, (Node *) word);
}

/*-------------------------------------------------------------------------
 * Function......: handleInset
 * Description...: 
 *                 
 * In............: Layouter *
 *                 Inset *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void handleInset (Layouter *layouter, Inset *inset)
{
  Element *blockBox, *box;

  box = layouter->box;
  blockBox = layouter->blockBox;
  checkLineWidth (blockBox, (Node *) inset);
  LineAddElement (blockBox->line, box, (Node *) inset);
  InsetDataMove (inset->inset, inset->x, inset->y);
}

/*-------------------------------------------------------------------------
 * Function......: handleBox
 * Description...: 
 *                 
 * In............: Layouter *
 *                 Element *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void handleBox (Layouter *layouter, Element *box)
{
  if (box != NULL && box->display != DISPLAY_NONE)
  {
    ElementCalculateWidth (box);
    if (box->parent != NULL)
    {
      checkLineWidth (box->parent, (Node *) box);
      addBoxToLine (box);
    }

    layouter->box = box;
    switch (box->display)
    {
      case DISPLAY_BLOCK:
	layouter->blockBox = box;
	if (box->line == NULL) box->line = XkNew (Line);
	LineStart (box->line, box, ElementTotalTop (box));
	break;
      case DISPLAY_INLINE:
	box->line = box->parent->line;
	break;
      default:
	break;
    }

    handleBoxContents (layouter, box->contents);

    if (box->display != DISPLAY_INLINE)
    {
      if (box->line->started)
      {
	LineFinish (box->line, box);
      }
      adjustParentHeight (layouter, box);
      if (box->parent != NULL && box->floating == FLOAT_NONE)
      {
	adjustParentLineHeight (box);
      }

      layouter->blockBox = box->parent;
    }
    else 
    {
      if (box->parent != NULL)
      {      
	adjustParentLineHeight (box);
      }
    }

    if (box->table != NULL) calculateTableExtents (box);

    layouter->box = box->parent;

    if (box->display != DISPLAY_INLINE && 
      box->floating == FLOAT_NONE && box->parent != NULL)
    {
      LineFinish (box->parent->line, box->parent);
      LineStart (box->parent->line, box->parent, box->parent->line->y);
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

void handleBoxContents (Layouter *layouter, Node *p)
{
  Element *b;
  int whitespacePending;
  int contentsSeen;

  contentsSeen = 0;
  whitespacePending = 0;
  while (p != NULL)
  {
    switch (p->type)
    {
      case TEXT_NODE:
	if (((Text *) p)->whitespace)
	{
	  if (contentsSeen) whitespacePending = 1;
	}
	else
	{
	  if (whitespacePending)
	  {
	    layouter->box->line->width +=
	      VisualTextWidth (layouter->box->font, " ");
	    whitespacePending = 0;
	  }
	  handleWord (layouter, (Text *) p);
	  contentsSeen = 1;
	}
	p = p->next;
	break;
      case ELEMENT_NODE:
	if (((Element *) p)->display != DISPLAY_NONE)
	{
	  if (whitespacePending)
	  {
	    layouter->box->line->width +=
	      VisualTextWidth (layouter->box->font, " ");
	    whitespacePending = 0;
	  }

	  handleBox (layouter, (Element *) p);
	  contentsSeen = 1;
	}

	if (((Element *) p)->display == DISPLAY_BLOCK)
	{
	  contentsSeen = 0;
	  whitespacePending = 0;
	}
	p = p->next;
	break;
      case INSET_NODE:
	if (whitespacePending)
	{
	  layouter->box->line->width +=
	    VisualTextWidth (layouter->box->font, " ");
	  whitespacePending = 0;
	}
	handleInset (layouter, (Inset *) p);
	contentsSeen = 1;
	p = p->next;
	break;
      case LINEBREAK_NODE:
	b = layouter->box;
	if (b->line->started)
	{
	  if (b->line->height == 0)
	  {
	    b->line->height = b->line_height.effective;
	  }
	  LineFinish (b->line, b);
	}
	contentsSeen = 1;
	LineStart (b->line, b, b->line->y);
	p = p->next;
	break;
      case DOCUMENT_NODE:
	assert (p->type != DOCUMENT_NODE);
	break;
      case PROCESSING_INSTRUCTION_NODE:
	p = p->next;
	break;
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
 * Function......: addBoxToLine
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void addBoxToLine (Element *box)
{
  switch (box->floating)
  {
    case FLOAT_LEFT:
      box->x = box->parent->line->x;
      box->y = box->parent->line->y;
      LineMoveRight (box->parent->line, box->width);
      LineSetWidth (box->parent->line,
	box->parent->line->totalWidth - box->width);
      break;
    case FLOAT_RIGHT:
      box->x = box->parent->line->x + box->parent->line->totalWidth
	- box->width;
      box->y = box->parent->line->y;
      LineSetWidth (box->parent->line,
	box->parent->line->totalWidth - box->width);
      break;
    case FLOAT_NONE:
      if (box->display == DISPLAY_BLOCK)
      {
	if (box->parent->line->started)
	{
	  LineFinish (box->parent->line, box->parent);
	}
	LineStart (box->parent->line, box->parent, box->parent->line->y);
	box->width = box->parent->line->totalWidth;
	LineAddElement (box->parent->line, box->parent, (Node *) box);
      }
      break;
  }
  box->positioned = True;
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

void reset (Element *box)
{
  Node *p;

  if (box != NULL)
  {
    box->positioned = False;
    box->x = 0;
    box->y = 0;
    
    for (p = box->contents; p != NULL; p = p->next)
    {
      switch (p->type)
      {
	case ELEMENT_NODE:
	  ((Element *) p)->x = 0;
	  ((Element *) p)->y = 0;
	  ((Element *) p)->height = 0;
	  ((Element *) p)->positioned = False;
	  reset ((Element *) p);
	  break;
	case INSET_NODE:
	  ((Inset *) p)->x = 0;
	  ((Inset *) p)->y = 0;
	  break;
	case TEXT_NODE:
	  ((Text *) p)->x = 0;
	  ((Text *) p)->y = 0;
	  break;
	case PROCESSING_INSTRUCTION_NODE:
	case LINEBREAK_NODE:
	case ATTRIBUTE_NODE:
	case CDATA_SECTION_NODE:
	case ENTITY_REFERENCE_NODE:
	case ENTITY_NODE:
	case COMMENT_NODE:
	case DOCUMENT_NODE:
	case DOCUMENT_TYPE_NODE:
	case DOCUMENT_FRAGMENT_NODE:
	case NOTATION_NODE:
	  break;
      }
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

void adjustParentHeight (Layouter *layouter, Element *box)
{
  int diff;

  if (box->parent != NULL)
  {
    diff = box->y + box->height - 
      (box->parent->height - ElementTotalBottom (box->parent));
    box->parent->height += max (diff, 0);
  }
  else
  {
    layouter->totalLines = box->y + box->height;
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

void adjustParentLineHeight (Element *box)
{
  box->parent->line->height = max (box->parent->line->height, box->height);
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

void checkLineWidth (Element *box, Node *element)
{
  int width;
  VisibleNode *v;

  if (element->type == ELEMENT_NODE ||
    element->type == TEXT_NODE ||
    element->type == INSET_NODE)
  {
    v = (VisibleNode *) element;
    width = v->width;
    if (v->parent->display == DISPLAY_INLINE)
    {
      if ((Node *) v == element->parent->contents)
      {
	width += ElementTotalLeft (v->parent);
      }
      if (v->next == NULL)
      {
	width += ElementTotalRight (v->parent);
      }
    }

    if ((box->width >= v->width) &&
      (v->type == INSET_NODE || 
	(v->type == TEXT_NODE && ((Text *) v)->wrap)))
    {
      while (box->line->width + v->width > box->line->totalWidth)
      {
	if (box->line->started) LineFinish (box->line, box);
	LineStart (box->line, box, box->line->y);
	box->line->height = box->line_height.effective;
      }
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

void calculateTableExtents (Element *box)
{
  Table *t;
  Cell *c, *cell;
  Column *col;
  int maxWidth;
  int x;

  t = box->table;
  x = 0;
  if (t->row != NULL)
  {
    for (c = t->row->cell, col = t->column;
	 col != NULL && c != NULL;
	 c = c->next, col = col->next)
    {
      maxWidth = 0;
      for (cell = c; cell != NULL; cell = cell->below)
      {
	if (cell->box != NULL)
	{
	  maxWidth = max (cell->box->width, maxWidth);
	}
      }

      for (cell = c; cell != NULL; cell = cell->below)
      {
	if (cell->box != NULL)
	{
	  cell->box->width = maxWidth;
	  cell->box->x = x;
	}
      }

      x += maxWidth;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: LayouterFindBox
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *LayouterFindBox (Element *box, int x, int y)
{
  return findBox (box, 0, 0, x, y);
}

/*-------------------------------------------------------------------------
 * Function......: findBox
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *findBox (Element *box, int bx, int by, int x, int y)
{
  Node *p;
  Element *r;
  VisibleNode *v;

  r = NULL;
  if (box != NULL && box->display != DISPLAY_NONE)
  {
    bx = bx + box->x;
    by = by + box->y;

    p = box->contents;
    while (p != NULL && r == NULL)
    {
      if (p->type == ELEMENT_NODE ||
	p->type == TEXT_NODE ||
	p->type == INSET_NODE)
      {
	v = (VisibleNode *) p;
	if (p->type == ELEMENT_NODE)
	{
	  if (((Element *) v)->display == DISPLAY_INLINE ||
	    (bx + v->x <= x && x <= bx + v->x + v->width &&
	      by + v->y <= y && y <= by + v->y + v->height))
	  {
	    r = findBox ((Element *) v, bx, by, x, y);
	  }
	}
	else if (p->type == INSET_NODE)
	{
	  if (p->parent != NULL &&
	    p->parent->display == DISPLAY_INLINE &&
	    v->x <= x && x <= v->x + v->width &&
	    v->y <= y && y <= v->y + v->height)
	  {
	    r = p->parent;
	  }
	}
	else if (p->parent != NULL &&
	  p->parent->display == DISPLAY_INLINE &&
	  bx + v->x <= x && x <= bx + v->x + v->width &&
	  by + v->y - VisualFontAscent (p->parent->font) <= y && 
	  y <= by + v->y - VisualFontAscent (p->parent->font) + v->height)
	{
	  r = p->parent;
	}
      }
      p = p->next;
    }
    if (r == NULL && box->display == DISPLAY_BLOCK)
    {
      r = box;
    }
  }
  return r;
}

