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
 * File..........: TableTag.c
 * Description...: Implementation of the TableTag object which handles the
 *                 TABLE tag
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: TableTags.c,v 1.5 1999/04/14 14:32:20 koeppen Exp $
 *
 * $Log: TableTags.c,v $
 * Revision 1.5  1999/04/14 14:32:20  koeppen
 * renamed startBox and box in the parser structure, added accessor functions
 *
 * Revision 1.4  1999/03/29 14:34:27  koeppen
 * added LGPL text
 *
 * Revision 1.3  1999/02/17 16:31:05  koeppen
 * changed from PData to DOM functions
 *
 * Revision 1.2  1999/01/25 11:29:45  koeppen
 * changed #includes
 *
 * Revision 1.1.1.1  1998/12/21 17:16:43  koeppen
 * initial checkin
 *
 * Revision 1.7  1998/12/01 20:49:42  koeppen
 * Vorbereitung für Komponenten
 *
 * Revision 1.6  1998/04/28 15:19:53  koeppen
 * ...
 *
 * Revision 1.5  1998/04/28 15:19:34  koeppen
 * handleCol entfernt
 *
 * Revision 1.4  1998/04/23 14:02:56  koeppen
 * einfache Table-Behandlung
 *
 * Revision 1.3  1998/03/10 13:00:05  koeppen
 * Tables angefangen
 *
 * Revision 1.2  1998/01/22 13:02:48  koeppen
 * Kommentar
 *
 * Revision 1.1  1998/01/14 16:07:39  koeppen
 * Initial revision
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/* local types */

/* local functions */

static void handleTableStart (Parser *, TagHandlerInfo *);
static void handleTableEnd (Parser *, TagHandlerInfo *);
static void handleTR (Parser *, TagHandlerInfo *);
static void handleTD (Parser *, TagHandlerInfo *);
static Cell *cellAbove (Parser *);
static void setCellAttributes (Parser *, TagHandlerInfo *, Cell *);
static void startCell (Parser *, TagHandlerInfo *);
static void addColumns (Parser *, Cell *);
static void finishCell (Parser *);
static void startRow (Parser *, TagHandlerInfo *);
static void finishRow (Parser *);
static void pushTable (Parser *, Table *);
static void popTable (Parser *);

/*-------------------------------------------------------------------------
 * Function......: TableTagsHandler
 * Description...: Function called during parsing, handles the table tags
 *
 * In............: 
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int TableTagsHandler (Parser *parser, TagHandlerInfo *i)
{
  int r;

  r = 1;
  if (!strcmp (i->name, "TABLE")) handleTableStart (parser, i);
  else if (!strcmp (i->name, "/TABLE")) handleTableEnd (parser, i);
  else if (!strcmp (i->name, "TR")) handleTR (parser, i);
  else if (!strcmp (i->name, "TD")) handleTD (parser, i);
  else if (!strcmp (i->name, "TH")) handleTD (parser, i);
  else if (strcmp (i->name, "/TR") &&
    strcmp (i->name, "/TD") && 
    strcmp (i->name, "/TH"))
  {
    r = 0;
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: pushTable
 * Description...: Create a new table stack object and push it onto the
 *                 table stack 
 *
 * In............: TableTagPart *: the TableTag object 
 *                 Table *: the table object to be pushed
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the table object has been pushed onto the stack
 *-----------------------------------------------------------------------*/

void pushTable (Parser *parser, Table *t)
{
  TableStack *s;

  s = XkNew (TableStack);
  s->table = t;
  s->next = parser->tableStack;
  s->columnCount = 0;
  s->currentColumnCount = 0;
  s->currentColumn = t->column;
  s->cellStarted = 0;
  s->rowStarted = 0;
  parser->tableStack = s;
}

/*-------------------------------------------------------------------------
 * Function......: popTable
 * Description...: Remove the topmost element from the table stack
 *
 * In............: TableTagPart *: the TableTag object 
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the topmost element is removed
 *-----------------------------------------------------------------------*/

void popTable (Parser *parser)
{
  TableStack *s;

  s = parser->tableStack;
  parser->tableStack = parser->tableStack->next;
  XkFree ((char *) s);
}

/*-------------------------------------------------------------------------
 * Function......: handleTableStart
 * Description...: Handle the beginnig tag for a table and set the
 *                 table's attributes
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void handleTableStart (Parser *parser, TagHandlerInfo *i)
{
  Table *t;
  Element *b;

  HTMLTagsCheckOpenParagraph (parser);
  t = TableNew ();
  pushTable (parser, t);
  b = ParserStartElement (parser, i);
  b->display = DISPLAY_BLOCK;
  b->table = t;
}

/*-------------------------------------------------------------------------
 * Function......: handleTableEnd
 * Description...: Handle the ending tag and finish any cells or rows
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void handleTableEnd (Parser *parser, TagHandlerInfo *i)
{
  if (parser->tableStack != NULL)
  {
    if (parser->tableStack->cellStarted)
    {
      /* finish any unfinished cells */

      finishCell (parser);
    }
    if (parser->tableStack->rowStarted)
    {
      /* finish any unfinished rows */

      finishRow (parser);
    }

    ParserFinishElement (parser, i);
    popTable (parser);
  }
}

/*-------------------------------------------------------------------------
 * Function......: handleTR
 * Description...: Handle the row tag and start a new row
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void handleTR (Parser *parser, TagHandlerInfo *i)
{
  if (parser->tableStack != NULL)
  {
    if (parser->tableStack->cellStarted)
    {
      /* finish any unfinished cell */

      finishCell (parser);
    }
    if (parser->tableStack->rowStarted)
    {
      /* finish any unfinished row */

      finishRow (parser);
    }
    /* start the new row */

    startRow (parser, i);
  }
}

/*-------------------------------------------------------------------------
 * Function......: handleTD
 * Description...: Handle the table data tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void handleTD (Parser *parser, TagHandlerInfo *i)
{
  if (parser->tableStack != NULL)
  {
    if (parser->tableStack->cellStarted)
    {
      /* finish any unfinished cell */

      finishCell (parser);
    }
    else
    {
      parser->tableStack->cellStarted = 1;
    }

    /* start a new cell */

    startCell (parser, i);
  }
}

/*-------------------------------------------------------------------------
 * Function......: startCell
 * Description...: Start a new cell, set the cell's attributes and
 *                 insert any filling cells for rowspanning cells
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void startCell (Parser *parser, TagHandlerInfo *i)
{
  Cell *c, *a;
  int colSpan, rowSpan;
  Element *box;

  /* handle rowspanning cells above the current cell */

  a = cellAbove (parser);	/* find the cell above the current cell */
  while (a != NULL && a->rowSpan > 1)
  {
    colSpan = a->columnSpan;
    rowSpan = a->rowSpan;
    c = TableCellNew ();
    a->below = c;
    parser->tableStack->currentCell = c;
    c->columnSpan = colSpan;
    c->rowSpan = rowSpan - 1;	/* decrease the rowspan */
    c->fillingCell = 1;	/* this is not a "real" cell! */

    TableAppendCell (parser->tableStack->table, c);
    addColumns (parser, c);
    
    a = cellAbove (parser);	/* find the cell above the current cell */
  }

  /* create new cell */

  c = TableCellNew ();

  if (a != NULL)
  {
    a->below = c;
  }

  parser->tableStack->currentCell = c;
  setCellAttributes (parser, i, c);

  /* add the cell to the table */

  TableAppendCell (parser->tableStack->table, c);
  parser->tableStack->firstNode = NodeGetLastChild ((Node *) parser->element);

  box = ParserStartElement (parser, i);
  box->display = DISPLAY_BLOCK;
  box->floating = FLOAT_LEFT;
  c->box = box;
}

/*-------------------------------------------------------------------------
 * Function......: finishCell
 * Description...: Finish the current cell, set its dimensions and styles
 *
 * In............: KinoWidget: the Kino widget
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void finishCell (Parser *parser)
{
  Cell *c;
  TableStack *s;

  s = parser->tableStack;
  c = s->currentCell;

  ParserFinishElement (parser, NULL);
  addColumns (parser, c);
}

/*-------------------------------------------------------------------------
 * Function......: startRow
 * Description...: Start a new row, set the row's attributes
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void startRow (Parser *parser, TagHandlerInfo *i)
{
  Row *r;
  Element *box;

  r = TableRowNew ();
  TableAppendRow (parser->tableStack->table, r);
  parser->tableStack->rowStarted = 1;
  box = ParserStartElement (parser, i);
  box->display = DISPLAY_BLOCK;
  box->floating = FLOAT_NONE;
}

/*-------------------------------------------------------------------------
 * Function......: finishRow
 * Description...: Finish the current row
 *
 * In............: KinoWidget: the Kino widget
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void finishRow (Parser *parser)
{
  TableStack *s;

  s = parser->tableStack;
  ParserFinishElement (parser, NULL);

  s->cellStarted = 0;

  /* adjust the column data for the table */

  s->columnCount = max (s->columnCount, s->currentColumnCount);
  s->currentColumnCount = 0;
  s->currentColumn = s->table->column;
}

/*-------------------------------------------------------------------------
 * Function......: addColumn
 * Description...: Create new columns if needed
 *
 * In............: KinoWidget: the Kino widget
 *                 TableTagPart *: the TableTag object
 *                 Cell *: the current cell
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void addColumns (Parser *parser, Cell *c)
{
  TableStack *s;
  int i;

  s = parser->tableStack;

  /*  calculateCellExtent (k, e, c); XXX */
  /* calculate the cell's dimensions */

  for (i = 0; i < c->columnSpan; i++)
  {
    s->currentColumnCount++;
    if (s->currentColumnCount > s->columnCount)
    {
      /* create new columns as needed */

      s->columnCount++;
      s->currentColumn = TableColumnNew ();
      TableAppendColumn (s->table, s->currentColumn);
    }
    s->currentColumn = s->currentColumn->next;
  }
}

/*-------------------------------------------------------------------------
 * Function......: cellAbove
 * Description...: Find the cell anbove the current cell
 *
 * In............: TableTagPart *: the TableTag object
 * Out...........: Cell *: the cell above the current cell or NULL
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Cell *cellAbove (Parser *parser)
{
  Cell *c;
  Column *column;
  Row *r;
  int i;

  c = NULL;
  r = parser->tableStack->table->row;
  while (r != NULL && r->next != NULL && r->next->next != NULL)
  {
    r = r->next;
  }
  if (r != NULL && r->next != NULL)
  {
    c = r->cell;
    column = parser->tableStack->table->column;
    while (c != NULL && column != parser->tableStack->currentColumn)
    {
      for (i = 0; i < c->columnSpan - 1; i++)
      {
	column = column->next;
      }
      if (column != parser->tableStack->currentColumn)
      {
	c = c->next;
	column = column->next;
      }
    }
  }
  return c;
}

/*-------------------------------------------------------------------------
 * Function......: setCellAttributes
 * Description...: Set the attributes of the current cell
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 *                 TableTagPart *: the TableTag data
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void setCellAttributes (Parser *parser, TagHandlerInfo *i, Cell *c)
{
  int n;
  char *v;
  TableStack* s;

  s = parser->tableStack;
  n = 1;
  v = AttributeGetValue (i->attributes, "ROWSPAN");
  if (v != NULL) n = atoi (v);
  s->cellRowSpan = max (1, n);

  n = 1;
  v = AttributeGetValue (i->attributes, "COLSPAN");
  if (v != NULL) n = atoi (v);
  s->cellColSpan = max (1, n);

  c->columnSpan = s->cellColSpan;
  c->rowSpan = s->cellRowSpan;
  c->minimalWidth = s->cellDefaultWidth;
  c->maximalWidth = s->cellDefaultWidth;
}
