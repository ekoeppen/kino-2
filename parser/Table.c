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
 * File..........: Table.c
 * Description...: Implementation of the Table structure which contains
 *                 the layout information for a HTML table (cells, rows,
 *                 columns)
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: Table.c,v 1.3 1999/03/29 14:34:25 koeppen Exp $
 *
 * $Log: Table.c,v $
 * Revision 1.3  1999/03/29 14:34:25  koeppen
 * added LGPL text
 *
 * Revision 1.2  1999/01/25 11:29:44  koeppen
 * changed #includes
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

#include "KinoParser.h"

/* local definitions */

#define STD_WIDTH 150		/* initial width of a table column */

/*-------------------------------------------------------------------------
 * Function......: TableNew
 * Description...: Allocate a new object and reset its data fields
 *
 * In............: -
 * Out...........: Table *: the allocated Table object
 * Precondition..: -
 * Postcondition.: the object is allocated and the data fields are reset
 *-----------------------------------------------------------------------*/

Table *TableNew (void)
{
  Table *t;

  t = XkNew (Table);
  t->row = NULL;
  t->column = NULL;
  t->minimalWidth = 0;
  t->maximalWidth = 0;
  t->cellpadding = 0;
  t->cellspacing = 0;
  t->layoutStatus = TABLE_LAYOUT_STARTED;
  t->frame = TABLE_FRAME_VOID;
  t->rules = TABLE_RULES_NONE;
  return t;
}

/*-------------------------------------------------------------------------
 * Function......: TableDelete
 * Description...: Delete a Table object
 *
 * In............: Table *: the Table object to be deleted
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Table object is deleted and the memory is released
 *-----------------------------------------------------------------------*/

void TableDelete (Table *t)
{
  Row *r;
  Column *c;

  /* delete all rows */
  
  while (t->row != NULL)
  {
    r = t->row;
    t->row = t->row->next;
    TableRowDelete (r);
  }

  /* delete all columns */

  while (t->column != NULL)
  {
    c = t->column;
    t->column = t->column->next;
    TableColumnDelete (c);
  }
  XkFree ((char *) t);
}

/*-------------------------------------------------------------------------
 * Function......: TableAppendRow
 * Description...: Append a row to a table's row list
 *
 * In............: Table *: the table
 *                 Row *: the row to append
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the row is appended to the table's row list
 *-----------------------------------------------------------------------*/

void TableAppendRow (Table *t, Row *r)
{
  Row *l;

  l = t->row;
  if (l != NULL)
  {
    while (l->next != NULL)
    {
      l = l->next;
    }
    r->next = NULL;
    l->next = r;
  }
  else
  {
    t->row = r;
  }
}

/*-------------------------------------------------------------------------
 * Function......: TableAppendCell
 * Description...: Append a cell to the last row in a table
 *
 * In............: Table *: the table containing the row list
 *                 Cell *: the cell to append
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the cell is appended to the last row in a table
 *-----------------------------------------------------------------------*/

void TableAppendCell (Table *t, Cell *c)
{
  Cell *l;
  Row *r;

  r = t->row;
  while (r != NULL && r->next != NULL)
  {
    r = r->next;
  }
  if (r != NULL)
  {
    l = r->cell;
    if (l != NULL)
    {
      while (l->next != NULL)
      {
	l = l->next;
      }
      c->next = NULL;
      l->next = c;
    }
    else
    {
      r->cell = c;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: TableAppendColumn
 * Description...: Append a column the end of a table's column list
 *
 * In............: Table *: the table containing the column list
 *                 Column *: the column to append
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the column is appended at the end of the column list
 *-----------------------------------------------------------------------*/

void TableAppendColumn (Table *t, Column *c)
{
  Column *l;

  l = t->column;
  if (l != NULL)
  {
    while (l->next != NULL)
    {
      l = l->next;
    }
    c->next = NULL;
    l->next = c;
  }
  else
  {
    t->column = c;
  }
}

/*-------------------------------------------------------------------------
 * Function......: TableCellNew
 * Description...: Allocate a new Cell object and reset its data fields
 *
 * In............: -
 * Out...........: Cell *: the allocated Cell
 * Precondition..: -
 * Postcondition.: the object is allocated and the data fields are reset
 *-----------------------------------------------------------------------*/

Cell *TableCellNew (void)
{
  Cell *c;
  
  c = XkNew (Cell);
  c->next = NULL;
  c->below = NULL;
  c->height = 0;
  c->oldHeight = 0;
  c->minimalWidth = 0;
  c->maximalWidth = 0;
  c->columnSpan = 1;
  c->rowSpan = 1;
  c->hAlign = ALIGN_NONE;
  c->vAlign = ALIGN_NONE;
  c->fillingCell = 0;
  c->proposedWidth = 0;
  c->widthUnit = UNIT_PIXEL;
  c->box = NULL;
  return c;
}

/*-------------------------------------------------------------------------
 * Function......: TableCellDelete
 * Description...: Delete a Cell object
 *
 * In............: Cell *: the Cell object to be deleted
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Cell object is deleted and the memory is released
 *-----------------------------------------------------------------------*/

void TableCellDelete (Cell *c)
{
  XkFree ((char *) c);
}

/*-------------------------------------------------------------------------
 * Function......: TableRowNew
 * Description...: Allocate a new Row object and reset its data fields
 *
 * In............: -
 * Out...........: Row *: the allocated row
 * Precondition..: -
 * Postcondition.: the object is allocated and the data fields are reset
 *-----------------------------------------------------------------------*/

Row *TableRowNew (void)
{
  Row *r;

  r = XkNew (Row);
  r->cell = NULL;
  r->next = NULL;
  r->height = 0;
  r->oldHeight = 0;
  r->hAlign = ALIGN_NONE;
  r->vAlign = ALIGN_NONE;
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: TableRowDelete
 * Description...: Delete a Row object
 *
 * In............: Row *: the Row object to be deleted
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Row object is deleted and the memory is released
 *-----------------------------------------------------------------------*/

void TableRowDelete (Row *r)
{
  Cell *c;
  while (r->cell != NULL)
  {
    c = r->cell;
    r->cell = r->cell->next;
    TableCellDelete (c);
  }
  XkFree ((char *) r);
}

/*-------------------------------------------------------------------------
 * Function......: TableColumnNew
 * Description...: Allocate a new Column object and reset its data fields
 *
 * In............: -
 * Out...........: Column *: the allocated column
 * Precondition..: -
 * Postcondition.: the object is allocated and the data fields are reset
 *-----------------------------------------------------------------------*/

Column *TableColumnNew (void)
{
  Column *c;

  c = XkNew (Column);
  c->next = NULL;
  c->minimalWidth = 0;
  c->maximalWidth = 0;
  c->width = STD_WIDTH;
  c->proposedWidth = 0;
  c->proposedWidthUnit = UNIT_PIXEL;
  c->hAlign = ALIGN_NONE;
  c->vAlign = ALIGN_NONE;
  return c;
}

/*-------------------------------------------------------------------------
 * Function......: TableColumnDelete
 * Description...: Delete a Column object
 *
 * In............: Column *: the Column object to be deleted
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Column object is deleted and the memory is released
 *-----------------------------------------------------------------------*/

void TableColumnDelete (Column *c)
{
  XkFree ((char *) c);
}

/*-------------------------------------------------------------------------
 * Function......: TablePrint
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void TablePrint (Table *t)
{
  Column *col;
  Row *row;
  Cell *c;
  
  printf ("Columns: ");
  for (col = t->column; col != NULL; col = col->next)
  {
    printf ("<width (min/max/current): %d/%d/%d>  ",
      col->minimalWidth, col->maximalWidth, col->width);
  }
  printf ("\n");
  for (row = t->row; row != NULL; row = row->next)
  {
    printf ("Row: <height: %d> \n  Cells:\n", row->height);
    for (c = row->cell; c != NULL; c = c->next)
    {
      printf ("    <%p> below: %p w: %d/%d h: %d cs: %d rs: %d b: %p\n",
	c, c->below,
	c->minimalWidth, c->maximalWidth, c->height,
	c->columnSpan, c->rowSpan, c->box);
    }
    printf ("\n");
  }
  printf ("\n");
}

  

