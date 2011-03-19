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
 * File..........: Layouter.h
 * Description...: Declaration of the Layouter structure which
 *                 responsible for arranging the parsed text into lines
 *                 which can be displayed. This is the most complex object
 *                 since some parts of the layouting process need to be
 *                 two-passed, especially tables are very complex to
 *                 layout.
 *
 * $Id: Layouter.h,v 1.4 1999/04/14 14:34:04 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __LAYOUTER_H
#define __LAYOUTER_H

/* global types */

typedef struct			/* Region structure with bigger range
				   than standard X region */
{
  int x, y;
  int width, height;
} LargeRegion;

typedef struct			/* the Layouter data */
{
  Document *document;	/* current document */
  Element *box;		/* current box */
  Element *blockBox;		/* current block-level box */

  int totalLines;		/* extents of the finished layout */
  int totalColumns;
} Layouter;

/* global functions */

Layouter *LayouterNew (void);
void LayouterReset (Layouter *);
void LayouterDelete (Layouter *);
LargeRegion LayouterReformat (Layouter *);
Element *LayouterFindBox (Element *, int, int);

#endif

