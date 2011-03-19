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
 * File..........: HTMLTags.c
 * Description...: Implementation of the HTMLTags object which realizes
 *                 most of the standard HTML tag handling
 *
 * $Id: HTMLTags.c,v 1.7 1999/06/30 17:28:57 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/* local defintions */

#define UNKNOWN_TAG        NULL
#define IGNORE             NULL
#define UL_ITEM              -1

/* the HTMLTagHandler function type */

typedef void (HTMLTagHandler)(Parser *, TagHandlerInfo *);

typedef struct			/* a complete Tag handler element */
{
  char *name;			/* name of handled tag */
  HTMLTagHandler *start;		/* called for beginning tag */
  HTMLTagHandler *end;		/* called for ending tag */
} Tag;

/* local functions */

static Tag *lookupTag (char *);

static HTMLTagHandler parseLink;
static HTMLTagHandler parseStyle;
static HTMLTagHandler parseBody;
static HTMLTagHandler parseA;
static HTMLTagHandler parseImg;
static HTMLTagHandler parseHR;
static HTMLTagHandler parseBr;
static HTMLTagHandler parseFont;
static HTMLTagHandler parseDL;
static HTMLTagHandler parseDT;
static HTMLTagHandler parseDD;
static HTMLTagHandler parseLI;
static HTMLTagHandler parsePre;

static Tag tag[] =
{
  {"LINK",       parseLink,       IGNORE},
  {"STYLE",      parseStyle,      parseStyle},
  {"BODY",       parseBody,       IGNORE},
  {"A",          parseA,          parseA},
  {"IMG",        parseImg,        IGNORE},
  {"HR",         parseHR,         IGNORE},
  {"BR",         parseBr,         IGNORE},
  {"FONT",       parseFont,       parseFont},
  {"DL",         parseDL,         parseDL},
  {"DT",         parseDT,         parseDT},
  {"DD",         parseDD,         IGNORE},
  {"LI",         parseLI,         IGNORE},
  {"PRE",        parsePre,        parsePre},
  {"LISTING",    parsePre,        parsePre},
};

/*-------------------------------------------------------------------------
 * Function......: HTMLTagsHandler
 * Description...: Function called during parsing, handles most of the
 *                 standard tags
 *
 * In............: Parser *: the parser
 *
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int HTMLTagsHandler (Parser *parser, TagHandlerInfo *info)
{
  Tag *t;
  int r;

  r = 1;
  t = lookupTag (info->name);
  if (t != UNKNOWN_TAG)
  {
    if (info->name[0] != '/')
    {
      /* beginning tag encountered */

      if (t->start != IGNORE) t->start (parser, info);
    }
    else
    {
      /* ending tag encountered */

      if (t->end != IGNORE) t->end (parser, info);
    }
  }
  else r = 0;

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: lookupTag
 * Description...: Search for a tag in the tag handler array
 *
 * In............: char *: name of the tag
 * Out...........: Tag *: entry in the tag hanlder array or UNKOWN_TAG if
 *                 no handler was found
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Tag *lookupTag (char *name)
{
  Tag *t;

  if (name[0] == '/') name++;
  t = tag;
  while (t - tag < XkNumber (tag) && !XkStrEqual (name, t->name))
  {
    t++;
  }
  if (t - tag < XkNumber (tag))
  {
    return t;
  }
  else
  {
    return UNKNOWN_TAG;
  }
}

/*-------------------------------------------------------------------------
 * Function......: HTMLTagsCheckOpenParagraph
 * Description...: 
 *                 
 * In............: Parser *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void HTMLTagsCheckOpenParagraph (Parser *parser)
{
  if (parser->element && parser->element->tag &&
    strcmp (parser->element->tag, "P") == 0)
  {
    parser->element = parser->element->parent;
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseBody
 * Description...: handle the BODY tag, set the background and text color
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseBody (Parser *parser, TagHandlerInfo *i)
{
  /*  char *v;
      KinoColor p; */
  Element *box;

  if (i->name[0] != '/')
  {
    box = ParserStartElement (parser, i);
    box->display = DISPLAY_BLOCK;
    box->floating = FLOAT_NONE;

    /*
    v = AttributeGetValue (i->attributes, "BGCOLOR");
    if (v != NULL)
    {
      p = getColor (v);
    }
    else
    {
      p = getColor ("C0C0C0");
    }

    v = AttributeGetValue (i->attributes, "LINK");
    if (v != NULL)
    {
      p = getColor (v);
    }
    else
    {
      p = getColor ("0000FF");
    } */
  }
  else
  {
    ParserFinishElement (parser, i);
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseHead
 * Description...: handle the LINK tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseLink (Parser *parser, TagHandlerInfo *i)
{
  Element *b;
  char *rel;
  char *href;
  Parser *p;
  LinkHandlerInfo *linkInfo;

  b = ParserStartElement (parser, i);
  b->floating = FLOAT_NONE;
  b->display = DISPLAY_NONE;

  rel = AttributeGetValue (i->attributes, "REL");
  href = AttributeGetValue (i->attributes, "HREF");

  if (XkStrEqual (rel, "stylesheet") && parser->document->linkHandler)
  {
    p = ParserNew ();
    p->URI = XkNewString (href);
    p->handling = HANDLE_EMBED;
    p->document = parser->document;
    p->startElement = b;
    linkInfo = LinkHandlerInfoNew ();
    linkInfo->parser = p;
    linkInfo->element = b;
    DocumentCallLinkHandler (p->document, linkInfo);
    LinkHandlerInfoDelete (linkInfo);
  }

  ParserFinishElement (parser, i);
}

/*-------------------------------------------------------------------------
 * Function......: parseStyle
 * Description...: handle the STYLE tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseStyle (Parser *parser, TagHandlerInfo *i)
{
  Element *b;
  Parser *p;

  if (i->name[0] != '/')
  {
    b = ParserStartElement (parser, i);
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_NONE;
    b->saveRawContents = 1;
  }
  else
  {
    b = ParserFinishElement (parser, i);
    p = ParserNew ();
    p->startElement = b->parent;
    p->handling = HANDLE_EMBED;
    ParserProcessData (p, "text/css", b->rawContents);
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseA
 * Description...: handle the A tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseA (Parser *parser, TagHandlerInfo *i)
{
  Element *b;
  Attribute *a;

  if (i->name[0] != '/')
  {
    b = ParserStartElement (parser, i);
    if (AttributeExists (i->attributes, "HREF"))
    {
      a = AttributeNew ();
      a->next = b->attributes;
      b->attributes = a;
      a->name = XkNewString ("CLASS");
      a->value = XkNewString ("link");

      a = AttributeNew ();
      a->next = b->attributes;
      b->attributes = a;
      a->name = XkNewString ("XML:LINK");
      a->value = XkNewString ("simple");

      a = AttributeNew ();
      a->next = b->attributes;
      b->attributes = a;
      a->name = XkNewString ("SHOW");
      a->value = XkNewString ("replace");

      a = AttributeNew ();
      a->next = b->attributes;
      b->attributes = a;
      a->name = XkNewString ("ACTUATE");
      a->value = XkNewString ("user");
    }
  }
  else
  {
    ParserFinishElement (parser, i);
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseImg
 * Description...: handle the IMG tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseImg (Parser *parser, TagHandlerInfo *i)
{
  Element *b;

  b = ParserStartElement (parser, i);
  ParserFinishElement (parser, i);
}

/*-------------------------------------------------------------------------
 * Function......: parseHr
 * Description...: handle the HR tag, insert a horizontal rule on a
 *                 new line
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseHR (Parser *parser, TagHandlerInfo *i)
{
  Node *p;
  
  p = LinebreakNew ();
  p->parent = parser->element;
  NodeAppendChild ((Node *) parser->element, p);
}

/*-------------------------------------------------------------------------
 * Function......: parseBr
 * Description...: handle the Br tag, insert a vertical skip
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseBr (Parser *parser, TagHandlerInfo *i)
{
  Node *p;
  
  p = LinebreakNew ();
  p->parent = parser->element;
  NodeAppendChild ((Node *) parser->element, p);
}

/*-------------------------------------------------------------------------
 * Function......: parseFont
 * Description...: handle the FONT tag, try to load the requested font
 *                 and change the text color
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseFont (Parser *parser, TagHandlerInfo *i)
{
}

/*-------------------------------------------------------------------------
 * Function......: parseDL
 * Description...: handle the DL tag, starts/ends a descripive list
 *                 and handles indention
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseDL (Parser *parser, TagHandlerInfo *i)
{
  Element *b;

  if (i->name[0] != '/')
  {
    b = ParserStartElement (parser, i);
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_BLOCK;
  }
  else
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "DL") != 0)))
    {
      parser->element = parser->element->parent;
    }

    ParserFinishElement (parser, i);
  }

  /*  if (e->dListDepth < MAX_LIST_DEPTH && i->name[0] != '/')
      {
      HTMLTagsCheckOpenParagraph (parser);

      e->dListDepth++;
      e->dListIndented[e->dListDepth] = 0;
      }
      else if (e->dListDepth > 0 && i->name[0] == '/')
      {
      if (e->dListIndented[e->dListDepth])
      {
      }
      e->dListDepth--;
      }
      */
}

/*-------------------------------------------------------------------------
 * Function......: parseDT
 * Description...: handle the DT tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseDT (Parser *parser, TagHandlerInfo *i)
{
  Element *b;

  if (i->name[0] != '/')
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "DL") != 0)))
    {
      parser->element = parser->element->parent;
    }

    b = ParserStartElement (parser, i);
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_BLOCK;
  }
  else
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "DL") != 0)))
    {
      parser->element = parser->element->parent;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseDD
 * Description...: handle the DD tag
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseDD (Parser *parser, TagHandlerInfo *i)
{
  Element *b;

  if (i->name[0] != '/')
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "DL") != 0)))
    {
      parser->element = parser->element->parent;
    }

    b = ParserStartElement (parser, i);
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_BLOCK;
  }
  else
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "DL") != 0)))
    {
      parser->element = parser->element->parent;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: parseLI
 * Description...: handle the LI tag, insert an ordered or unordered
 *                 list item (bullet or number)
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parseLI (Parser *parser, TagHandlerInfo *i)
{
  Element *b;

  if (i->name[0] != '/')
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "OL") != 0 &&
	strcmp (parser->element->tag, "UL") != 0)))
    {
      parser->element = parser->element->parent;
    }

    /* enclosing box */

    b = ParserStartElement (parser, i);
    b->tag = XkNewString ("KINO2:LIBOX");
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_BLOCK;
    b->anonymous = ANON_BOX;

    /* bullet box */

    b = ParserStartElement (parser, i);
    b->tag = XkNewString ("KINO2:LIBULLET");
    b->floating = FLOAT_LEFT;
    b->display = DISPLAY_BLOCK;
    b->font_family = FONT_FAMILY_SYMBOL;
    b->anonymous = ANON_CONT;

    ParserAddWord (parser, "\xB7", 0);
    ParserFinishElement (parser, NULL);

    /* text box */

    b = ParserStartElement (parser, i);
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_BLOCK;
  }
  else
  {
    while (parser->element != NULL &&
      parser->element->parent != NULL &&
      (parser->element->tag == NULL ||
      (parser->element->tag != NULL &&
	strcmp (parser->element->tag, "OL") != 0 &&
	strcmp (parser->element->tag, "UL") != 0)))
    {
      parser->element = parser->element->parent;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: parsePre
 * Description...: handle the PRE tag, changes autoformatting of the
 *                 parser and changes the font
 *
 * In............: KinoWidget: the Kino widget
 *                 TagHandlerInfo *: information about the current tag
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void parsePre (Parser *parser, TagHandlerInfo *i)
{
  Element *b;

  if (i->name[0] != '/')
  {
    HTMLTagsCheckOpenParagraph (parser);
    b = ParserStartElement (parser, i);
    b->floating = FLOAT_NONE;
    b->display = DISPLAY_BLOCK;
    b->font_family = FONT_FAMILY_COURIER;
    parser->autoformat = 0;
  }
  else
  {
    ParserFinishElement (parser, i);
    parser->autoformat = 1;
  }
}
