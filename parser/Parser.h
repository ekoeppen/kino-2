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
 * File..........: Parser.h
 * Description...: Declaration of the Parser structure which handles
 *                 the parsing of the source text and the construction
 *                 of the Node tree
 *
 * $Id: Parser.h,v 1.5 1999/04/14 14:32:19 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __PARSER_H
#define __PARSER_H

/* global types */

typedef enum
{
  HANDLE_REPLACE,
  HANDLE_EMBED,
  HANDLE_NEW,
  HANDLE_ENTITY,
  HANDLE_DOCTYPE
} HandlingType;

typedef struct _TableStack	/* the stack used by the parser for
				    the current table info */
{
  Table *table;			/* the current table */
  int rowStarted;		/* have we started a row yet? */
  int cellStarted;		/* have we started a cell yet? */
  int headerCell;		/* is this a header cell */
  int cellHAlign;		/* horizontal alignment */
  int cellVAlign;		/* vertical alignment */

  Cell *currentCell;		/* currently parsed cell */
  Column *currentColumn;	/* current column of the cell */
  int columnCount;		/* no. of columns in the table */
  int currentColumnCount;	/* no. of columns in the current row */
  int cellColSpan;		/* columnspan of the current cell */
  int cellRowSpan;		/* rowspan of the current cell*/
  int cellDefaultWidth;		/* calculated width of the cell */
  Node *firstNode;		/* first Node element in the cell */

  struct _TableStack *next;
} TableStack;

typedef struct _Parser
{
  char *text;			/* text to be parsed */
  char *current;		/* current parsing position */
  char *last;			/* position after the last successfull
				   parsing */
  char *tagStart;		/* start of the current tag */
  int lastNodeStart;		/* first character of the last Node element*/
  int wordAfterBreak;		/* have we seen a word after a line break? */
  int trailingSpace;		/* had the last word trailing whitespace? */
  int inputPending;		/* is there still more text to come? */
  int autoformat;		/* use autoformatting for PRE-Tags? */
  int inProlog;			/* are we parsing the prolog? */
  int inCData;			/* are we parsing a CDATA section? */

  Element *element;		/* current element */
  Element *startElement;	/* start element */
  Document *document;		/* current document */
  char *entityName;		/* name of the requested entity */
  char *URI;			/* URI of the requested entity */
  char *MIMEType;		/* content type of the requested entity */
  
  TableStack *tableStack;	/* tables */

  HandlingType handling;	/* what to to with the result */

  struct _Parser *parent;	/* parent parser */

  int holdCount;		/* how many running child parsers */
} Parser;

/* exported functions */

Parser *ParserNew (void);
void ParserDelete (Parser *);
Element *ParserProcessData (Parser *, char *, char *);
Element *ParserParseText (Parser *);
void ParserParseStylesheet (Parser *);
void ParserHold (Parser *);
void ParserRelease (Parser *);

void ParserSetStartElement (Parser *, Element *);
void ParserSetDocument (Parser *, Document *);
void ParserSetURI (Parser *, char *);
char *ParserGetURI (Parser *);
Element *ParserGetElement (Parser *);
Parser *ParserGetParent (Parser *);

Element *ParserStartElement (Parser *, TagHandlerInfo *);
Element *ParserFinishElement (Parser *, TagHandlerInfo *);
Element *ParserFinishInline (Parser *);

Text *ParserAddWord (Parser *, char *, int);
Inset *ParserAddInset (Parser *, InsetData *);

void ParserChangeCurrentText (Parser *, char *, int);

#endif
