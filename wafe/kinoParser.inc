/* $Id: kinoParser.inc,v 1.7 1999/03/02 12:26:00 koeppen Exp $ */
/* -*- C -*- */

#include "KinoParser.h"

Tcl_HashTable kinoParserTable;  /* for stringToP and friends */

static void wafeTagHandler (HandlerInfo *);
static void wafeLinkHandler (HandlerInfo *);
static void wafeScriptHandler (HandlerInfo *);
static void wafeEventHandler (HandlerInfo *);
static void getElementsByTagName (Node *, char *, wafeString);

/*-------------------------------------------------------------------------
 * Function......: DocumentSetHandlers
 * Description...: 
 *                 
 * In............: Document *
 *                 char *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void DocumentSetHandlers (Document *d,
  char *tagHandler, char *linkHandler, char *eventHandler)
{
  DocumentAddTagHandler (d, wafeTagHandler, XkNewString (tagHandler));
  DocumentAddLinkHandler (d, wafeLinkHandler, XkNewString (linkHandler));
  DocumentAddEventHandler (d, wafeEventHandler,
    XkNewString (eventHandler));
}

/*-------------------------------------------------------------------------
 * Function......: wafeTagHandler
 * Description...: 
 *                 
 * In............: TagHandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void wafeTagHandler (HandlerInfo *info)
{
  char cmd[256];
  
  Tcl_GlobalEval (wafeInterpreter, "unset handlerInfo");

  sprintf (cmd, "set handlerInfo(document) \"%s\"", 
    wafePtrToString (info->document, "Node_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(parser) \"%s\"", 
    wafePtrToString (info->info.tag->parser, "Parser_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(name) %s", info->info.tag->name);
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(empty) %d", info->info.tag->empty);
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(element) \"%s\"", 
    wafePtrToString (info->info.tag->parser->element, "Node_",
      kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  Tcl_GlobalEval (wafeInterpreter, info->userData);
}

void wafeLinkHandler (HandlerInfo *info)
{
  char cmd[256];
  
  Tcl_GlobalEval (wafeInterpreter, "unset handlerInfo");

  sprintf (cmd, "set handlerInfo(document) \"%s\"", 
    wafePtrToString (info->document, "Node_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(element) \"%s\"", 
    wafePtrToString (info->info.link->element, "Node_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(parser) \"%s\"", 
    wafePtrToString (info->info.link->parser, "Parser_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  Tcl_GlobalEval (wafeInterpreter, info->userData);
}

void wafeEventHandler (HandlerInfo *info)
{
  Tcl_GlobalEval (wafeInterpreter, info->userData);
}

char *NodeGetNodeTypeString (Node *node)
{
  char *r;

  r = NULL;
  switch (node->type)
  {
    case TEXT_NODE: r = "TEXT_NODE";
      break;
    case INSET_NODE: r = "INSET_NODE";
      break;
    case ELEMENT_NODE: r = "ELEMENT_NODE";
      break;
    case LINEBREAK_NODE: r = "LINEBREAK_NODE";
      break;
    case DOCUMENT_NODE: r = "DOCUMENT_NODE";
      break;
    case PROCESSING_INSTRUCTION_NODE: r = "PROCESSING_INSTRUCTION_NODE";
      break;
  }
  return r;
}

void ElementSetAnon (Element *box, char *state)
{
  if (XkStrEqual (state, "ANON_NORMAL")) box->anonymous = ANON_NORMAL;
  else if (XkStrEqual (state, "ANON_CONT")) box->anonymous = ANON_CONT;
  else if (XkStrEqual (state, "ANON_BOX")) box->anonymous = ANON_BOX;
  else if (XkStrEqual (state, "ANON_DELIMTER"))
    box->anonymous = ANON_DELIMITER;
}

void InsetSetInset (Inset *p, Widget i)
{
  p->inset = (Inset *) i;
}

Widget InsetGetInset (Inset *p)
{
  return (Widget) ((Inset *) p->inset);
}

void TextSetWrap (Text *p, Boolean value)
{
  p->wrap = value;
}

void ElementSaveRawContents (Element *box)
{
  box->saveRawContents = True;
}

char *ElementGetRawContents (Element *box)
{
  return box->rawContents;
}

Node *PointerToNode (int p)
{
  return (Node *) p;
}

int ParserReleased (Parser *parser)
{
  return (parser->holdCount == 0);
}

char *ParserGetHandling (Parser *parser)
{
  char *r;

  switch (parser->handling)
  {
    case HANDLE_EMBED: r = "HANDLE_EMBED"; break;
    case HANDLE_NEW: r = "HANDLE_NEW"; break;
    case HANDLE_REPLACE: r = "HANDLE_REPLACE"; break;
    case HANDLE_ENTITY: r = "HANDLE_ENTITY"; break;
    case HANDLE_DOCTYPE: r = "HANDLE_DOCTYPE"; break;
    default: r = "";
  }
  return r;
}

Element *ParserStartElementVA (Parser *p, char *tag, int argc, char **argv)
{
  TagHandlerInfo *info;
  Attribute *a;
  Element *b;

  info = TagHandlerInfoNew ();
  info->name = XkNewString (tag);
  while (argc > 0)
  {
    a = XkNew (Attribute);
    a->name = XkNewString (argv[argc - 1]);
    a->value = XkNewString (argv[argc - 2]);
    a->next = info->attributes;
    info->attributes = a;
    argc -= 2;
  }
  
  b = ParserStartElement (p, info);

  TagHandlerInfoDelete (info);

  return b;
}

Element *ParserFinishElementVA (Parser *p, char *tag, int argc, char **argv)
{
  TagHandlerInfo *info;
  Attribute *a;
  Element *b;

  info = TagHandlerInfoNew ();
  info->name = XkNewString (tag);
  while (argc > 0)
  {
    a = XkNew (Attribute);
    a->name = XkNewString (argv[argc - 1]);
    a->value = XkNewString (argv[argc - 2]);
    a->next = info->attributes;
    info->attributes = a;
    argc -= 2;
  }

  b = ParserFinishElement (p, info);

  TagHandlerInfoDelete (info);

  return b;
}

char *ElementGetAttributes (Element *b)
{
  Attribute *a;
  int argc;
  char **argv;
  char *aargv[2];
  char *r;
  
  r = NULL;
  argc = 0;
  for (a = b->attributes; a != NULL; a = a->next)
  {
    argc++;
  }
  if (argc > 0)
  {
    argv = malloc (sizeof (char *) * argc);
    argc = 0;
    for (a = b->attributes; a != NULL; a = a->next)
    {
      aargv[0] = a->name;
      aargv[1] = a->value;
      argv[argc] = Tcl_Merge (2, aargv);
      argc++;
    }
    r = Tcl_Merge (argc, argv);
    while (argc > 0)
    {
      argc--;
      Tcl_Free (argv[argc]);
    }
  }
  return r;
}

void ElementSetAttributes (Element *box, char *list)
{
  int i, r;
  int argc;
  char **argv;
  int attributeArgc;
  char **attributeArgv;
  Attribute *a, *h;

  r = Tcl_SplitList (wafeInterpreter, list, &argc, &argv);
  if (r == TCL_OK)
  {
    a = box->attributes; 
    while (a != NULL)
    {
      h = a;
      a = a->next;

      if (h->name != NULL) XkFree (h->name);
      if (h->value != NULL) XkFree (h->value);
      XkFree (h);
    }
    box->attributes = NULL;

    for (i = 0; i < argc; i++)
    {
      r = Tcl_SplitList (wafeInterpreter, argv[i],
	&attributeArgc, &attributeArgv);
      if (r == TCL_OK && attributeArgc == 2)
      {
	a = XkNew (Attribute);
	a->name = XkNewString (attributeArgv[0]);
	a->value = XkNewString (attributeArgv[1]);
	a->next = box->attributes;
	box->attributes = a;
	Tcl_Free ((char *) attributeArgv);
      }
      else
      {
	wafeWarn ("ElementSetAttributes", 
	  "Failed to parse attribute: %s", argv[i], NULL, NULL);
      }
    }
    Tcl_Free ((char *) argv);
  }
  else
  {
    wafeWarn ("ElementSetAttributes", 
      "Failed to parse attribute list: %s", list, NULL, NULL);
  }
}

void EntityAdd (Parser *parser, char *value)
{
  Entity *entity;

  entity = XkNew (Entity);
  entity->name = XkNewString (parser->entityName);
  entity->value = XkNewString (value);
  entity->next = parser->document->entities;
  parser->document->entities = entity;

  if (parser->parent != NULL && parser->holdCount == 0) 
  {
    ParserRelease (parser->parent);
  }
}

/*
 * Local Variables:
 * mode: c
 * End:
 */
