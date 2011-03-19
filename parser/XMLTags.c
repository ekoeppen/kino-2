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
 * File..........: XMLTags.c
 * Description...: 
 *
 * $Id: XMLTags.c,v 1.8 1999/04/14 14:32:21 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/* #define DEBUG */

static void fetchLink (Parser *, Element *);

/*-------------------------------------------------------------------------
 * Function......: XMLTagsPreHandler
 * Description...: Function called during parsing
 *
 * In............: Widget: the Kino widget
 *                 XtPointer: the AHD object
 *                 XtPointer: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int XMLTagsPreHandler (Parser *parser, TagHandlerInfo *i, int handled)
{
  if (!handled && i != NULL && i->name != NULL)
  {
    if (i->name[0] != '/')
    {
      ParserStartElement (parser, i);
    }
  }

  return 1;
}

/*-------------------------------------------------------------------------
 * Function......: XMLTagsPostHandler
 * Description...: Function called during parsing
 *
 * In............: Widget: the Kino widget
 *                 XtPointer: the AHD object
 *                 XtPointer: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int XMLTagsPostHandler (Parser *parser, TagHandlerInfo *i, int handled)
{
  Element *box;
  char *actuate;

  if (!handled && i != NULL && i->name != NULL)
  {
    if (i->name[0] == '/' || i->empty)
    {
      box = ParserFinishElement (parser, i);
      if (AttributeGetValue (box->attributes, "XML:LINK") != NULL)
      {
	actuate = AttributeGetValue (box->attributes, "ACTUATE");
	if (XkStrEqual (actuate, "AUTO"))
	{
	  fetchLink (parser, box);
	}
      }
    }
  }

  return 1;
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

void XMLEventHandler (HandlerInfo *i)
{
  EventHandlerInfo *e;

  e = i->info.event;
  if (e->source != NULL &&
    e->reason == EE_CLICK && 
    e->source->type == ELEMENT_NODE &&
    AttributeGetValue (((Element *) e->source)->attributes,
      "XML:LINK") != NULL)
  {
#ifdef FIXME
    EventReset (k);
#endif
    fetchLink (NULL, (Element *) e->source);
  }
}

/*-------------------------------------------------------------------------
 * Function......: fetchLink
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void fetchLink (Parser *p, Element *element)
{
  char *show;
  char *link;
  Parser *parser;
  LinkHandlerInfo linkInfo;

  link = AttributeGetValue (element->attributes, "XML:LINK");
  if (!XkStrEqual (link, "EXTENDED"))
  {
    parser = ParserNew ();
    parser->parent = p;
    parser->URI = 
      XkNewString (AttributeGetValue (element->attributes, "HREF"));

#ifdef DEBUG
    fprintf (stderr, "%s: %s\n", __PRETTY_FUNCTION__, parser->URI);
#endif

    show = AttributeGetValue (element->attributes, "SHOW");
    if (XkStrEqual (show, "EMBED"))
    {
      parser->handling = HANDLE_EMBED;
      parser->document = element->document;
      parser->startElement = ParserStartElement (parser, NULL);
      parser->startElement->tag = XkNewString ("KINO2:EMBEDDED");
      NodeInsertBefore ((Node *) element->parent,
	NULL, (Node *) parser->startElement);
      parser->startElement->display = DISPLAY_INLINE;
      parser->startElement->floating = FLOAT_NONE;
      parser->startElement->anonymous = ANON_CONT;
      if (parser->parent) ParserHold (parser->parent);
    }
    else if (XkStrEqual (show, "REPLACE"))
    {
      /*
	NodeDelete ((Node *) element->document->root);
	ElementDeclarationDelete (element->document->elements);
	EntityDelete (element->document->entities);
	RuleSetDelete (element->document->cssRules);
	element->document->elements = NULL;
	element->document->cssRules = NULL;
	element->document->entities = NULL;

	parser->handling = HANDLE_REPLACE;
	parser->document = element->document;
	parser->startElement = ParserStartElement (parser, NULL);
	parser->startElement->tag = XkNewString ("KINO2:REPLACED");
	parser->startElement->display = DISPLAY_BLOCK;
	parser->startElement->floating = FLOAT_NONE;
	parser->startElement->anonymous = ANON_BOX;
	element->document->root = parser->startElement;
	*/

      parser->handling = HANDLE_REPLACE;
      parser->document = element->document;
      parser->startElement = NULL;
    }
    else if (XkStrEqual (show, "NEW"))
    {
      parser->handling = HANDLE_NEW;
      parser->document = element->document;
      parser->startElement = NULL;
    }
    
    linkInfo.parser = parser;
    linkInfo.element = element;
    DocumentCallLinkHandler (element->document, &linkInfo);
  }
}
