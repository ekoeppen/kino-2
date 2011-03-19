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
 * File..........: Parser.c
 * Description...: Implementation of the Parser structure which handles
 *                 the parsing of the source text and the construction
 *                 of the Node list
 *
 * $Id: Parser.c,v 1.13 1999/09/07 16:18:13 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/* local definitions */

/* #define DEBUG */

#define current(parser)   (*parser->current)
#define nextChar(parser) parser->current++
#define MAX_WORD     512

/* local functions */

static int parseWord (Parser *);
static int parsePreformattedWord (Parser *);
static void expandEntity (Parser *);
void addEntity (Parser *);
static int parseMarkup (Parser *);
static int parseTag (Parser *);
static void parseProcessingInstruction (Parser *);
static void parseComment (Parser *);
static void parseCData (Parser *);
static void parseDoctype (Parser *);
static void parseInternalSubset (Parser *);
static void parseMarkupDeclaration (Parser *);
static void parseElementDeclaration (Parser *);
static void parseEntityDeclaration (Parser *);
static void parseAttributeDeclaration (Parser *);
static void parseToken (Parser *, char *, char *);
static void parseCSToken (Parser *, char *, char *);
static void parseAtKeyword (Parser *);
static void parseCSSComment (Parser *);
static void parseSelector (Parser *, char *);
static void parseDeclaration (Parser *, char *);
static void skipWhitespace (Parser *, int);
static Attribute *parseTagAttributes (Parser *);
static void parseStringLiteral (Parser *, char *);
static void callTagHandlers (Parser *, TagHandlerInfo *);
void resumeProcessing (Parser *);

/*-------------------------------------------------------------------------
 * Function......: ParserNew
 * Description...: Allocates a new parser structure
 *
 * In............: KinoWidget: the associated Kino widget
 * Out...........: Parser *: the parser structure
 * Precondition..: -
 * Postcondition.: the parser is allocated and reset
 *-----------------------------------------------------------------------*/

Parser *ParserNew (void)
{
  Parser *parser;

  parser = XkNew (Parser);
  parser->current = NULL;
  parser->wordAfterBreak = 0;
  parser->trailingSpace = 1;
  parser->lastNodeStart = 0;
  parser->element = NULL;
  parser->startElement = NULL;
  parser->tagStart = NULL;
  parser->autoformat = 1;

  parser->inputPending = 0;
  parser->tableStack = NULL;
  parser->document = NULL;
  parser->entityName = NULL;
  parser->URI = NULL;
  parser->MIMEType = NULL;
  parser->parent = NULL;
  parser->holdCount = 0;
  parser->text = NULL;
  parser->handling = HANDLE_EMBED;
  parser->inProlog = 0;
  parser->inCData = 0;

#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

  return parser;
}

/*-------------------------------------------------------------------------
 * Function......: ParserDelete
 * Description...: Free all memory taken by the parser
 *
 * In............: Parser *: the parser structure
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the parser is deleted
 *-----------------------------------------------------------------------*/

void ParserDelete (Parser *parser)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

 if (parser != NULL)
 {
   if (parser->URI) XkFree (parser->URI);
   if (parser->entityName) XkFree (parser->entityName);
   if (parser->MIMEType) XkFree (parser->MIMEType);
   if (parser->text) XkFree (parser->text);
   XkFree ((char *) parser);
 }
}

/*-------------------------------------------------------------------------
 * Function......: ParserSetStartElement
 * Description...: 
 *                 
 * In............: Parser *
 *                 Element *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ParserSetStartElement (Parser *parser, Element *element)
{
  parser->startElement = element;
}

/*-------------------------------------------------------------------------
 * Function......: ParserSetDocument
 * Description...: 
 *                 
 * In............: Parser *
 *                 Document *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ParserSetDocument (Parser *parser, Document *d)
{
  parser->document = d;
}

/*-------------------------------------------------------------------------
 * Function......: ParserGetElement
 * Description...: 
 *                 
 * In............: Parser *
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *ParserGetElement (Parser *parser)
{
  return parser->element;
}

/*-------------------------------------------------------------------------
 * Function......: ParserGetParent
 * Description...: 
 *                 
 * In............: Parser *
 * Out...........: Parser *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Parser *ParserGetParent (Parser *parser)
{
  return parser->parent;
}

/*-------------------------------------------------------------------------
 * Function......: ParserSetURI
 * Description...: 
 *                 
 * In............: Parser *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ParserSetURI (Parser *parser, char *uri)
{
  if (parser->URI != NULL) XkFree (parser->URI);
  parser->URI = XkNewString (uri);
}

char *ParserGetURI (Parser *parser)
{
  return parser->URI;
}

/*-------------------------------------------------------------------------
 * Function......: ParserProcessData
 * Description...: Process the given data with the given content type
 *                 
 * In............: Parser *:
 *                 char *:
 *                 char *:
 * Out...........: Element *:
 * Precondition..: The Parser structure must have been initialized
 *                 (startElement and document must have reasonable values)
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *ParserProcessData (Parser *parser, char *MIMEType, char *data)
{
#ifdef DEBUG
  fprintf (stderr, ">>> %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

  assert (parser->startElement != NULL);
  assert (parser->document != NULL);

  parser->text = XkNewString (data);
  parser->current = parser->text;
  parser->MIMEType = XkNewString (MIMEType);
  parser->document->MIMEType = XkNewString (MIMEType);
  parser->element = parser->startElement;
  if (XkStrEqual (MIMEType, "text/html"))
  {
    parser->document->useHTMLStyles = 1;
  }

  ParserHold (parser);

  resumeProcessing (parser);

  /*  ParserDelete (parser); */

#ifdef DEBUG
  fprintf (stderr, "<<< %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

  return parser->startElement;
}

/*-------------------------------------------------------------------------
 * Function......: ParserHold
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ParserHold (Parser *parser)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

  parser->holdCount++;
}

/*-------------------------------------------------------------------------
 * Function......: ParserRelease
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ParserRelease (Parser *parser)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

  if (parser->holdCount > 0)
  {
    parser->holdCount--;
  }
  if (parser->holdCount == 1)
  {
    resumeProcessing (parser);
  }
}

/*-------------------------------------------------------------------------
 * Function......: resumeProcessing
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void resumeProcessing (Parser *parser)
{
  Node *p;
  Element *delimiter;

#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, parser);
#endif

  switch (parser->handling)
  {
    case HANDLE_ENTITY:
      addEntity (parser);
      ParserRelease (parser);
      break;
    case HANDLE_REPLACE:
    case HANDLE_EMBED:
      if (XkStrEqual (parser->MIMEType, "text/css"))
      {
	ParserParseStylesheet (parser);
      }
      else
      {
	ParserParseText (parser);
      }
      break;
    case HANDLE_NEW:
      break;
    case HANDLE_DOCTYPE:
      parseInternalSubset (parser);
      ParserRelease (parser);
      break;
  }

  if (parser->holdCount == 0) 
  {
    /* we are finished with handling the text */

    if (parser->parent != NULL)
    {
      /* release the parent parser (may trigger a resumeProcessing) */

      ParserRelease (parser->parent);
    }

    if (parser->handling == HANDLE_EMBED)
    {
      /* move the resulting Node one level upwards */

      p = parser->startElement->contents;

      if (p != NULL && parser->startElement->parent != NULL)
      {
	parser->startElement->contents = NULL;
	delimiter = ElementNew ();
	delimiter->anonymous = ANON_DELIMITER;
	delimiter->display = DISPLAY_NONE;
	NodeInsertBefore ((Node *) parser->startElement->parent,
	  (Node *) parser->startElement, (Node *) delimiter);
	
	NodeInsertBefore ((Node *) parser->startElement->parent,
	  (Node *) parser->startElement, p);

	delimiter = ElementNew ();
	delimiter->display = DISPLAY_NONE;
	delimiter->anonymous = ANON_DELIMITER;
	NodeInsertBefore ((Node *) parser->startElement->parent,
	  (Node *) parser->startElement, (Node *) delimiter);
      }
      else
      {
	/* insert the element at the top level */
      }
    }

#ifdef FIXME
    if ((parser->handling == HANDLE_REPLACE ||
      parser->handling == HANDLE_EMBED) && parser->kino != NULL)
    {
      KinoRelayout ((Widget) parser->kino);
    }
#endif

  }
}

/*-------------------------------------------------------------------------
 * Function......: ParserParseText
 * Description...: Parse any text from the current parsing position on
 *                 until no more text is available
 *
 * In............: Parser *: the parser structure
 * Out...........: -
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: the text has been parsed
 *-----------------------------------------------------------------------*/

Element *ParserParseText (Parser *parser)
{
  int finished;
  int length;

  finished = 0;
  if (parser->current != NULL)
  {
    if (!finished && parser->holdCount == 1 &&
      parser->current == parser->text &&
      parser->element != NULL && parser->element->contents == NULL)
    {
      skipWhitespace (parser, 1);
    }
    while (!finished && parser->holdCount == 1)
    {
      if (parser->inProlog)
      {
	skipWhitespace (parser, 0);
	parseInternalSubset (parser);
	while (current (parser) != '\0' && current (parser) != '>')
	{
	  nextChar (parser);
	}
	if (current (parser) != '\0') nextChar (parser);
	parser->inProlog = 0;
	length = parser->current - parser->text -
	  parser->document->subsetStart;
	parser->document->internalSubset = malloc (length + 1);
	memcpy (parser->document->internalSubset,
	  parser->text + parser->document->subsetStart, length);
	parser->document->internalSubset[length] = '\0';
      }
      else
      {
	switch (current (parser))
	{
	  case '\0':
	    finished = 1;
	    break;
	  case '<':
	    finished = parseMarkup (parser);
	    break;
	  default:
	    if (parser->autoformat)
	    {
	      finished = parseWord (parser);
	    }
	    else
	    {
	      finished = parsePreformattedWord (parser);
	    }
	    break;
	}
      }
    }
  }
  if (finished)
  {
    ParserRelease (parser);
  }

  return parser->startElement;
}

/*-------------------------------------------------------------------------
 * Function......: ParserParseStylesheet
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ParserParseStylesheet (Parser *parser)
{
  RuleSet *r, *rules, *firstRule;
  char selector[MAX_NAME_LENGTH + 1];
  char declaration[MAX_WORD + 1];

  rules = parser->element->document->cssRules;
  while (current (parser) != '\0')
  {
    skipWhitespace (parser, 0);
    if (current (parser) == '@')
    {
      parseAtKeyword (parser);
    }
    else if (current (parser) == '/')
    {
      parseCSSComment (parser);
    }
    else
    {
      firstRule = rules;
      while (current (parser) != '\0' && current (parser) != '{')
      {
	skipWhitespace (parser, 0);
	selector[0] = '\0';
	parseSelector (parser, selector);
	if (strlen (selector) > 0
	  && (current (parser) == '{' || current (parser) == ','))
	{
	  r = XkNew (RuleSet);
	  r->selector = XkNewString (selector);
	  r->declaration = NULL;
	  r->next = rules;
	  rules = r;
	  if (current (parser) == ',') nextChar (parser);
	}
	else
	{
	  while (current (parser) != '\0' && current (parser) != '{')
	  {
	    nextChar (parser);
	  }
	}
      }

      while (current (parser) != '\0' && current (parser) != '}')
      {
	declaration[0] = '\0';
	parseDeclaration (parser, declaration);
	if (strlen (declaration))
	{
	  for (r = rules; r != firstRule; r = r->next)
	  {
	    r->declaration = XkNewString (declaration);
	  }
	}
      }

      if (current (parser) == '}') nextChar (parser);
    }
  }
  ParserRelease (parser);
  
  parser->element->document->cssRules = rules;
}

/*-------------------------------------------------------------------------
 * Function......: ParserStartElement
 * Description...: 
 *                 
 * In............: Parser *:
 *                 LinkHandlerInfo *:
 * Out...........: Element *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *ParserStartElement (Parser *parser, TagHandlerInfo *info)
{
  Element *b;

  b = ElementNew ();
  if (info != NULL)
  {
    b->tag = XkNewString (info->name);
    if (info->namespace) b->namespace = XkNewString (info->namespace);
    b->attributes = AttributeDuplicate (info->attributes);
    b->empty = info->empty;
  }
  b->source = parser->current - parser->text;
  b->document = parser->document;
  ElementSetDefaultAttributes (b);

  StyleBaseSetBoxDisplay (b);
  if (b->display == DISPLAY_BLOCK && b->document->useHTMLStyles)
  {
    HTMLTagsCheckOpenParagraph (parser);
  }

  b->parent = parser->element;

  if (b->parent != NULL)
  {
    NodeAppendChild ((Node *) b->parent, (Node *) b);
  }

  parser->element = b;

  return b;
}

/*-------------------------------------------------------------------------
 * Function......: ParserFinishElement
 * Description...: 
 *                 
 * In............: Parser *:
 *                 LinkHandlerInfo *:
 * Out...........: Element *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *ParserFinishElement (Parser *parser, TagHandlerInfo *info)
{
  Element *box, *r;
  char *tag;
  int length;

  box = parser->element;
  r = box;
  assert (box != NULL);

  if (box->saveRawContents && box->source != 0)
  {
    length = parser->tagStart - box->source - parser->text;
    box->rawContents = malloc (length + 1);
    memcpy (box->rawContents, &parser->text[box->source], length);
    box->rawContents[length] = '\0';
  }

  if (info != NULL)
  {
    tag = info->name;
    if (tag[0] == '/') tag++;
    while (box != NULL && !XkStrEqual (box->tag, tag))
    {
      box = box->parent;
    }

    if (box != NULL && XkStrEqual (box->tag, tag))
    {
      parser->element = box->parent;

      if (box->parent != NULL)
      {
	box->parent->minWidth = max (box->minWidth, box->parent->minWidth);
	/* FIXME  if (box->display == DISPLAY_INLINE)
	{
	  box->parent->maxWidth += box->maxWidth;
	}*/
      }
    }
  }
  else
  {
    parser->element = box->parent;
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: ParserFinishInline
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: Element *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *ParserFinishInline (Parser *parser)
{
  Element *r;

  r = parser->element;
  while (parser->element != NULL &&
    parser->element->display == DISPLAY_INLINE)
  {
    parser->element = parser->element->parent;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: ParserAddWord
 * Description...: Add a text element to the Node list
 *
 * In............: 
 *                 char *: text for the text element
 *                 int: is this word wrappable?
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the element has been added to the list
 *-----------------------------------------------------------------------*/

Text *ParserAddWord (Parser *parser, char *t, int wrap)
{
  Text *p;
  Element *b;
 
  /* Add words only if the content of the current tag is not to be */
  /* skipped */

  p = NULL;
  b = parser->element;
  if (b != NULL && !b->saveRawContents)
  {
    p = TextNew ();

    p->parent = b;
    p->wrap = wrap;
    
    /* Allocate a new copy of the text and assign it to the Node */
    /* element */
    
    p->text = XkNewString (t);
    parser->wordAfterBreak = 1;
    p->source = parser->lastNodeStart;
    parser->lastNodeStart = parser->current - parser->text;

    NodeAppendChild ((Node *) b, (Node *) p);
  }
  return p;
}

/*-------------------------------------------------------------------------
 * Function......: ParserAddInset
 * Description...: Add an inset to the Node list
 *
 * In............: 
 *                 InsetData: the inset
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the element has been added to the list
 *-----------------------------------------------------------------------*/

Inset *ParserAddInset (Parser *parser, InsetData *inset)
{
  Inset *p;
  Element *b;
 
  /* Add elements only if the content of the current tag is not to be */
  /* skipped */

  p = NULL;
  b = parser->element;
  if (b != NULL && !b->saveRawContents)
  {
    p = InsetNew ();

    p->parent = b;
    p->inset = inset;
    
    p->source = parser->lastNodeStart;
    parser->lastNodeStart = parser->current - parser->text;

    NodeAppendChild ((Node *) b, (Node *) p);
    b->minWidth = max (b->minWidth, p->width);
    b->maxWidth += p->width;
  }
  return p;
}

/*-------------------------------------------------------------------------
 * Function......: ParserChangeCurrentText
 * Description...: change the text at the current position
 *
 * In............: Parser *: Parser containing the text
 *                 char *: the new text
 *                 int: number of characters to replace, or zero for
 *                 simply inserting the text
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void ParserChangeCurrentText (Parser *parser, char *s, int len)
{
  int textLength, newTextLength;
  int current;

  current = parser->current - parser->text;
  textLength = strlen (parser->text);

  /* allocate more memory */

  newTextLength = textLength + strlen (s) - len;
  parser->text = XkRealloc (parser->text, newTextLength + 1);
  parser->current = parser->text + current;

  /* move following text */

  memmove (parser->current + strlen (s),
    parser->current + len,
    newTextLength - current - len);

  /* insert the new text */

  memcpy (parser->current, s, strlen (s));

  parser->text[newTextLength] = '\0';
}

/*-------------------------------------------------------------------------
 * Function......: parseWord
 * Description...: Parse a word, skipping leading and trailing
 *                 whitespace, and add the word to the pdata list. If
 *                 the word was not terminated correctly (i.e. if
 *                 there was still pending input and the end of the
 *                 source has been reached), the current parsing
 *                 position is restored
 *
 * In............: Parser *: the parser structure
 * Out...........: int: 0, if the word was parsed completely;
 *                 1 if the word was not terminated correctly
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: any recognized word has been added to the pdata
 *                 list, or the current parsing position has been retained
 *-----------------------------------------------------------------------*/

int parseWord (Parser *parser)
{
  char word[MAX_WORD + 1];	/* words should not be longer than 512
				   characters! */ 
  char *s;
  int r;

  /* remember current position */

  parser->last = parser->current;
  r = 1;
  s = word;

  skipWhitespace (parser, 1);
  parser->last = parser->current;

  /* if we have not reached the end of the source or if there will */
  /* be no more input, parse the word itself */

  if (current (parser) != '\0' || !parser->inputPending)
  {
    /* words consist of anything but whitespace or a beginning tag */
    /* delimiter. Check also if we reach the end of the source */

    while (!isspace (current (parser)) && current (parser) != '<' && 
      current (parser) != '\0')
    {
      if (current (parser) != '&')
      {
	*s = current (parser);
	s++;
	nextChar (parser);
      }
      else
      {
	expandEntity (parser);
	if (current (parser) == '<' || current (parser) == '&')
	{
	  *s = current (parser);
	  if (s - word < MAX_WORD) s++;
	  nextChar (parser);
	}
      }
    }

    /* skip trailing whitespace if there is still text or no more */
    /* input is pending */

    parser->trailingSpace = 0;
    if (current (parser) != '\0' || !parser->inputPending)
    {
      /* add the word to the pdata list only if we have not reached */
      /* the end of the text or if no more input is pending */
    
      if (current (parser) != '\0' || !parser->inputPending)
      {
	*s = '\0';
	if (s > word)
	{
	  /* only add words with contents */

	  ParserAddWord (parser, word, 1);
	}
	r = 0;
      }

      skipWhitespace (parser, 1);
    }
  }
  if (r == 1)
  {
    /* if the end of the text has been reached but there will be */
    /* more input later, restore the parsing position and try later */

    parser->current = parser->last;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: parsePreformattedWord
 * Description...: Parse a word in preformatted text (i.e. don't treat
 *                 whitespace differently but insert linebreaks if
 *                 necessary), and add the word to the pdata list. If
 *                 word was not terminated correctly  (i.e. if was
 *                 still pending input and the end of the has been
 *                 reached), the current parsing position is restored
 *
 * In............: Parser *: the parser structure
 * Out...........: int: 0, if the word was parsed completely;
 *                 1 if the word was not terminated correctly
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: any recognized word has been added to the pdata
 *                 list, or the current parsing position has been retained
 *-----------------------------------------------------------------------*/

int parsePreformattedWord (Parser *parser)
{
  char word[MAX_WORD + 1];	/* words should not be longer than 512
				   characters! */ 
  char *s;
  int r;
  Node *p;

  /* remember current position */

  parser->last = parser->current;
  r = 1;
  s = word;

  /* treat anything but linefeeds and starting tag delimiters as */
  /* part of the word */

  while (current (parser) != '<' && current (parser) != '\0' 
    && current (parser) != '\n')
  {
    if (current (parser) != '&')
    {
      *s = current (parser);
      if (s - word < MAX_WORD) s++;
      nextChar (parser);
    }
    else
    {
      expandEntity (parser);
      if (current (parser) == '<' || current (parser) == '&')
      {
	*s = current (parser);
	if (s - word < MAX_WORD) s++;
	nextChar (parser);
      }
    }
  }

  /* add the word to the pdata list only if we have not reached */
  /* the end of the text or if no more input is pending */
    
  if (current (parser) != '\0' || !parser->inputPending)
  {
    *s = '\0';
    if (s > word) ParserAddWord (parser, word, 0);
    if (current (parser) == '\n')
    {
      /* add a linebreak */

      nextChar (parser);
      p = NodeNew ();
      p->type = LINEBREAK_NODE;
      p->parent = parser->element;
      NodeAppendChild ((Node *) parser->element, p);
    }
    r = 0;
  }
  if (r == 1)
  {
    /* if the end of the text has been reached but there will be */
    /* more input later, restore the parsing position and try later */

    parser->current = parser->last;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: expandEntity
 * Description...: Parse the entity at the current parsing
 *                 position, replacing it with its value
 *
 * In............: Parser *: the parser structure
 * Out...........: -
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: the current entity has been expanded
 *-----------------------------------------------------------------------*/

void expandEntity (Parser *parser)
{
  char n[MAX_NAME_LENGTH + 1];
  char *last;
  int length;
  char *value;

  last = parser->current;

  parseCSToken (parser, " \r\n\t;", n);
  length = parser->current - last;
  if (current (parser) == ';') length++;
  parser->current = last;

  value = EntityLookup (parser->document->entities, n);
  if (value)
  {
    ParserChangeCurrentText (parser, value, length);
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseMarkup
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: int :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int parseMarkup (Parser *parser)
{
  /* skip the tag delimiter */

  if (current (parser) == '<') nextChar (parser);
  switch (current (parser))
  {
    case '!': parseMarkupDeclaration (parser); break;
    case '?': parseProcessingInstruction (parser); break;
    default: parseTag (parser); break;
  }

  return 0;
}

/*-------------------------------------------------------------------------
 * Function......: parse...
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseProcessingInstruction (Parser *parser)
{
  char t[MAX_NAME_LENGTH + 1];
  ProcessingInstruction *pi;
  char *dataStart;
  Parser *p;
  LinkHandlerInfo *linkInfo;

  pi = ProcessingInstructionNew ();
  parser->tagStart = parser->current - 1;
  nextChar (parser);
  do
  {
    parseCSToken (parser, " ?\t\n\r>", t);
    pi->target = XkNewString (t);
    skipWhitespace (parser, 0);
    dataStart = parser->current;
    pi->attributes = parseTagAttributes (parser);
    if (current (parser) == '?') 
    {
      pi->data = malloc (parser->current - dataStart + 1);
      memcpy (pi->data, dataStart, parser->current - dataStart);
      pi->data[parser->current - dataStart] = '\0';
      nextChar (parser);
    }
  }
  while (current (parser) != '\0' && current (parser) != '>');
  if (current (parser) != '\0') nextChar (parser);

  NodeAppendChild ((Node *) parser->element, (Node *) pi);

  if (XkStrEqual (pi->target, "xml-stylesheet") &&
    parser->document->linkHandler)
  {
    p = ParserNew ();
    p->parent = parser;
    p->URI = XkNewString (AttributeGetValue (pi->attributes, "href"));
    p->handling = HANDLE_EMBED;
    p->document = parser->document;
    p->startElement = parser->document->root;
    ParserHold (p->parent);
    linkInfo = LinkHandlerInfoNew ();
    linkInfo->parser = p;
    linkInfo->element = parser->document->root;
    DocumentCallLinkHandler (parser->document, linkInfo);
    LinkHandlerInfoDelete (linkInfo);
  }
}

void parseMarkupDeclaration (Parser *parser)
{
  char token[8];

  parser->tagStart = parser->current - 1;
  nextChar (parser);
  strncpy (token, parser->current, 7);
  token[7] = '\0';
  if (!strncmp (parser->current, "--", 2))
  {
    parser->current += 2;
    parseComment (parser);
  }
  else if (XkStrEqual (token, "[CDATA["))
  {
    parser->current += 7;
    parseCData (parser);
  }
  else if (XkStrEqual (token, "DOCTYPE"))
  {
    parser->current += 7;
    parseDoctype (parser);
  }
}

void parseComment (Parser *parser)
{
  /* skip comments completely, comments must not be nested! */

  if (current (parser) != '>')
  {
    do
    {
      do
      {
	while (current (parser) != '\0' && current (parser) != '-')
	{
	  nextChar (parser);
	}
	if (current (parser) != '\0') nextChar (parser);
      }
      while (current (parser) != '\0' && current (parser) != '-');
      if (current (parser) != '\0') nextChar (parser);
    }
    while (current (parser) != '\0' && current (parser) != '>');
    if (current (parser) != '\0') nextChar (parser);
  }
  else
  {
    /* a comment like <!--> has occured, so there is no real tag name */

    nextChar (parser);
  }
}

void parseCData (Parser *parser)
{
  char *start;
  CDATASection *t;

  start = parser->current;
  do
  {
    do
    {
      while (current (parser) != '\0' && current (parser) != ']')
      {
	nextChar (parser);
      }
      if (current (parser) != '\0') nextChar (parser);
    }
    while (current (parser) != '\0' && current (parser) != ']');
    if (current (parser) != '\0') nextChar (parser);
  }
  while (current (parser) != '\0' && current (parser) != '>');

  *(parser->current - 2) = '\0';
  t = CDATASectionNew ();
  t->text = XkNewString (start);
  *(parser->current - 2) = ']';
  NodeAppendChild ((Node *) parser->element, (Node *) t);

  if (current (parser) != '\0') nextChar (parser);
}

void parseDoctype (Parser *parser)
{
  char t[MAX_NAME_LENGTH + 1];
  char externalID[2 * MAX_ATTR_LENGTH + 16];
  Parser *p;
  LinkHandlerInfo *linkInfo;
  char value1[MAX_ATTR_LENGTH + 1];
  char value2[MAX_ATTR_LENGTH + 1];

  value1[0] = '\0';
  value2[0] = '\0';
  parser->document->subsetStart = parser->current - parser->text;
  while (parser->text[parser->document->subsetStart] != '<')
  {
    parser->document->subsetStart--;
  }

  skipWhitespace (parser, 0);
  parseCSToken (parser, " \r\n\t[>", t);
  parser->document->doctype = XkNewString (t);

  skipWhitespace (parser, 0);
  if (current (parser) != '[' && current (parser) != '>')
  {
    parseToken (parser, " \r\n\t[>", t);
    strcpy (externalID, t);
    if (XkStrEqual (t, "PUBLIC") || XkStrEqual (t, "SYSTEM"))
    {
      parseStringLiteral (parser, value1);
      strcat (externalID, " \"");
      strcat (externalID, value1);
      strcat (externalID, "\"");

      skipWhitespace (parser, 0);
      if (current (parser) == '"')
      {
	parseStringLiteral (parser, value2);
	strcat (externalID, " \"");
	strcat (externalID, value2);
	strcat (externalID, "\"");
      }

      if (XkStrEqual (t, "SYSTEM"))
      {
	parser->element->document->systemID = XkNewString (value1);
      }
      else
      {
	parser->element->document->publicID = XkNewString (value1);
	parser->element->document->systemID = XkNewString (value2);
      }
    }
    parser->element->document->externalID = XkNewString (externalID);
    while (current (parser) != '\0' && current (parser) != '[' &&
      current (parser) != '>')
    {
      nextChar (parser);
    }
  }  

  if (current (parser) == '[')
  {
    nextChar (parser);
    parser->inProlog = 1;
  }
  else
  {
    while (current (parser) != '\0' && current (parser) != '>')
    {
      nextChar (parser);
    }
    if (current (parser) != '\0') nextChar (parser);
  }

  if (parser->document->externalID && parser->document->linkHandler)
  {
    p = ParserNew ();
    p->parent = parser;
    p->URI = XkNewString (parser->document->externalID);
    p->handling = HANDLE_DOCTYPE;
    p->document = parser->document;
    p->startElement = parser->document->root;
    ParserHold (p->parent);
    linkInfo = LinkHandlerInfoNew ();
    linkInfo->parser = p;
    linkInfo->element = parser->document->root;
    DocumentCallLinkHandler (parser->document, linkInfo);
    LinkHandlerInfoDelete (linkInfo);
  }
}

void parseInternalSubset (Parser *parser)
{
  char t[MAX_NAME_LENGTH + 1];

  if (parser->current)
  {
    while (current (parser) != '\0' && current (parser) != ']')
    {
      if (current (parser) == '<')
      {
	parseToken (parser, " \r\n\t>", t);
	if (XkStrEqual (t, "<!ELEMENT"))
	  parseElementDeclaration (parser);
	else if (XkStrEqual (t, "<!ENTITY"))
	  parseEntityDeclaration (parser);
	else if (XkStrEqual (t, "<!ATTLIST"))
	  parseAttributeDeclaration (parser);
	else
	{
	  nextChar (parser);
	}
      }
      else
      {
	nextChar (parser);
      }
    }
  }
}

void parseElementDeclaration (Parser *parser)
{
  while (current (parser) != '\0' && current (parser) != '>')
  {
    nextChar (parser);
  }
  if (current (parser) != '\0') nextChar (parser);
}

void parseEntityDeclaration (Parser *parser)
{
  char t[MAX_NAME_LENGTH + 1];
  Parser *p;
  char name[MAX_NAME_LENGTH + 1];
  char value[MAX_ATTR_LENGTH + 1];
  Entity *entity;
  LinkHandlerInfo linkInfo;

  skipWhitespace (parser, 0);
  if (current (parser) == '%')
  {
    nextChar (parser);
    skipWhitespace (parser, 0);
  }
  parseCSToken (parser, " \r\n\t", name);
  skipWhitespace (parser, 0);
  if (current (parser) == '"' || current (parser) == '\'')
  {
    parseStringLiteral (parser, value);
    entity = XkNew (Entity);
    entity->name = XkNewString (name);
    entity->value = XkNewString (value);
    entity->next = parser->document->entities;
    parser->document->entities = entity;

    while (current (parser) != '\0' && current (parser) != '>')
    {
      nextChar (parser);
    }
    if (current (parser) != '\0') nextChar (parser);
  }
  else
  {
    parseToken (parser, " \r\n\t", t);
    if (XkStrEqual (t, "PUBLIC"))
    {
      parseStringLiteral (parser, value);
    }
    parseStringLiteral (parser, value);

    if (parser->document->linkHandler)
    {
      p = ParserNew ();
      p->URI = XkNewString (value);
      p->entityName = XkNewString (name);
      p->handling = HANDLE_ENTITY;
      p->document = parser->document;
      p->parent = parser;
      
      ParserHold (p->parent);
      
      while (current (parser) != '\0' && current (parser) != '>')
      {
	nextChar (parser);
      }
      if (current (parser) != '\0') nextChar (parser);

      linkInfo.parser = p;
      linkInfo.element = NULL;
      DocumentCallLinkHandler (p->document, &linkInfo);
    }
    else
    {
      while (current (parser) != '\0' && current (parser) != '>')
      {
	nextChar (parser);
      }
      if (current (parser) != '\0') nextChar (parser);
    }
  }
}

void parseAttributeDeclaration (Parser *parser)
{
  char t[MAX_NAME_LENGTH + 1];
  char tag[MAX_NAME_LENGTH + 1];
  char name[MAX_NAME_LENGTH + 1];
  char value[MAX_ATTR_LENGTH + 1];

  skipWhitespace (parser, 0);
  parseCSToken (parser, " \r\n\t", tag);
  while (current (parser) != '\0' && current (parser) != '>')
  {
    skipWhitespace (parser, 0);
    parseCSToken (parser, " \r\n\t", name);

    skipWhitespace (parser, 0);
    parseCSToken (parser, " \r\n\t", t);

    if (XkStrEqual (t, "CDATA") || XkStrEqual (t, "ID") ||
      XkStrEqual (t, "IDREF") || XkStrEqual (t, "IDREFS") ||
      XkStrEqual (t, "ENTITY") || XkStrEqual (t, "ENTITIES") ||
      XkStrEqual (t, "NMTOKEN") || XkStrEqual (t, "NMTOKENS"))
    {
      skipWhitespace (parser, 0);
    }
    else
    {
      while (current (parser) != '\0' && current (parser) != ')')
      {
	nextChar (parser);
      }
      if (current (parser) == ')') nextChar (parser);
      skipWhitespace (parser, 0);
    }

    if (current (parser) == '#')
    {
      parseToken (parser, " \n\r\t>", t);
      if (XkStrEqual (t, "#FIXED"))
      {
	skipWhitespace (parser, 0);
	parseStringLiteral (parser, value);
	DocumentAddDefaultAttribute (parser->document, tag, name, value);
      }
    }
    else
    {
      parseStringLiteral (parser, value);
      DocumentAddDefaultAttribute (parser->document, tag, name, value);
    }

    skipWhitespace (parser, 0);
  }
  if (current (parser) != '\0') nextChar (parser);
}

/*-------------------------------------------------------------------------
 * Function......: parseToken
 * Description...: 
 *                 
 * In............: Parser *
 *                 char *
 *                 char *
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseToken (Parser *parser, char *delimiter, char *buffer)
{
  int i;

  i = 0;
  while (current (parser) != '\0' &&
    index (delimiter, current (parser)) == NULL)
  {
    /* change any character to uppercase */

    buffer[i] = toupper (current (parser));
    if (i < MAX_NAME_LENGTH)
    {
      i++;
    }
    nextChar (parser);
  }
  buffer[i] = '\0';
}

/*-------------------------------------------------------------------------
 * Function......: parseCSToken
 * Description...: 
 *                 
 * In............: Parser *
 *                 char *
 *                 char *
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseCSToken (Parser *parser, char *delimiter, char *buffer)
{
  int i;

  i = 0;
  while (current (parser) != '\0' &&
    index (delimiter, current (parser)) == NULL)
  {
    buffer[i] = current (parser);
    if (i < MAX_NAME_LENGTH)
    {
      i++;
    }
    nextChar (parser);
  }
  buffer[i] = '\0';
}

/*-------------------------------------------------------------------------
 * Function......: skipWhitespace
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void skipWhitespace (Parser *parser, int addToElement)
{
  char buffer[MAX_WORD + 1];
  int i;
  Text *t;

  if (addToElement)
  {
    i = 0;
    while (current (parser) != '\0' && isspace (current (parser)))
    {
      buffer[i++] = current (parser);
      nextChar (parser);
    }
    buffer[i] = '\0';
    if (i > 0)
    {
      t = ParserAddWord (parser, buffer, 1);
      if (t) t->whitespace = 1;
    }
  }
  else
  {
    while (current (parser) != '\0' && isspace (current (parser)))
    {
      nextChar (parser);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseTag
 * Description...: Try to parse a complete tag with name and
 *                 attributes. The tag is handed on to the
 *                 extensions. If the tag was not complete but there 
 *                 was still pending input, restore the current
 *                 parsing position
 *
 * In............: Parser *: the parser structure
 * Out...........: int: 0, if the tag was parsed completely;
 *                 1 if the tag was incomplete
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: any recognized tag has been parsed and handed on to
 *                 the extensions, or the current parsing position has
 *                 been retained 
 *-----------------------------------------------------------------------*/

int parseTag (Parser *parser)
{
  TagHandlerInfo *info;
  int r;
  char name[MAX_NAME_LENGTH + 1];
  char class[MAX_NAME_LENGTH + 1];
  char *ns;
  Attribute *a;

  /* remember current position */

  parser->last = parser->current;
  parser->tagStart = parser->current - 1;
  r = 1;

  skipWhitespace (parser, 1);
  parser->last = parser->current;

  parser->trailingSpace = 0;

  /* construct taginfo structure */

  info = TagHandlerInfoNew ();
  if (current (parser) == '/')
  {
    nextChar (parser);
    parseCSToken (parser, " \r\n\t>/", name);
    ns = strchr (name, ':');
    if (ns == NULL)
    {
      info->name = malloc (strlen (name) + 2);
      strcpy (info->name, "/");
      strcat (info->name, name);
    }
    else
    {
      info->name = malloc (strlen (name) - (ns - name) + 2);
      info->namespace = calloc (ns - name + 2, 1);
      strcpy (info->name, "/");
      strcat (info->name, ns + 1);
      strncpy (info->namespace, name, ns - name);
    }
  }
  else
  {
    parseCSToken (parser, " .\r\n\t>/", name);
    ns = strchr (name, ':');
    if (ns == NULL)
    {
      info->name = malloc (strlen (name) + 2);
      strcpy (info->name, name);
    }
    else
    {
      info->name = malloc (strlen (name) - (ns - name) + 2);
      info->namespace = calloc (ns - name + 2, 1);
      strcpy (info->name, ns + 1);
      strncpy (info->namespace, name, ns - name);
    }
    if (current (parser) == '.')
    {
      nextChar (parser);
      parseCSToken (parser, " \r\n\t>/", class);
      a = AttributeNew ();
      a->next = info->attributes;
      info->attributes = a;
      a->name = XkNewString ("class");
      a->value = XkNewString (class);
    }
  }

  /* parse attributes if there is more text or no more input is */
  /* pending */
    
  if (current (parser) != '\0' || !parser->inputPending)
  {
    a = parseTagAttributes (parser);
    if (info->attributes != NULL) info->attributes->next = a;
    else info->attributes = a;

    if (current (parser) == '/')
    {
      info->empty = 1;
      nextChar (parser);
    }
    else
    {
      info->empty = 0;
    }
    if (current (parser) != '\0') nextChar (parser);

    /* if the attributes were parsed completely notify extensions */
      
    if (current (parser) != '\0' || !parser->inputPending)
    {
      callTagHandlers (parser, info);
      r = 0;
    }
  }

  if (info->name != NULL && info->name[0] != '/')
  {
    if (XkStrEqual (info->name, "pre"))
    {
      while (current (parser) == '\n' || current (parser) == '\r')
      {
	nextChar (parser);
      }
    }
    else
    {
      skipWhitespace (parser, 1);
    }
  }

  if (r == 1)
  {
    parser->current = parser->last;
  }

  TagHandlerInfoDelete (info);
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: parseTagAttributes
 * Description...: Try to parse tag attributes, i.e. either a single
 *                 keyword as a name or a name/value pair separated by
 *                 '='. Attribute values can be enclosed in quotes
 *
 * In............: Parser *: the parser structure
 * Out...........: Attribute *: new allocated attribute structure (list)
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: the parsing position is one character past the end
 *                 ('>') of the tag 
 *-----------------------------------------------------------------------*/

Attribute *parseTagAttributes (Parser *parser)
{
  Attribute *a, *r;
  char name[MAX_ATTR_LENGTH + 1];
  char *ns;
  char value[MAX_ATTR_LENGTH + 1]; /* limits according to the HTML DTD */

  r = NULL;

  /* skip leading whitespace */

  skipWhitespace (parser, 0);

  if (current (parser) != '>')
  {
    /* tag has not ended yet, so parse the attributes */

    do
    {
      a = AttributeNew ();
      a->next = r;
      r = a;

      /* skip whitespace before the attribute */

      skipWhitespace (parser, 0);

      /* tag has not ended yet, so parse the attributes */

      if (current (parser) != '>' &&
	strncmp (parser->current, "/>", 2) &&
	strncmp (parser->current, "?>", 2) != 0)
      {
	/* parse and allocate attribute name */

	parseCSToken (parser, " =>\r\n\t/", name);
	ns = strchr (name, ':');
	if (ns == NULL)
	{
	  a->name = malloc (strlen (name) + 2);
	  strcpy (a->name, name);
	}
	else
	{
	  a->name = malloc (strlen (name) - (ns - name) + 2);
	  a->namespace = calloc (ns - name + 2, 1);
	  strcpy (a->name, ns + 1);
	  strncpy (a->namespace, name, ns - name);
	}

	skipWhitespace (parser, 0);
	if (current (parser) == '=')
	{
	  nextChar (parser);
	  parseStringLiteral (parser, value);
	  a->value = XkNewString (value);
	}
	else
	{
	  a->value = NULL;
	}

      }
    }
    while (current (parser) != '\0' && current (parser) != '>' && 
      strncmp (parser->current, "/>", 2) != 0 &&
      strncmp (parser->current, "?>", 2) != 0);
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: parseStringLiteral
 * Description...: Try to parse the value of an attribute, any
 *                 enclosing quotes are removed
 *
 * In............: Parser *: the parser structure
 *                 char *: string where to put the value
 * Out...........: -
 * Precondition..: the parser must have been allocated and the current
 *                 parsing position in the parser must be valid
 * Postcondition.: the parsing position is at the end of the attribute
 *                 value (including a trailing qoute)
 *-----------------------------------------------------------------------*/

void parseStringLiteral (Parser *parser, char *value)
{	
  char *v;
  char delimiter;

  value[0] = '\0';
  v = value;
  
  /* skip leading whitespace */

  skipWhitespace (parser, 0);

  /* check if the value is enclosed in quotes or other delimiters */

  delimiter = current (parser);
  if (delimiter == '"' || delimiter == '\'' || delimiter == '§')
  {
    nextChar (parser);

    /* get everything until the ending delimiter is reached */

    while (current (parser) != delimiter && current (parser) != '\0')
    {
      if (current (parser) != '&')
      {
	if (!isspace (current (parser)))
	{
	  *v = current (parser);
	}
	else
	{
	  *v = ' ';
	}
	
	/* ignore very long values */
	
	if (v - value < MAX_ATTR_LENGTH)
	{
	  v++;
	}
	nextChar (parser);
      }
      else
      {
	expandEntity (parser);
	if (current (parser) == '<' || current (parser) == '&')
	{
	  *v = current (parser);
	  if (v - value < MAX_ATTR_LENGTH) v++;
	  nextChar (parser);
	}
      }
    }
    *v = '\0';
    if (current (parser) == delimiter)
    {
      nextChar (parser);
    }

    /* skip whitespace after the value */

    skipWhitespace (parser, 0);
  }
  else
  {
    /* get everything until delimiting whitespace or the end tag */
    /* delimiter is reached */

    while (current (parser) != '\0' && !isspace (current (parser)) &&
      current (parser) != '>')
    {
      *v = current (parser);

      /* ignore very long values */

      if (v - value < MAX_ATTR_LENGTH)
      {
	v++;
      }
      nextChar (parser);
    }

    /* skip whitespace after the value */

    skipWhitespace (parser, 0);
  }
  
  *v = '\0';
}

/*-------------------------------------------------------------------------
 * Function......: callTagHandlers
 * Description...: Process any tags and call
 *                 the external tag handlers afterwards 
 *
 * In............: Parser *: the parser structure
 *                 TagHandlerInfo *: tag information
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void callTagHandlers (Parser *parser, TagHandlerInfo *info)
{
  int handled;

  assert (info != NULL);
  info->parser = parser;

  /* handle HTML tags */

  handled = 0;
  if (parser->document->useHTMLStyles) 
  {
    handled = HTMLTagsHandler (parser, info);
  }

  handled = handled || TableTagsHandler (parser, info);

  /* handle all remaining tags as XML tags */

  XMLTagsPreHandler (parser, info, handled);

  /* notify external handlers */

  DocumentCallTagHandler (parser->document, info);

  XMLTagsPostHandler (parser, info, handled);
}

/*-------------------------------------------------------------------------
 * Function......: printTagHandlerInfo
 * Description...: Print the name and attributes of a tag to stdout
 *
 * In............: TagHandlerInfo *: taginfo to be printed
 * Out...........: -
 * Precondition..: the taginfo must have been allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void printTagHandlerInfo (TagHandlerInfo *i)
{
  Attribute *a;

  printf ("Tag: %s", i->name);
  if (i->attributes != NULL)
  {
    a = i->attributes;
    printf (" (");
    while (a != NULL)
    {
      printf ("%s = %s ", a->name, a->value);
      a = a->next;
    }
    printf (")");
  }
  printf ("\n");
}

/*-------------------------------------------------------------------------
 * Function......: parseAtKeyword
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseAtKeyword (Parser *parser)
{
}

/*-------------------------------------------------------------------------
 * Function......: parseCSSComment
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseCSSComment (Parser *parser)
{
  while (current (parser) != '\0' && strncmp (parser->current, "*/", 2) != 0)
  {
    nextChar (parser);
  }
  if (current (parser) != '\0') nextChar (parser);
  if (current (parser) != '\0') nextChar (parser);
}

/*-------------------------------------------------------------------------
 * Function......: parseSelector
 * Description...: 
 *                 
 * In............: Parser *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseSelector (Parser *parser, char *buffer)
{
  parseCSToken (parser, " \r\n\t,{", buffer);
  skipWhitespace (parser, 0);
}

/*-------------------------------------------------------------------------
 * Function......: parseDeclaration
 * Description...: 
 *                 
 * In............: Parser *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void parseDeclaration (Parser *parser, char *buffer)
{
  char *c;

  c = buffer;
  if (current (parser) == '{') nextChar (parser);
  skipWhitespace (parser, 0);
  while (current (parser) != '}' && current (parser) != '\0')
  {
    if (c - buffer < MAX_WORD)
    {
      *c = current (parser);
      c++;
    }
    nextChar (parser);
  }
  *c = '\0';
}

/*-------------------------------------------------------------------------
 * Function......: addEntity
 * Description...: 
 *                 
 * In............: Parser *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void addEntity (Parser *parser)
{
  Entity *entity;

  entity = XkNew (Entity);
  entity->name = XkNewString (parser->entityName);
  entity->value = XkNewString (parser->text);
  entity->next = parser->document->entities;
  parser->document->entities = entity;
  ParserRelease (parser);
}
