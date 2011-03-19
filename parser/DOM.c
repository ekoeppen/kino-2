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
 * File..........: DOM.c
 * Description...: Implementation of the DOM structures which are the most
 *                 important data structures since they hold the parsed data
 *                 of the source text and are the basis for any further
 *                 processing
 *
 * $Id: DOM.c,v 1.8 1999/06/30 17:27:06 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/*
#define DEBUG
*/

/* local functions */

static void deleteContents (Element *b);
static char *toText (Node *, char *, int *, int *);
static char *toPostScript (Element *, char *, int, int, int *, int *);
static char *strvarcat (char *, char *, int *, int *);
static int iteratorMatch (NodeIterator *i);
static Node *successor (Node *);
static int recursiveMatch (Element *, Element *, NodeList *);
static void searchSubtree (Element *, char *, NodeList *);
static void setOwnerDocumentModified (Node *, int);

/*-------------------------------------------------------------------------
 * Function......: NodeDelete
 * Description...: Delete the Node
 *
 * In............: KinoWidget: widget containing the Node list
 * Out...........: -
 * Precondition..: -
 * Postcondition.: the Node and its children are deleted
 *-----------------------------------------------------------------------*/

void NodeDelete (Node *p)
{
  Node *previous, *q;

#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, p);
#endif

  if (p != NULL)
  {
    if (p->type == DOCUMENT_NODE)
    {
      q = p;
      p = (Node *) ((Document *) p)->root;
      NodeFree (q);
      ((Element *) p)->document = NULL;
    }
    setOwnerDocumentModified (p, 1);
    previous = NodeGetPreviousSibling (p);
    if (previous != NULL)
    {
      previous->next = p->next;
    }
    else if (p->parent != NULL)
    {
      p->parent->contents = p->next;
    }
  
    NodeFree (p);
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeFree
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NodeFree (Node *p)
{
  Document *document;
  Element *box;
  ProcessingInstruction *pi;

  switch (p->type)
  {
    case TEXT_NODE:
      XkFree (((Text *) p)->text);
      break;
    case ELEMENT_NODE:
      box = (Element *) p;
      if (box->rawContents) XkFree (box->rawContents);
      if (box->tag) XkFree (box->tag);
      if (box->namespace) XkFree (box->namespace);
      deleteContents (box);
      if (box->line && 
	(box->parent == NULL || box->parent->line != box->line))
      {
	LineDelete (box->line);
      }
      AttributeDelete (box->attributes);
      break;
    case INSET_NODE:
      if (((Inset *) p)->delete != NULL)
      {
	((Inset *) p)->delete (((Inset *) p)->inset);
      }
      break;
    case LINEBREAK_NODE:
      break;
    case PROCESSING_INSTRUCTION_NODE:
      pi = (ProcessingInstruction *) p;
      if (pi->target != NULL) XkFree (pi->target);
      if (pi->data != NULL) XkFree (pi->data);
      if (pi->attributes != NULL) AttributeDelete (pi->attributes);
      break;
    case DOCUMENT_NODE:
      document = (Document *) p;
      ElementDeclarationDelete (document->elements);
      EntityDelete (document->entities);
      RuleSetDelete (document->cssRules);
      HandlerDelete (document->tagHandler);
      HandlerDelete (document->linkHandler);
      HandlerDelete (document->eventHandler);
      if (document->internalSubset) XkFree (document->internalSubset);
      if (document->doctype) XkFree (document->doctype);
      if (document->externalID) XkFree (document->externalID);
      if (document->systemID) XkFree (document->systemID);
      if (document->publicID) XkFree (document->publicID);
      if (document->MIMEType) XkFree (document->MIMEType);
#ifdef HAVE_LIBPHTHREAD
      pthread_mutex_destroy (document->readerMutex);
      pthread_mutex_destroy (document->writerMutex);
#endif
      break;
    case CDATA_SECTION_NODE:
      XkFree (((CDATASection *) p)->text);
      break;
    case ATTRIBUTE_NODE:
    case ENTITY_REFERENCE_NODE:
    case ENTITY_NODE:
    case COMMENT_NODE:
    case DOCUMENT_TYPE_NODE:
    case DOCUMENT_FRAGMENT_NODE:
    case NOTATION_NODE:
      break;
  }
  XkFree (p);
}

/*-------------------------------------------------------------------------
 * Function......: deleteContents
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void deleteContents (Element *b)
{
  Node *p, *q;

  if (b != NULL)
  {
    p = b->contents;
    while (p != NULL)
    {
      q = p;
      p = p->next;
      NodeFree (q);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodePrint
 * Description...: 
 *
 * In............: Node *: the Node list
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NodePrint (Element *b, int depth)
{
  Node *p;
  char *d;

  if (b != NULL)
  {
    d = "";
    switch (b->display)
    {
      case DISPLAY_NONE: d = "none"; break;
      case DISPLAY_BLOCK: d = "block"; break;
      case DISPLAY_INLINE: d = "inline"; break;
      case DISPLAY_LIST_ITEM: d = "list-item"; break;
    }
    printf ("\n%*c<%s %d|%d-%dx%d / %d %d> (%s)", depth, ' ', 
      b->tag, b->x, b->y, b->width, b->height, b->minWidth, b->maxWidth, d);
    p = b->contents;
    while (p != NULL)
    {
      switch (p->type)
      {
	case TEXT_NODE:
	  printf ("<%s>", ((Text *) p)->text);
	  break;
	case ELEMENT_NODE:
	  NodePrint ((Element *) p, depth + 1);
	  printf ("\n%*c", depth, ' ');
	  break;
	case INSET_NODE:
	  printf ("[*] ");
	  break;
	case LINEBREAK_NODE:
	  printf ("\n");
	  break;
	case CDATA_SECTION_NODE:
	  printf ("[[%s]]", ((CDATASection *) p)->text);
	  break;
	case PROCESSING_INSTRUCTION_NODE:
	case DOCUMENT_NODE:
	case ATTRIBUTE_NODE:
	case ENTITY_REFERENCE_NODE:
	case ENTITY_NODE:
	case COMMENT_NODE:
	case DOCUMENT_TYPE_NODE:
	case DOCUMENT_FRAGMENT_NODE:
	case NOTATION_NODE:
	  break;
      }
      p = p->next;
    }
    printf ("</%s>", b->tag);
    fflush (stdout);
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeToText
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *NodeToText (Node *p)
{
  int length, allocated;

  length = 0;
  allocated = 0;
  return toText (p, NULL, &length, &allocated);
}

/*-------------------------------------------------------------------------
 * Function......: toText
 * Description...: 
 *                 
 * In............: Node *:
 *                 char *:
 *                 int *:
 *                 int *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *toText (Node *p, char *text, int *length, int *allocated)
{
  char buffer[1024];
  char *tag;
  Attribute *a;
  Element *element;
  Node *c;

  tag = NULL;
  switch (p->type)
  {
    case TEXT_NODE:
      text = strvarcat (text, ((Text *) p)->text,
	length, allocated);
      break;
    case LINEBREAK_NODE:
      text = strvarcat (text, "\n", length, allocated);
      break;
    case INSET_NODE:
      break;
    case DOCUMENT_NODE:
      text = strvarcat (text, ((Document *) p )->internalSubset,
	length, allocated);
      text = toText ((Node *) ((Document *) p )->root, text,
	length, allocated);
      break;
    case PROCESSING_INSTRUCTION_NODE:
      text = strvarcat (text, "<?", length, allocated);
      text = strvarcat (text, ((ProcessingInstruction *) p)->target,
	length, allocated);
      text = strvarcat (text, " ", length, allocated);
      text = strvarcat (text, ((ProcessingInstruction *) p)->data,
	length, allocated);
      text = strvarcat (text, "?>", length, allocated);
      break;
    case ELEMENT_NODE:
      element = (Element *) p;
      if (element->anonymous != ANON_CONT)
      {
	if (element->anonymous != ANON_BOX)
	{
	  strcpy (buffer, "<");
	  if (element->namespace != NULL)
	  {
	    strcat (buffer, element->namespace);
	    strcat (buffer, ":");
	  }
	  tag = element->tag;
	  if (tag == NULL) tag = "UNKNOWN";
	  strcat (buffer, tag);
	  if (element->attributes != NULL) strcat (buffer, " ");
	  for (a = element->attributes; a != NULL; a = a->next)
	  {
	    if (a->name &&
	      !XkStrEqual (DocumentGetDefaultAttribute (element->document,
		element->tag, a->name), a->value))
	    {
	      strcat (buffer, a->name);
	      if (a->value != NULL)
	      {
		strcat (buffer, "=\"");
		strcat (buffer, a->value);
		strcat (buffer, "\"");
	      }
	      if (a->next != NULL) strcat (buffer, " ");
	    }
	  }

	  if (!element->empty)
	  {
	    strcat (buffer, ">");
	  }
	  else
	  {
	    strcat (buffer, "/>");
	  }

	  text = strvarcat (text, buffer, length, allocated);
	}

	if (!element->empty)
	{
	  for (c = element->contents; c != NULL; c = c->next)
	  {
	    switch (c->type)
	    {
	      case ELEMENT_NODE:
		if (((Element *) c)->anonymous == ANON_DELIMITER)
		{
		  c = c->next;
		  while (c != NULL &&
		    (c->type != ELEMENT_NODE ||
		      ((Element *) c)->anonymous != ANON_DELIMITER))
		  {
		    c = c->next;
		  }
		}
		else
		{
		  text = toText (c, text, length, allocated);
		}
		break;
	      default:
		text = toText (c, text, length, allocated);
		break;
	    }
	  }

	  if (element->anonymous != ANON_BOX)
	  {
	    strcpy (buffer, "</");
	    if (element->namespace != NULL)
	    {
	      strcat (buffer, element->namespace);
	      strcat (buffer, ":");
	    }
	    strcat (buffer, tag);
	    strcat (buffer, ">");
	    text = strvarcat (text, buffer, length, allocated);
	  }
	}
      }
      break;
    case CDATA_SECTION_NODE:
      text = strvarcat (text, "<![CDATA[", length, allocated);
      text = strvarcat (text, ((CDATASection *) p)->text,
	length, allocated);
      text = strvarcat (text, "]]>", length, allocated);
      break;
    case ATTRIBUTE_NODE:
    case ENTITY_REFERENCE_NODE:
    case ENTITY_NODE:
    case COMMENT_NODE:
    case DOCUMENT_TYPE_NODE:
    case DOCUMENT_FRAGMENT_NODE:
    case NOTATION_NODE:
      break;
  }

  return text;
}

/*-------------------------------------------------------------------------
 * Function......: NodeToPostScript
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *NodeToPostScript (Element *element)
{
  int length, allocated;
  char *text;

  length = 0;
  allocated = 0;
  text = strvarcat (NULL, "/Times-Roman findfont\n\
12 scalefont\n\
setfont\n\
newpath\n", &length, &allocated);
  text = toPostScript (element, text, 0, 0, &length, &allocated);
  text = strvarcat (text, "closepath\n\
stroke\n\
showpage\n", &length, &allocated);
  return text;
}

/*-------------------------------------------------------------------------
 * Function......: toPostScript
 * Description...: 
 *                 
 * In............: Element *:
 *                 char *:
 *                 int :
 *                 int :
 *                 int *:
 *                 int *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *toPostScript (Element *element, char *text,
  int x, int y,
  int *length, int *allocated)
{
  char buffer[1024];
  Node *p;

  if (element != NULL && element->anonymous != ANON_CONT)
  {
    for (p = element->contents; p != NULL; p = p->next)
    {
      switch (p->type)
      {
	case ELEMENT_NODE:
	  text = toPostScript ((Element *) p,
	    text,
	    x + ((Element *) p)->x,
	    y + ((Element *) p)->y, length, allocated);
	  break;
	case TEXT_NODE:
	  sprintf (buffer, "%d %d moveto (%s) show\n",
	    x + ((Text *) p)->x,
	    842 - y - ((Text *) p)->y, 
	    ((Text *) p)->text);
	  text = strvarcat (text, buffer,
	    length, allocated);
	  break;
	case LINEBREAK_NODE:
	case DOCUMENT_NODE:
	case INSET_NODE:
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

  return text;
}

/*-------------------------------------------------------------------------
 * Function......: strvarcat
 * Description...: 
 *                 
 * In............: char *:
 *                 char *:
 *                 int *:
 *                 int *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *strvarcat (char *text, char *new, int *length, int *allocated)
{
  if (new && new[0] != '\0')
  {
    if (*length + strlen (new) > *allocated)
    {
      *allocated += (2048 + strlen (new));
      text = realloc (text, *allocated);
      text[*length] = '\0';
    }
    strcat (text, new);
    *length += strlen (new);
  }

  return text;
}

/*-------------------------------------------------------------------------
 * Function......: NodeNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Node *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *NodeNew (void)
{
  Node *p;

  p = XkNew (Node);
  p->parent = NULL;
  p->next = NULL;

  return p;
}

/*-------------------------------------------------------------------------
 * Function......: LinebreakNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Node *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *LinebreakNew (void)
{
  Node *p;

  p = XkNew (Node);
  p->parent = NULL;
  p->next = NULL;
  p->type = LINEBREAK_NODE;

  return p;
}

/*-------------------------------------------------------------------------
 * Function......: ElementNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Element *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *ElementNew (void)
{
  Element *b;

  b = XkNew (Element);

  memset (b, 0, sizeof (Element));

  b->x = 0;
  b->y = 0;
  b->width = 0;
  b->height = 0;
  b->parent = NULL;
  b->next = NULL;

  b->empty = 0;
  b->anonymous = ANON_NORMAL;
  b->source = 0;
  b->document = NULL;
  b->table = NULL;
  b->saveRawContents = 0;
  b->rawContents = NULL;
  b->tag = NULL;
  b->namespace = NULL;
  b->type = ELEMENT_NODE;
  b->attributes = NULL;
  b->minWidth = 0;
  b->maxWidth = 0;
  b->floating = FLOAT_NONE;
  b->display = DISPLAY_NONE;
  b->positioned = 0;
  b->contents = NULL;
  b->collapsed = 0;
  b->text_align = ALIGN_LEFT;
  b->color.red = 0;
  b->color.green = 0;
  b->color.blue = 0;
  b->color.alpha  = 0;
  b->background_color.red = 65535;
  b->background_color.green = 65535;
  b->background_color.blue = 65535;
  b->background_color.alpha  = 0;
  b->border_top_color.red = 65535;
  b->border_top_color.green = 65535;
  b->border_top_color.blue = 65535;
  b->border_top_color.alpha = 0;
  b->border_right_color.red = 65535;
  b->border_right_color.green = 65535;
  b->border_right_color.blue = 65535;
  b->border_right_color.alpha = 0;
  b->border_left_color.red = 65535;
  b->border_left_color.green = 65535;
  b->border_left_color.blue = 65535;
  b->border_left_color.alpha = 0;
  b->border_bottom_color.red = 65535;
  b->border_bottom_color.green = 65535;
  b->border_bottom_color.blue = 65535;
  b->border_bottom_color.alpha = 0;
  b->border_top_width.effective = 0;
  b->border_right_width.effective = 0;
  b->border_left_width.effective = 0;
  b->border_bottom_width.effective = 0;
  b->padding_top.effective = 0;
  b->padding_right.effective = 0;
  b->padding_left.effective = 0;
  b->padding_bottom.effective = 0;
  b->margin_top.effective = 0;
  b->margin_right.effective = 0;
  b->margin_left.effective = 0;
  b->margin_bottom.effective = 0;
  b->line = NULL;
  b->underline = 0;
  b->overline = 0;
  b->font_size.effective = 12;
  b->font_family = FONT_FAMILY_TIMES;
  b->font_weight = FONT_WEIGHT_MEDIUM;
  b->font_slant = FONT_SLANT_ROMAN;
  b->line_height.effective = 12;

#ifdef DEBUG
  fprintf (stderr, "+++ %s: %p\n", __PRETTY_FUNCTION__, b);
#endif

  return b;
}

/*-------------------------------------------------------------------------
 * Function......: TextNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Text *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Text *TextNew (void)
{
  Text *p;

  p = XkNew (Text);
  p->type = TEXT_NODE;
  p->x = 0;
  p->y = 0;
  p->width = 0;
  p->height = 0;
  p->parent = NULL;
  p->next = NULL;

  p->text = NULL;
  p->wrap = 1;
  p->whitespace = 0;
  p->source = 0;

  return p;
}

/*-------------------------------------------------------------------------
 * Function......: CDATASectionNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: CDATASection *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

CDATASection *CDATASectionNew (void)
{
  CDATASection *p;

  p = XkNew (CDATASection);
  p->type = CDATA_SECTION_NODE;
  p->x = 0;
  p->y = 0;
  p->width = 0;
  p->height = 0;
  p->parent = NULL;
  p->next = NULL;

  p->text = NULL;
  p->source = 0;

  return p;
}

/*-------------------------------------------------------------------------
 * Function......: InsetNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Inset *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Inset *InsetNew (void)
{
  Inset *p;

  p = XkNew (Inset);
  p->x = 0;
  p->y = 0;
  p->width = 0;
  p->height = 0;
  p->parent = NULL;
  p->type = INSET_NODE;
  p->inset = NULL;
  p->source = 0;
  p->setValue = NULL;
  p->getValue = NULL;
  p->delete = NULL;
  p->next = NULL;

  return p;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Document *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Document *DocumentNew (void)
{
  Document *d;
#ifdef HAVE_LIBPTHREAD
  pthread_mutexattr_t attr;
#endif

  d = XkNew (Document);
  d->type = DOCUMENT_NODE;
  d->next = NULL;
  d->parent = NULL;
  d->elements = NULL;
  d->entities = NULL;
  d->root = NULL;
  d->cssRules = NULL;
  d->linkHandler = NULL;
  d->tagHandler = NULL;
  d->linkHandler = NULL;
  d->eventHandler = NULL;
  d->userData = NULL;
  d->doctype = NULL;
  d->externalID = NULL;
  d->systemID = NULL;
  d->publicID = NULL;
  d->internalSubset = NULL;
  d->subsetStart = 0;
  d->modified = 0;
  d->MIMEType = NULL;
  d->useHTMLStyles = 0;

#ifdef HAVE_LIBPTHREAD
  d->readers = 0;
  pthread_mutexattr_init (&attr);
  pthread_mutexattr_setkind_np (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init (&d->readerMutex, &attr);
  pthread_mutex_init (&d->writerMutex, &attr);
#endif

  DocumentAddEventHandler (d, XMLEventHandler, NULL);

  return d;
}

/*-------------------------------------------------------------------------
 * Function......: ProcessingInstructionNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: ProcessingInstructionNew *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

ProcessingInstruction *ProcessingInstructionNew (void)
{
  ProcessingInstruction *p;

  p = XkNew (ProcessingInstruction);
  p->type = PROCESSING_INSTRUCTION_NODE;
  p->parent = NULL;
  p->next = NULL;
  p->target = NULL;
  p->data = NULL;

  return p;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentAddTagHandler
 * Description...: 
 *                 
 * In............: Document *
 *                 HandlerProc 
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void DocumentAddTagHandler (Document *d, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = d->tagHandler;
  d->tagHandler = h;
}

void DocumentAddLinkHandler (Document *d, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = d->linkHandler;
  d->linkHandler = h;
}

void DocumentAddEventHandler (Document *d, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = d->eventHandler;
  d->eventHandler = h;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentCall...Handler
 * Description...: 
 *                 
 * In............: Document *
 *                 LinkHandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void DocumentCallTagHandler (Document *d, TagHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  if (d != NULL)
  {
    i.document = d;
    for (h = d->tagHandler; h != NULL; h = h->next)
    {
      i.userData = h->userData;
      i.document = d;
      i.info.tag = info;

      (h->handler)(&i);
    }
  }
}

void DocumentCallLinkHandler (Document *d, LinkHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  if (d != NULL)
  {
    i.document = d;
    for (h = d->linkHandler; h != NULL; h = h->next)
    {
      i.userData = h->userData;
      i.document = d;
      i.info.link = info;

      (h->handler)(&i);
    }
  }
}

void DocumentCallEventHandler (Document *d, EventHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  if (d != NULL)
  {
    i.document = d;
    for (h = d->eventHandler; h != NULL; h = h->next)
    {
      i.userData = h->userData;
      i.document = d;
      i.info.event = info;

      (h->handler)(&i);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeCloneNode
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: Node *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *NodeCloneNode (Node *p)
{
  Node *r, *s, *q;

  r = NULL;
  switch (p->type)
  {
    case ELEMENT_NODE:
      r = (Node *) ElementNew ();
      memcpy (r, p, sizeof (Element));
      ((Element *) r)->contents = NULL;
      ((Element *) r)->line = NULL;
      ((Element *) r)->table = NULL;
      ((Element *) r)->tag = XkNewString (((Element *) p)->tag);
      ((Element *) r)->rawContents =
	XkNewString (((Element *) p)->rawContents);
      for (q = ((Element *) p)->contents; q != NULL; q = q->next)
      {
	s = NodeCloneNode (q);
	NodeInsertBefore (r, NULL, s);
      }
      break;
    case INSET_NODE:
      r = (Node *) InsetNew ();
      break;
    case LINEBREAK_NODE:
      r = (Node *) LinebreakNew ();
      break;
    case TEXT_NODE:
      r = (Node *) TextNew ();
      ((Text *) r)->text = XkNewString (((Text *) p)->text);
      ((Text *) r)->source =  ((Text *) p)->source;
      ((Text *) r)->wrap =  ((Text *) p)->wrap;
      break;
    case DOCUMENT_NODE:
      r = (Node *) DocumentNew ();
      break;
    case PROCESSING_INSTRUCTION_NODE:
      /* FIXME! */
      break;
    case ATTRIBUTE_NODE:
    case CDATA_SECTION_NODE:
      r = (Node *) CDATASectionNew ();
      ((CDATASection *) r)->text = XkNewString (((CDATASection *) p)->text);
      ((CDATASection *) r)->source =  ((CDATASection *) p)->source;
      break;
    case ENTITY_REFERENCE_NODE:
    case ENTITY_NODE:
    case COMMENT_NODE:
    case DOCUMENT_TYPE_NODE:
    case DOCUMENT_FRAGMENT_NODE:
    case NOTATION_NODE:
      break;
  }

  r->parent = p->parent;
  r->type = p->type;

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeIsSpace
 * Description...: Check wether the element contains a single space (" ")
 *
 * In............: Node *: the Node
 * Out...........: int: 1, if the element contains a single space
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int NodeIsSpace (Node *p)
{  
  if (p->type == TEXT_NODE &&
    ((Text *) p)->text[0] == ' ' &&
    ((Text *) p)->text[1] == '\0')
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeIsWord
 * Description...: Check wether the element is a word
 *
 * In............: Node *: the Node
 * Out...........: int: 1, if the element contains a word
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int NodeIsWord (Node *p)
{
  if (p->type == TEXT_NODE &&
    !(((Text *) p)->text[0] == ' '
      && ((Text *) p)->text[1] == '\0'))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeFindInset
 * Description...: 
 *                 
 * In............: Element *:
 *                 InsetData *:
 * Out...........: Inset *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Inset *NodeFindInset (Element *b, InsetData *w)
{
  Node *p;
  Inset *i;

  i = NULL;
  if (b != NULL)
  {
    for (p = b->contents; i == NULL && p != NULL; p = p->next)
    {
      switch (p->type)
      {
	case ELEMENT_NODE:
	  i = NodeFindInset ((Element *) p, w);
	  break;
	case INSET_NODE:
	  if (((Inset *) p)->inset == w)
	  {
	    i = (Inset *) p;
	  }
	  break;
	case TEXT_NODE:
	case LINEBREAK_NODE:
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
  return i;
}

/*-------------------------------------------------------------------------
 * Function......: NodePositionAbsolute
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NodePositionAbsolute (Node *p)
{
  Element *parent;
  VisibleNode *n;

  if (p->type == ELEMENT_NODE ||
    p->type == INSET_NODE ||
    p->type == TEXT_NODE)
  {
    n = (VisibleNode *) p;
    parent = p->parent;
    while (parent != NULL)
    {
      n->x += parent->x;
      n->y += parent->y;
      parent = parent->parent;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeNextBlock
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: Element *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *NodeNextBlock (Node *p)
{
  Element *b;

  b = NULL;
  p = p->next;
  while (p != NULL && NodeIsSpace (p))
  {
    p = p->next;
  }
  if (p != NULL &&
    p->type == ELEMENT_NODE &&
    ((Element *) p)->display == DISPLAY_BLOCK)
  {
    b = (Element *) p;
  }
  return b;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetPreviousSibling
 * Description...: Returns the preceding Node of an element
 *
 * In............: Node *: the Node
 * Out...........: Node *: the preceding Node
 * Precondition..: the Node must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Node *NodeGetPreviousSibling (Node *p)
{
  Element *boss;
  Node *help1,*help2, *r;

  r = NULL;
  boss = p->parent;
  if (boss != NULL)
  {
    help1 = boss->contents;
    if (p != help1)
    {
      help2 = NULL; 
      while(help1 && help1 != p)
      {
	help2 = help1;
	help1 = help1->next;
      }
      if (help1)             /* Element gefunden */
      {
	if (help2)           /* nicht erstes Element */
	{
	  r = help2;
	}
	else
	{
	  r = boss->contents; /* erstes Element */
	}
      }
    }
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetNextSibling
 * Description...: Returns the following Node of an element
 *
 * In............: Node *: the Node
 * Out...........: Node *: the following Node
 * Precondition..: the Node must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Node *NodeGetNextSibling (Node *p)
{
  return p->next;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetSuccessor
 * Description...: 
 *                 
 * In............: Node *
 * Out...........: Node *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *NodeGetSuccessor (Node *n)
{
  Node *r;

  if (n->type == ELEMENT_NODE && ((Element *) n)->contents != NULL)
  {
    r = ((Element *) n)->contents;
  }
  else if (n->next != NULL)
  {
    r = n->next;
  }
  else if (n->parent != NULL && n->parent->next != NULL)
  {
    r = n->parent->next;
  }
  else r = NULL;

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeParentLookup
 * Description...: Returns the first Element which can be found in
 *                 the parent chain of a given Node and which
 *                 has an attribute that is equal to a given string
 *
 * In............: Node *: the Node
 *                 char *: the name of the Attribute
 *                 char *: the value of the Attribute to look for
 * Out...........: Element *: the first Element has the given name
 *                 or NULL if no element can be found
 * Precondition..: the Node must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Element *NodeParentLookup (Node *p, char *name, char *value)
{
  Element *current;
  Node *help1;
  int abort;
  char *v;

  abort = 0;
  current = (Element *) p;
  while (current && !abort)
  {
    v = AttributeGetValue (current->attributes, name);
    if (XkStrEqual (v, value))
    {
      abort=1;
    }
    else
    {
      if (current->contents)
      {
        help1 = current->contents;
        while (help1 && !abort)
        {
	  if (help1->type==ELEMENT_NODE)
	  {
	    v = AttributeGetValue (((Element *) help1)->attributes, name);
	    if (XkStrEqual (v, value))
	    {
	      abort = 1;
	      current=(Element *)help1;
	    }
	  }
          help1=help1->next;
        }
      }
    } 
    if (!abort)
    {
      current = current->parent;
    }
  }
  return current;
}

/*-------------------------------------------------------------------------
 * Function......: NodeChildLookup
 * Description...: 
 *                 
 * In............: Node *:
 *                 char *:
 *                 char *:
 * Out...........: Element *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *NodeChildLookup (Node *node, char *name, char *value)
{
  Node *p;
  Element *r;
  char *v;

  r = NULL;
  for (p = NodeGetFirstChild (node); p != NULL && r == NULL; p = p->next)
  {
    if (p->type == ELEMENT_NODE)
    {
      v = AttributeGetValue (((Element *) p)->attributes, name);
      if (XkStrEqual (v, value))
      {
	r = (Element *) p;
      }
      else r = NodeChildLookup (p, name, value);
    }
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetNodeType
 * Description...: Returns the type of the Node
 *
 * In............: Node *: the Node
 * Out...........: NodeType: 
 * Precondition..: the Node must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

NodeType NodeGetNodeType (Node *p)
{
  return p->type;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetParentNode
 * Description...: Returns the parent of a Node
 *
 * In............: Node *: the Node
 * Out...........: Node *: the parent of the Node
 * Precondition..: the Node must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Element *NodeGetParentNode (Node *p)
{
  return p->parent; 
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetLastChild
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: Node* :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node* NodeGetLastChild (Node *node)
{
  Node *r;
  Element *e;

  r = NULL;
  if (node->type == ELEMENT_NODE)
  {
    e = (Element *) node;
    for (r = e->contents; r != NULL && r->next != NULL; r = r->next);
  }
  else if (node->type == DOCUMENT_NODE)
  {
    r = (Node *)((Document *) node)->root;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetFirstChild
 * Description...: 
 *                 
 * In............: Node *
 * Out...........: Node* 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node* NodeGetFirstChild (Node *node)
{
  Node *r;

  r = NULL;
  if (node->type == ELEMENT_NODE)
  {
    r = ((Element *) node)->contents;
  }
  else if (node->type == DOCUMENT_NODE)
  {
    r = (Node *)((Document *) node)->root;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeHasChildren
 * Description...: Checks if a Node has children
 *
 * In............: Node *: the Node
 * Out...........: int: 1, if the child list is not NULL
 * Precondition..: the Node must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int NodeHasChildNodes (Node *node)
{
  int r;

  r = 0;
  if ((node->type == ELEMENT_NODE && ((Element *) node)->contents != NULL) ||
    (node->type == DOCUMENT_NODE && ((Document *) node)->root != NULL))
  {
    r = 1;
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeAppendChild
 * Description...: 
 *                 
 * In............: Node *:
 *                 Node *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NodeAppendChild (Node *b, Node *p)
{
  Element *e;

  if (b->type == ELEMENT_NODE)
  {
    setOwnerDocumentModified (b, 1);
    e = (Element *) b;
    if (e != NULL)
    {
      p->next = NULL;
      p->parent = (Element *) b;
      if (e->contents != NULL)
      {
	NodeGetLastChild (b)->next = p;
      }
      else
      {
	e->contents = p;
      }
      if (p->type == ELEMENT_NODE)
      {
	ElementSetInherited ((Element *) p);
      }
    }
  }
  else if (b->type == DOCUMENT_NODE && p->type == ELEMENT_NODE)
  {
    ((Document *) b)->modified = 1;
    ((Document *) b)->root = (Element *) p;
    ((Element *) p)->document = (Document *) b;
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeInsertBefore
 * Description...: Inserts a new element before a given element
 *
 * In............: Node *: the Node
 *                 Node *: element before the new element will be inserted
 *                 Node *: the new element
 * Out...........: -
 * Precondition..: the parent and the child element must be allocated
 * Postcondition.: the child element is inserted into the list of children
 *-----------------------------------------------------------------------*/

void NodeInsertBefore (Node *node, Node *refChild, Node *newChild)
{
  Node *p, *last, *next;
  Element *e;

  assert (node != NULL);
  if (node->type == ELEMENT_NODE)
  {
    setOwnerDocumentModified (node, 1);
    e = (Element *) node;
    last = NULL;
    for (next = e->contents;
	 next != NULL && next != refChild;
	 next = next->next)
    {
      last = next;
    }

    if (last != NULL)
    {
      last->next = newChild;
    }
    else
    {
      e->contents = newChild;
    }

    last = NULL;
    for (p = newChild; p != NULL; p = p->next)
    {
      p->parent = e;

      if (p->type == ELEMENT_NODE)
      {
	ElementSetInherited ((Element *) p);
      }

      if (p->next == NULL)
      {
	last = p;
      }
    }

    if (last != NULL)
    {
      last->next = next;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeReplaceChild
 * Description...: Replaces a child element
 *
 * In............: Node *: the Node
 *                 Node *: the old child
 *                 Node *: the new child
 * Out...........: Node *: the old child
 * Precondition..: the parent and child element must be allocated
 * Postcondition.: the element is replaced
 *-----------------------------------------------------------------------*/

Node *NodeReplaceChild (Node *node, Node *oldChild, Node *newChild)
{
  Node *p, *last;
  Element *e;

  assert (node != NULL);
  if (node->type == ELEMENT_NODE)
  {
    setOwnerDocumentModified (node, 1);
    e = (Element *) node;
    last = NULL;
    for (p = e->contents; p != NULL && p != oldChild; p = p->next)
    {
      last = p;
    }
    if (p != NULL)
    {
      newChild->next = oldChild->next;
      if (last != NULL)
      {
	last->next = newChild;
      }
      else
      {
	e->contents = newChild;
      }
      newChild->parent = e;
      if (newChild->type == ELEMENT_NODE)
      {
	ElementSetInherited ((Element *) newChild);
      }
    }
  }
  return oldChild;
}

/*-------------------------------------------------------------------------
 * Function......: NodeRemoveChild
 * Description...: Removes a child from the list of children 
 *
 * In............: Node *: the Node
 *                 Node *: the element to be removed
 * Out...........: Node *: the element to be removed
 * Precondition..: the Node must be allocated
 * Postcondition.: the element at the given position is removed
 *-----------------------------------------------------------------------*/

Node *NodeRemoveChild (Node *node, Node *child)
{
  Node *previous;
  Element *e;

  setOwnerDocumentModified (node, 1);

  if (child->type == ELEMENT_NODE)
  {
    e = (Element *) child;
    if (e->parent != NULL && e->parent->line == e->line)
    {
      e->line = NULL;
    }
  }

  previous = NodeGetPreviousSibling (child);
  if (previous != NULL)
  {
    previous->next = child->next;
  }
  else if (child->parent != NULL)
  {
    child->parent->contents = child->next;
  }

  child->next = NULL;
  child->parent = NULL;

  return child;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetOwnerDocument
 * Description...: 
 *                 
 * In............: Node *
 * Out...........: Document *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Document *NodeGetOwnerDocument (Node *node)
{
  while (node->parent != NULL)
  {
    node = (Node *) node->parent;
  }

  return (Document *) ((Element *) node)->document;
}

/*-------------------------------------------------------------------------
 * Function......: NodeGetRange
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *NodeGetRange (Node *first)
{
  Text *current;
  char *text;

  text = XkNew (char);
  text[0] = '\0';
  while (first != NULL && first->type == TEXT_NODE)
  {
    current = (Text *) first;
    text = realloc (text, strlen (text) + strlen (current->text) + 1);
    strcat (text, current->text);
    first = first->next;
  }
  return text;
}

/*-------------------------------------------------------------------------
 * Function......: NodeDeleteRange
 * Description...: 
 *                 
 * In............: Node *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NodeDeleteRange (Node *first)
{
  Node *current;

  setOwnerDocumentModified (first, 1);
  current = first;
  while (current != NULL && current->type == TEXT_NODE)
  {
    current = first->next;
    NodeDelete (first);
    first = current;
  }
}

/*-------------------------------------------------------------------------
 * Function......: NodeChangeRange
 * Description...: 
 *                 
 * In............: Node *:
 *                 char *:
 * Out...........: Node *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *NodeChangeRange (Node *first, char *text)
{
  Parser *p;
  Element *b;
  Node *next;

  setOwnerDocumentModified (first, 1);
  next = first;
  while (next != NULL && next->type == TEXT_NODE)
  {
    next = next->next;
  }
  p = ParserNew ();
  p->document = first->parent->document;
  p->element = ParserStartElement (p, NULL);
  p->element->parent = first->parent;
  b = ParserProcessData (p, "text/html", text);
  NodeDeleteRange (first);
  NodeInsertBefore ((Node *) b->parent, next, b->contents);
  return b->contents;
}

/*-------------------------------------------------------------------------
 * Function......: ElementGetTagName
 * Description...: 
 *                 
 * In............: Element *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *ElementGetTagName (Element *e)
{
  return e->tag;
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetInherited
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetInherited (Element *element)
{
  Node *p;

  assert (element->parent != NULL);

  element->document = element->parent->document;
  element->color = element->parent->color;
  element->font_size = element->parent->font_size;
  element->font_family = element->parent->font_family;
  element->font_weight = element->parent->font_weight;
  element->font_slant = element->parent->font_slant;
  element->text_align = element->parent->text_align;
  element->underline = element->parent->underline;

  for (p = element->contents; p != NULL; p = p->next)
  {
    if (p->type == ELEMENT_NODE)
    {
      ElementSetInherited ((Element *) p);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetContents
 * Description...: Sets the textual contents of a Element. The
 *                 contents is stored in a single Text
 *                 element. Any previous contents is deleted.
 *
 * In............: Element *: the Element
 *                 char *: the new contents
 * Out...........: -
 * Precondition..: the Element must be allocated
 * Postcondition.: the contents is allocated
 *-----------------------------------------------------------------------*/

void ElementSetContents (Element *element, char *contents)
{
  Text *help;

  setOwnerDocumentModified ((Node *) element, 1);
  while(element->contents)
  {
    NodeDelete(element->contents);
  }
  element->contents=(Node*)TextNew();
  (Node*)help=element->contents;
  help->text = XkNewString(contents);
  help->parent = element;
}

/*-------------------------------------------------------------------------
 * Function......: ElementGetElementsByTagName
 * Description...: 
 *                 
 * In............: Element *
 *                 char *
 * Out...........: NodeList *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeList *ElementGetElementsByTagName (Element *e, char *tag)
{
  NodeList *l;

  l = NodeListNew ();

  searchSubtree (e, tag, l);

  return l;
}

/*-------------------------------------------------------------------------
 * Function......: ElementCalculateWidth
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementCalculateWidth (Element *b)
{
  int w;

  if (b->maxWidth < b->minWidth) b->maxWidth = b->minWidth;

  if (b->parent != NULL)
  {
    if (b->maxWidth == b->minWidth)
    {
      w = b->minWidth;
    }
    else
    {
      if (b->maxWidth < b->minWidth * 3)
      {
	w = b->maxWidth;
      }
      else
      {
	w = b->minWidth * 3;
      }
    
      w = min (b->parent->width / 2, w);
    }

    b->width = max (b->minWidth, w);
  }

  /* we are clueless if there is no parent! */
}

/*-------------------------------------------------------------------------
 * Function......: ElementLeftMargin
 * Description...: 
 *                 
 * In............: Element *:
 *                 int :
 * Out...........: int :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int ElementLeftMargin (Element *element, int y)
{
  Node *p;
  int m;

  m = ElementTotalLeft (element);
  for (p = element->contents; p != NULL; p = p->next)
  {
    if (p->type == ELEMENT_NODE &&
      ((Element *) p)->positioned &&
      ((Element *) p)->floating == FLOAT_LEFT &&
      ((Element *) p)->y <= y &&
      y < ((Element *) p)->y + ((Element *) p)->height)
    {
      m = max (m, ((Element *) p)->x + ((Element *) p)->width);
    }
  }
  return m;
}

/*-------------------------------------------------------------------------
 * Function......: ElementRightMargin
 * Description...: 
 *                 
 * In............: Element *:
 *                 int :
 * Out...........: int :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int ElementRightMargin (Element *element, int y)
{
  Node *p;
  int m;

  m = element->width - ElementTotalRight (element);
  for (p = element->contents; p != NULL; p = p->next)
  {
    if (p->type == ELEMENT_NODE &&
      ((Element *) p)->positioned &&
      ((Element *) p)->floating == FLOAT_RIGHT &&
      ((Element *) p)->y <= y &&
      y < ((Element *) p)->y + ((Element *) p)->height)
    {
      m = min (m, ((Element *) p)->x);
    }
  }
  return m;
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

int ElementTotalTop (Element *element)
{
  return element->margin_top.effective +
    element->border_top_width.effective +
    element->padding_top.effective;
}

int ElementTotalRight (Element *element)
{
  return element->margin_right.effective +
    element->border_right_width.effective +
    element->padding_right.effective;

}

int ElementTotalLeft (Element *element)
{
  return element->margin_left.effective +
    element->border_left_width.effective +
    element->padding_left.effective;

}

int ElementTotalBottom (Element *element)
{
  return element->margin_bottom.effective +
    element->border_bottom_width.effective +
    element->padding_bottom.effective;
}

/*-------------------------------------------------------------------------
 * Function......: ElementGetContents
 * Description...: Returns the textual contents of a Element. Any
 *                 non-textual context is ignored. The result is a new
 *                 allocated string which has to be freed by the caller
 *
 * In............: Element *: the Element
 * Out...........: char *: the contents in a newly allocated string or
 *                 NULL if the element has no contents
 * Precondition..: the Element must be allocated
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

char *ElementGetContents (Element *element)
{
  Node * help;
  char *r;
  int L;
  r = NULL;
  L = 0;
  if (element->contents)
  {
    help=element->contents;
    while(help)
    {
      if(help->type==TEXT_NODE)
      {
	r = realloc(r, L+strlen(((Text*)help)->text) + 1);
	r[L] = 0;
	strcat(r, ((Text*)help)->text);
	L += strlen(((Text*)help)->text);
      }
      else if(help->type==CDATA_SECTION_NODE)
      {
	r = realloc(r, L+strlen(((CDATASection*)help)->text) + 1);
	r[L] = 0;
	strcat(r, ((CDATASection*)help)->text);
	L += strlen(((CDATASection*)help)->text);
      }

      help=help->next;
    }
  }
  return r;

}

/*-------------------------------------------------------------------------
 * Function......: ElementGetAttribute
 * Description...: 
 *                 
 * In............: Element *:
 *                 char *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *ElementGetAttribute (Element *element, char *attrib)
{
  char *r;

  if (XkStrEqual (attrib, "value") && 
    element->contents != NULL && element->contents->type == INSET_NODE &&
    ((Inset *) element->contents)->getValue != NULL)
  {
    r = ((Inset *) element->contents)->getValue (((Inset *) element->contents)->inset);
  }
  else
  {
    r = AttributeGetValue (element->attributes, attrib);
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetAttribute
 * Description...: 
 *                 
 * In............: Element *:
 *                 char *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetAttribute (Element *element, char *attrib, char *value)
{
  Attribute *a, *help;
  int found;

/*   setOwnerDocumentModified ((Node *) element, 1); */

  if (XkStrEqual (attrib, "value") && 
    element->contents != NULL && element->contents->type == INSET_NODE &&
    ((Inset *) element->contents)->setValue != NULL)
  {
    ((Inset *) element->contents)->setValue (((Inset *) element->contents)->inset,
      value);
  }

  help = element->attributes;
  found = 0;
  while (help && !found) 
  {
    if (!XkStrEqual (help->name, attrib))
    {
      help = help->next;
    } else
    {
      found = 1;
    }
  }
  if (found) 
  {
    XkFree(help->value);
    help->value = XkNewString(value);
  } else 
  {
    a = AttributeNew ();
    a->name = XkNewString (attrib);
    a->value = XkNewString (value);
    a->next = element->attributes;
    element->attributes = a;
  }
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetTagName
 * Description...: 
 *                 
 * In............: Element *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetTagName (Element *element, char *value)
{
  setOwnerDocumentModified ((Node *) element, 1);
  if (element->tag != NULL) XkFree (element->tag);
  element->tag = XkNewString (value);
  ElementSetDefaultAttributes (element);
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetNSPrefix
 * Description...: 
 *                 
 * In............: Element *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetNSPrefix (Element *element, char *p)
{
  if (element->namespace != NULL) XkFree (element->namespace);
  element->namespace = XkNewString (p);
}

/*-------------------------------------------------------------------------
 * Function......: ElementGetNSPrefix
 * Description...: 
 *                 
 * In............: Element *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *ElementGetNSPrefix (Element *element)
{
  return element->namespace;
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetDefaultAttributes
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetDefaultAttributes (Element *element)
{
  Attribute *d, *a;

  d = DocumentGetDefaultAttributes (element->document, element->tag);
  while (d != NULL)
  {
    if (!AttributeExists (element->attributes, d->name))
    {
      a = AttributeNew ();
      a->name = XkNewString (d->name);
      a->value = XkNewString (d->value);
      a->next = element->attributes;
      element->attributes = a;
    }
    d = d->next;
  }
}

/*-------------------------------------------------------------------------
 * Function......: ElementMatch
 * Description...: 
 *                 
 * In............: Element *
 *                 Element *
 * Out...........: NodeList *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeList *ElementMatch (Element *source, Element *pattern)
{
  NodeList *l;

  l = NodeListNew ();

  recursiveMatch (source, pattern, l);

  return l;
}

int recursiveMatch (Element *s, Element *p, NodeList *result)
{
  NodeList *l;
  int r, i;
  Node *n;
  char buffer[32];

  r = 0;
  l = NodeListNew ();

  searchSubtree (s, p->tag, l);
  for (i = 0; i < l->length; i++)
  {
    r = 1;
    NodeListAppendItem (result, l->nodes[i]);
    for (n = p->contents; n != NULL; n = n->next)
    {
      if (n->type == ELEMENT_NODE &&
	!XkStrEqual (((Element *) n)->tag, "func") &&
	!XkStrEqual (((Element *) n)->tag, "var"))
      {
	r = r && recursiveMatch (
	  (Element *) l->nodes[i],
	  (Element *) n,
	  result);
      }
    }

    if (!r)
    {
      NodeListRemoveItem (result, l->nodes[i]);
      ElementSetAttribute (p, "peer", "");
    }
    else
    {
      sprintf (buffer, "%p", l->nodes[i]);
      ElementSetAttribute (p, "peer", buffer);
    }
  }

  NodeListDelete (l);

  return r;
}

void searchSubtree (Element *s, char *tag, NodeList *result)
{
  Node *n;

  if ((tag[0] == '*' && tag[1] == '\0') || XkStrEqual (s->tag, tag))
  {
    NodeListAppendItem (result, (Node *) s);
  }

  for (n = s->contents; n != NULL; n = n->next)
  {
    if (n->type == ELEMENT_NODE)
    {
      searchSubtree ((Element *) n, tag, result);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: DocumentAddDefaultAttribute
 * Description...: 
 *                 
 * In............: Document *:
 *                 char *:
 *                 char *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void DocumentAddDefaultAttribute (Document *document, char *tag, 
  char *name, char *value)
{
  ElementDeclaration *info;
  Attribute *a;

  info = document->elements;
  while (info != NULL && !XkStrEqual (info->name, tag))
  {
    info = info->next;
  }

  if (info == NULL)
  {
    info = XkNew (ElementDeclaration);
    info->name = XkNewString (tag);
    info->attributes = NULL;
    info->next = document->elements;
    document->elements = info;
  }

  if (AttributeGetValue (info->attributes, name) == NULL)
  {
    a = AttributeNew ();
    a->name = XkNewString (name);
    a->value = XkNewString (value);
    a->next = info->attributes;
    info->attributes = a;
  }
}

/*-------------------------------------------------------------------------
 * Function......: DocumentAddRuleSet
 * Description...: 
 *                 
 * In............: Document *:
 *                 char *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void DocumentAddRuleSet (Document *document, char *selector, char *declaration)
{
  RuleSet *r;

  r = XkNew (RuleSet);
  r->selector = XkNewString (selector);
  r->declaration = XkNewString (declaration);
  r->next = document->cssRules;
  document->cssRules = r;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentGetDefaultAttributes
 * Description...: 
 *                 
 * In............: Document *:
 *                 char *:
 * Out...........: Attribute *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Attribute *DocumentGetDefaultAttributes (Document *document, char *tag)
{
  Attribute *a;
  ElementDeclaration *info;
  int found;

  a = NULL;
  found = 0;
  if (document)
  {
    info = document->elements;
    while (!found && info != NULL)
    {
      if (XkStrEqual (info->name, tag))
      {
	a = info->attributes;
	found = 1;
      }
      else
      {
	info = info->next;
      }
    }
  }
  return a;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentGetDefaultAttribute
 * Description...: 
 *                 
 * In............: Document *
 *                 char *
 *                 char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *DocumentGetDefaultAttribute (Document *document, char *tag,
  char *name)
{
  char *a;
  ElementDeclaration *info;
  int found;

  a = NULL;
  found = 0;
  if (document)
  {
    info = document->elements;
    while (!found && info != NULL)
    {
      if (XkStrEqual (info->name, tag))
      {
	a = AttributeGetValue (info->attributes, name);
	found = 1;
      }
      else
      {
	info = info->next;
      }
    }
  }
  return a;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentSetRoot
 * Description...: 
 *                 
 * In............: Document *
 *                 Element *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void DocumentSetRoot (Document *d, Element *r)
{
  d->modified = 1;
  d->root = r;
}

Element *DocumentGetRoot (Document *d)
{
  return d->root;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentGetNonanonymousRoot
 * Description...: 
 *                 
 * In............: Document *
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *DocumentGetNonanonymousRoot (Document *d)
{
  Node *r;

  r = (Node *) d->root;
  while (r != NULL &&
    !(r->type == ELEMENT_NODE && ((Element *) r)->anonymous == ANON_NORMAL))
  {
    r = NodeGetSuccessor (r);
  }
  return (Element *) r;
}

/*-------------------------------------------------------------------------
 * Function......: DocumentGetElementsByTagName
 * Description...: 
 *                 
 * In............: Document *
 *                 char *
 * Out...........: NodeList *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeList *DocumentGetElementsByTagName (Document *d, char *tag)
{
  NodeList *l;

  l = NodeListNew ();

  searchSubtree (d->root, tag, l);

  return l;
}

/*-------------------------------------------------------------------------
 * Function......: TextSetText
 * Description...: 
 *                 
 * In............: Text *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void TextSetData (Text *p, char *value)
{
  char *c;

  setOwnerDocumentModified ((Node *) p, 1);
  if (p->text != NULL) XkFree (p->text);
  p->text = XkNewString (value);
  p->whitespace = 1;
  for (c = p->text; *c != '\0' && p->whitespace; c++)
  {
    if (!isspace (*c)) p->whitespace = 0;
  }
}

/*-------------------------------------------------------------------------
 * Function......: TextGetText
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *TextGetData (Text *p)
{
  return p->text;
}

/*-------------------------------------------------------------------------
 * Function......: CDATASectionSetData
 * Description...: 
 *                 
 * In............: CDATASection *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void CDATASectionSetData (CDATASection *p, char *value)
{
  setOwnerDocumentModified ((Node *) p, 1);
  if (p->text != NULL) XkFree (p->text);
  p->text = XkNewString (value);
}

/*-------------------------------------------------------------------------
 * Function......: CDATASectionGetData
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *CDATASectionGetData (CDATASection *p)
{
  return p->text;
}

/*-------------------------------------------------------------------------
 * Function......: NodeListNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: NodeList *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeList *NodeListNew (void)
{
  NodeList *r;

  r = XkNew (NodeList);
  r->nodes = malloc (sizeof (Node *) * 64);
  r->length = 0;
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NodeListDelete
 * Description...: 
 *                 
 * In............: NodeList *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NodeListDelete (NodeList *l)
{
  if (l->nodes != NULL) XkFree (l->nodes);
  XkFree (l);
}

/*-------------------------------------------------------------------------
 * Function......: NodeListAppendItem
 * Description...: 
 *                 
 * In............: NodeList *
 *                 Node *
 * Out...........: int 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int NodeListAppendItem (NodeList *l, Node *n)
{
  l->nodes[l->length] = n;
  l->length++;

  if (l->length % 64 == 0)
  {
    l->nodes = realloc (l->nodes, sizeof (Node *) * (l->length + 64));
  }

  return l->length - 1;
}

/*-------------------------------------------------------------------------
 * Function......: NodeListGetItem
 * Description...: 
 *                 
 * In............: NodeList *
 *                 int
 * Out...........: Node *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *NodeListGetItem (NodeList *l, int i)
{
  return i < l->length ? l->nodes[i] : NULL;
}

/*-------------------------------------------------------------------------
 * Function......: NodeListRemoveItem
 * Description...: 
 *                 
 * In............: NodeList *
 *                 Node *
 * Out...........: Node *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Node *NodeListRemoveItem (NodeList *l, Node *n)
{
  int i;

  for (i = 0; i < l->length; i++)
  {
    if (l->nodes[i] == n)
    {
      memmove (&l->nodes[i + 1], &l->nodes[i], l->length - i - 1);
      l->length--;
    }
  }

  return n;
}

/*-------------------------------------------------------------------------
 * Function......: NodeListGetLength
 * Description...: 
 *                 
 * In............: NodeList *
 * Out...........: int 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int NodeListGetLength (NodeList *l)
{
  return l->length;
}

/*-------------------------------------------------------------------------
 * Function......: Iterator functions
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeIterator *NodeIteratorNew (Element *start,
  char *name, char *id, char *tag, char *class)
{
  NodeIterator *r;

  r = XkNew (NodeIterator);
  r->start = start;
  r->current = NULL;
  r->tag = XkNewString (tag);
  r->id = XkNewString (id);
  r->name = XkNewString (name);
  r->class = XkNewString (class);

  return r;
}

void NodeIteratorDelete (NodeIterator *i)
{
  if (i->name != NULL) XkFree (i->name);
  if (i->tag != NULL) XkFree (i->tag);
  if (i->id != NULL) XkFree (i->id);
  if (i->class != NULL) XkFree (i->class);
}

Element *NodeIteratorNext (NodeIterator *i)
{
  Element *r;
  Node *p;

  r = NULL;
  if (i->current == NULL) p = (Node *) i->start;
  else p = (Node *) i->current;

  p = successor (p);
  while (p != NULL && p->type != ELEMENT_NODE) p = successor (p);
  i->current = (Element *) p;

  while (i->current != NULL && r == NULL)
  {
    p = (Node *) i->current;
    while (p != NULL && p->type != ELEMENT_NODE) p = successor (p);
    i->current = (Element *) p;

    if (i->current != NULL && i->current != i->start)
    {
      if (iteratorMatch (i))
      {
	r = i->current;
      }
      else
      {
	i->current = (Element *) successor ((Node *) i->current);
      }
    }
  }
  return r;
}

Element *NodeIteratorPrev (NodeIterator *i)
{
  return NULL;
}

Element *NodeIteratorFirst (NodeIterator *i)
{
  i->current = NULL;
  return NodeIteratorNext (i);
}

Element *NodeIteratorLast (NodeIterator *i)
{
  Element *last;

  i->current = NULL;
  do
  {
    last = i->current;
  }
  while (NodeIteratorNext (i) != NULL);
  i->current = last;
  return i->current;
}

int iteratorMatch (NodeIterator *i)
{
  int r;
  char *name;
  char *class;
  char *id;

  if (i->current != NULL)
  {
    r = 1;
    if (i->name != NULL && i->name [0] != '\0')
    {
      name = AttributeGetValue (i->current->attributes, "NAME");
      if (!XkStrEqual (i->name, name)) r = 0;
    }
    if (r && i->class != NULL && i->class [0] != '\0')
    {
      class = AttributeGetValue (i->current->attributes, "CLASS");
      if (!XkStrEqual (i->class, class)) r = 0;
    }
    if (r && i->id != NULL && i->id [0] != '\0')
    {
      id = AttributeGetValue (i->current->attributes, "ID");
      if (!XkStrEqual (i->id, id)) r = 0;
    }
    if (r && i->tag != NULL && i->tag [0] != '\0')
    {
      if (!XkStrEqual (i->current->tag, i->tag)) r = 0;
    }
  }
  else
  {
    r = 0;
  }

  return r;
}

Node *successor (Node *p)
{
  Node *r;

  r = NULL;
  if (p->type == ELEMENT_NODE && ((Element *) p)->contents != NULL)
  {
    r = ((Element *) p)->contents;
  }
  else if (p->next != NULL)
  {
    r = p->next;
  }
  else
  {
    r = (Node *) p->parent;
    while (r != NULL && r->next == NULL)
    {
      r = (Node *) r->parent;
    }
    if (r != NULL)
    {
      r = r->next;
    }
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: ElementDeclarationDelete
 * Description...: 
 *                 
 * In............: ElementDeclaration *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementDeclarationDelete (ElementDeclaration *e)
{
  ElementDeclaration *element;

  while (e != NULL)
  {
    element = e;
    e = e->next;
    if (element->name != NULL) XkFree (element->name);
    AttributeDelete (element->attributes);
    XkFree (element);
  }
}

/*-------------------------------------------------------------------------
 * Function......: setOwnerDocumentModified
 * Description...: 
 *                 
 * In............: Node *
 *                 int
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void setOwnerDocumentModified (Node *node, int modified)
{
  if (node->parent != NULL && node->parent->document != NULL)
  {
    node->parent->document->modified = 1;
  }
}
