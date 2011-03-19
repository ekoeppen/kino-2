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
 * File..........: Table.h
 * Description...: Declaration of the Table structure which contains
 *                 the layout information for a HTML table (cells, rows,
 *                 columns)
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: Table.h,v 1.3 1999/03/29 14:34:26 koeppen Exp $
 *
 * $Log: Table.h,v $
 * Revision 1.3  1999/03/29 14:34:26  koeppen
 * added LGPL text
 *
 * Revision 1.2  1999/02/17 16:31:04  koeppen
 * changed from PData to DOM functions
 *
 * Revision 1.1.1.1  1998/12/21 17:16:43  koeppen
 * initial checkin
 *
 * Revision 1.4  1998/12/01 20:49:42  koeppen
 * Vorbereitung für Komponenten
 *
 * Revision 1.3  1998/03/10 13:00:05  koeppen
 * Cell mit box
 *
 * Revision 1.2  1998/01/22 13:02:48  koeppen
 * Kommentar
 *
 * Revision 1.1  1998/01/14 16:07:39  koeppen
 * Initial revision
 *-----------------------------------------------------------------------*/

#ifndef __TABLE_H
#define __TABLE_H

/* global definitions */

/* state information for table layout */

#define TABLE_LAYOUT_STARTED  0
#define TABLE_LAYOUT_FINISHED 1

/* unit types */

#define T_UNIT_PIXEL            0
#define T_UNIT_PERCENT          1

/* possible cell alignmets */

#define CELL_VALIGN_TOP       0
#define CELL_VALIGN_MIDDLE    1
#define CELL_VALIGN_BOTTOM    2

/* possible frames and rules */

#define TABLE_FRAME_VOID      0
#define TABLE_FRAME_ABOVE     1
#define TABLE_FRAME_BELOW     2
#define TABLE_FRAME_HSIDES    3
#define TABLE_FRAME_VSIDES    4
#define TABLE_FRAME_LHS       5
#define TABLE_FRAME_RHS       6
#define TABLE_FRAME_BOX       7
#define TABLE_FRAME_BORDER    8

#define TABLE_RULES_NONE      0
#define TABLE_RULES_COLS      1
#define TABLE_RULES_ROWS      2
#define TABLE_RULES_ALL       3

/* global types */

typedef struct _Cell		/* the cell structure */
{
  int minimalWidth;		/* dimensions */
  int maximalWidth;
  int height;
  int oldHeight;
  int rowSpan;			/* spans */
  int columnSpan;
  int hAlign;			/* alignments */
  int vAlign;
  int fillingCell;		/* is this a 'dummy' cell used width
				   rowspans? */
  int proposedWidth;		/* width as proposed by source text */
  int widthUnit;
  Element* box;
  struct _Cell *next;
  struct _Cell *below;
} Cell;

typedef struct _Column		/* the column structure */
{
  int minimalWidth;		/* dimensions */
  int maximalWidth;
  int width;			/* actual width */
  int proposedWidth;		/* width as proposed by source text */
  int proposedWidthUnit;
  int hAlign;			/* alignment of cells in this column */
  int vAlign;
  struct _Column *next;
} Column;

typedef struct _Row		/* the row structure */
{
  Cell *cell;			/* list of cells in this row */
  int height;			/* actual height of this row */
  int oldHeight;
  int hAlign;			/* alignment of cells in this row */
  int vAlign;
  struct _Row *next;
} Row;

typedef struct _Table		/* the table structure */
{
  Column *column;		/* list of column definitions */
  Row *row;			/* list of rows */
  int minimalWidth;		/* dimension */
  int maximalWidth;
  int proposedWidth;		/* width as proposed by source text */
  int widthUnit;
  int cellpadding;		/* padding and spacing in pixels */
  int cellspacing;
  int layoutStatus;		/* layouting state of the table */
  int frame;			/* which frames/rules to draw? */
  int rules;
  int hAlign;			/* horizontal alignment of the table */
} Table;

/* global functions */

Table *TableNew (void);
void TableDelete (Table *);
void TableAppendRow (Table *, Row *);
void TableAppendCell (Table *, Cell *);
void TableAppendColumn (Table *, Column *);
Cell *TableCellNew (void);
void TableCellDelete (Cell *);
Row *TableRowNew (void);
void TableRowDelete (Row *);
Column *TableColumnNew (void);
void TableColumnDelete (Column *);
void TablePrint (Table *t);

#endif
