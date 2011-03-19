/*-------------------------------------------------------------------------
 * kinoahd - XML scripting for the Kino XML/CSS processor
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
 * File..........: AHDRuntime.c
 * Description...: 
 *
 * $Id: AHDRuntime.c,v 1.4 1999/07/12 12:10:09 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <sys/utsname.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "KinoParser.h"

#ifdef NATIVE_WIDGET
#include "Visual.h"
#endif

#ifdef NATIVE_NETWORKING
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include "Net.h"
#endif

#ifdef HAVE_LIBLUA
#include <lua.h>

#ifdef NATIVE_SCRIPTING
static int luaWrapperInit = 0;

extern void lua_userinit (void);
extern void kinoparserlua_init (void);
extern void kinoahdlua_init (void);

#ifdef HAVE_LIBLOADLIB
extern void loadlib_open (void);
#endif

#ifdef HAVE_LIBPOSLIB
extern void poslib_open (void);
#endif

#ifdef NATIVE_WIDGET
extern void kinowidgetlua_init (void);
#endif
#endif
#endif

#ifdef HAVE_LIBTCL8_0
#include <tcl.h>

Tcl_Interp *tclInterp;

#ifdef NATIVE_SCRIPTING
extern void Kinoparser_Init (Tcl_Interp *);
extern void Kinoahd_Init (Tcl_Interp *);

#ifdef NATIVE_WIDGET
extern void Kinowidget_Init (Tcl_Interp *);
#endif
#endif
#endif

#include "AHDRuntime.h"

/* local definitions */

#define HEX2INT(c) (c >= '0' && c <= '9' ? c - '0' : \
  (c >= 'a' && c <= 'f' ? c - 'a' + 10 : \
    (c >= 'A' && c <= 'F' ? c - 'A' + 10 : 0)))

#define DEBUG

/* local functions */

#ifdef NATIVE_SCRIPTING
extern SWIG_MakePtr(char *buffer, void *ptr, char *typestring);
static void scriptHandler (HandlerInfo *);
#endif

static void readerLock (AHDRuntime *);
static void readerUnlock (AHDRuntime *);

static void writerLock (AHDRuntime *);
static void writerUnlock (AHDRuntime *);

static void tagHandler (HandlerInfo *);
static void eventHandler (HandlerInfo *);
static void linkHandler (HandlerInfo *);

static void callScriptHandler (AHDRuntime *, struct _ScriptHandlerInfo *);
static void callTagHandler (AHDRuntime *, struct _TagHandlerInfo *);
static void callEventHandler (AHDRuntime *, struct _EventHandlerInfo *);
static void callLinkHandler (AHDRuntime *, struct _LinkHandlerInfo *);

static Element *attributeParentLookup (Element *, char *);
static void urlDecode (char *, char **, char **);
static Element *parentAndDelegateLookup (AHDRuntime *,
  Element *, char *, char *);

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: AHDRuntime *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

AHDRuntime *AHDRuntimeNew (int port, char *root)
{
  AHDRuntime *r;
  struct utsname u;
#ifdef HAVE_LIBPTHREAD
  pthread_mutexattr_t attr;
#endif

  r = XkNew (AHDRuntime);
  r->loadedDocuments = NULL;
  r->scriptHandler = NULL;
  r->eventHandler = NULL;
  r->tagHandler = NULL;
  r->linkHandler = NULL;

#ifdef NATIVE_NETWORKING
  r->server = NULL;
#endif

#ifdef HAVE_LIBPTHREAD
  r->readers = 0;
  pthread_mutexattr_init (&attr);
  pthread_mutexattr_setkind_np (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init (&r->readerMutex, &attr);
  pthread_mutex_init (&r->writerMutex, &attr);
#endif

  uname (&u);
  r->here = malloc (strlen (u.nodename) + 20);
  sprintf (r->here, "http://%s:%d/", u.nodename, port);

#ifdef NATIVE_SCRIPTING
  AHDRuntimeAddScriptHandler (r, scriptHandler, r);

#ifdef HAVE_LIBTCL8_0
  r->tclProcs = NULL;
#endif
#endif

#ifdef NATIVE_NETWORKING
  if (port != 0 && root != NULL)
  {
    NetServerNew (port, root, r);
  }
#endif

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeDelete
 * Description...: 
 *                 
 * In............: AHDRuntime *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeDelete (AHDRuntime *r)
{
  LoadedDocument *l;

#ifdef HAVE_LIBPHTHREAD
  pthread_mutex_destroy (r->readerMutex);
  pthread_mutex_destroy (r->writerMutex);
#endif

  while (r->loadedDocuments != NULL)
  {
    l = r->loadedDocuments;
    r->loadedDocuments = r->loadedDocuments->next;
    if (l->url) XkFree (l->url);
    XkFree (l);
  }

#ifdef NATIVE_NETWORKING
  if (r->server)
  {
    NetServerDelete (r->server);
  }
#endif

#ifdef NATIVE_SCRIPTING
#ifdef HAVE_LIBTCL8_0
  if (r->tclProcs != NULL) XkFree (r->tclProcs);
#endif
#endif

  HandlerDelete (r->scriptHandler);
  XkFree (r->here);
  XkFree (r);
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeHere
 * Description...: 
 *                 
 * In............: AHDRuntime *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *AHDRuntimeHere (AHDRuntime *r)
{
  return r->here;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeGetLoadedDocuments
 * Description...: 
 *                 
 * In............: AHDRuntime *
 * Out...........: NodeList *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeList *AHDRuntimeGetLoadedDocuments (AHDRuntime *r)
{
  NodeList *l;
  LoadedDocument *d;

  l = NodeListNew ();
  readerLock (r);
  for (d = r->loadedDocuments; d != NULL; d = d->next)
  {
    NodeListAppendItem (l, (Node *) d->document);
  }
  readerUnlock (r);
  return l;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeDocumentInit
 * Description...: 
 *                 
 * In............: Document *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeDocumentInit (AHDRuntime *r, Document *d)
{
  DocumentAddLinkHandler (d, linkHandler, r);
  DocumentAddTagHandler (d, tagHandler, r);
  DocumentAddEventHandler (d, eventHandler, r);
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeAdd...Handler
 * Description...: 
 *                 
 * In............: Document *
 *                 HandlerProc 
 *                 void *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeAddScriptHandler (AHDRuntime *r, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = r->scriptHandler;
  r->scriptHandler = h;
}

void AHDRuntimeAddEventHandler (AHDRuntime *r, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = r->eventHandler;
  r->eventHandler = h;
}

void AHDRuntimeAddTagHandler (AHDRuntime *r, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = r->tagHandler;
  r->tagHandler = h;
}

void AHDRuntimeAddLinkHandler (AHDRuntime *r, HandlerProc p, void *u)
{
  Handler *h;

  h = HandlerNew ();
  h->userData = u;
  h->handler = p;
  h->next = r->linkHandler;
  r->linkHandler = h;
}

/*-------------------------------------------------------------------------
 * Function......: call...Handler
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 ScriptHandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void callScriptHandler (AHDRuntime *r, ScriptHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  i.document = info->element->document;
  for (h = r->scriptHandler; h != NULL; h = h->next)
  {
    i.userData = h->userData;
    i.info.script = info;

    (h->handler)(&i);
  }
}

void callEventHandler (AHDRuntime *r, EventHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  if (info->source->type == ELEMENT_NODE)
  {
    i.document = ((Element *) info->source)->document;
  }
  else if (info->source->type == DOCUMENT_NODE)
  {
    i.document = (Document *) info->source;
  }
  else
  {
    i.document = NULL;
  }
  for (h = r->eventHandler; h != NULL; h = h->next)
  {
    i.userData = h->userData;
    i.info.event = info;
    
    (h->handler)(&i);
  }
}

void callTagHandler (AHDRuntime *r, TagHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  i.document = info->parser->document;
  for (h = r->tagHandler; h != NULL; h = h->next)
  {
    i.userData = h->userData;
    i.info.tag = info;
    
    (h->handler)(&i);
  }
}

void callLinkHandler (AHDRuntime *r, LinkHandlerInfo *info)
{
  HandlerInfo i;
  Handler *h;

  i.document = info->element->document;
  for (h = r->linkHandler; h != NULL; h = h->next)
  {
    i.userData = h->userData;
    i.info.link = info;
    
    (h->handler)(&i);
  }
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeParseText
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 char *
 *                 char *
 * Out...........: Document *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Document *AHDRuntimeParseText (AHDRuntime *r, char *text, char *url)
{
  Parser *p;
  Document *d;
  LoadedDocument *l;
  char *params;
  int loaded;

  d = DocumentNew ();
  d->root = ElementNew ();
  d->root->display = DISPLAY_BLOCK;
  d->root->floating = FLOAT_NONE;
  d->root->anonymous = ANON_BOX;
  d->root->document = d;
  d->root->tag = XkNewString ("root");
  d->root->namespace = XkNewString ("ahdruntime");
  
  AHDRuntimeDocumentInit (r, d);
  
  p = ParserNew ();
  p->document = d;
  p->startElement = d->root;

  ParserProcessData (p, "text/x-ahd", text);
  ParserDelete (p);

  writerLock (r);
  l = r->loadedDocuments;
  loaded = 1;
  while (l != NULL && !XkStrEqual (l->url, url))
  {
    l = l->next;
  }

  if (l == NULL)
  {
    l = XkNew (LoadedDocument);
    l->next = r->loadedDocuments;
    l->url = XkNewString (url);
    r->loadedDocuments = l;
    loaded = 0;
  }
  l->document = d;
  writerUnlock (r);
  
  if (!loaded)
  {
    params = malloc (strlen (url) + 12);
    strcpy (params, "ahd_origin=");
    strcat (params, url);
    AHDRuntimeBroadcastCall (r, (Node *) d->root, "onload", params);
    free (params);
  }

  return d;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeActivate
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 char *
 * Out...........: Document *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Document *AHDRuntimeActivate (AHDRuntime *r, char *path, char *url)
{
  FILE *in;
  Document *d;
  char *text;
  struct stat stats;

  d = AHDRuntimeFindLoadedDocument (r, url);
  if (d == NULL)
  {
    lstat (path, &stats);
    text = malloc (stats.st_size + 1);
    in = fopen (path, "r");
    if (in != NULL)
    {
      fread (text, stats.st_size, 1, in);
      fclose (in);
      text[stats.st_size] = '\0';
      d = AHDRuntimeParseText (r, text, url);
      XkFree (text);
    }
  }

  return d;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeLoad
 * Description...: 
 *                 
 * In............: AHDRuntime *
 * Out...........: Document *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Document *AHDRuntimeLoad (AHDRuntime *r, char *url)
{
#ifdef NATIVE_NETWORKING
  Document *d;
  char *text;

  d = AHDRuntimeFindLoadedDocument (r, url);
  if (d == NULL)
  {
    text = NetGet (url);
    d = AHDRuntimeParseText (r, text, url);
    XkFree (text);
  }
  
  return d;
#else
  return NULL;
#endif
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeUnload
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Document *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeUnload (AHDRuntime *r, Document *d)
{
  LoadedDocument *l, *p;

  AHDRuntimeBroadcastCall (r, (Node *) d->root, "onunload", NULL);

  writerLock (r);
  p = NULL;
  l = r->loadedDocuments;
  while (l != NULL && d != l->document)
  {
    p = l;
    l = l->next;
  }
  if (l != NULL)
  {
    if (p != NULL) p->next = l->next;
    else r->loadedDocuments = l->next;
    if (l->url) XkFree (l->url);
    XkFree (l);
  }
  writerUnlock (r);
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeSave
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Document *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeSave (AHDRuntime *r, Document *d)
{
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeCall
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Node *
 *                 char *
 *                 char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *AHDRuntimeCall (AHDRuntime *r, Node *current, char *name, char *params)
{
  Element *func;
  ScriptHandlerInfo scriptInfo;
  char *v;
  char buffer[32];
  char *url;
  char *fragment;
  int i, l;
  Document *d;

  v = NULL;
  if (strncasecmp (name, "http://", 7) == 0 ||
    strncasecmp (name, "urn:", 4) == 0)
  {
    l = strlen (name);
    url = malloc (l);
    fragment = malloc (l);

    for (i = l - 1; i > 0 && name[i] != '#'; i--) ;
    if (i > 0)
    {
      strncpy (url, name, i);
      url[i] = '\0';
      strncpy (fragment, name + i + 1, l - i - 1);
      fragment[l - i - 1] = '\0';

      d = AHDRuntimeFindLoadedDocument (r, url);
      if (d == NULL)
      {
#ifdef NATIVE_NETWORKING
	NetRequest *request;
	NetRRHeader header;
      
	request = NetRequestNew (METHOD_POST, name);
	request->data = params;
	header.next = NULL;
	header.name = "Content-length";
	header.value = buffer;
	sprintf (buffer, "%d", strlen (params));
	request->header = &header;
	NetRequestPerform (request);
	request->header = NULL;
	v = request->data;
	request->data = NULL;
	NetRequestDelete (request);
#endif
      }
      else
      {
	v = AHDRuntimeCall (r, (Node *) DocumentGetNonanonymousRoot (d),
	  fragment, params);
      }
    }
    free (url);
    free (fragment);
  }
  else
  {
    fprintf (stderr, "calling %s ... ", name);
    func = parentAndDelegateLookup (r, (Element *) current, "name", name);
    if (func != NULL)
    {
      scriptInfo.caller = (Element *) current;
      scriptInfo.element = func->parent;
      scriptInfo.script = func->rawContents;
      scriptInfo.params = params;
      scriptInfo.ret = NULL;
      scriptInfo.MIMEType = AttributeGetValue (func->attributes, "type");
      
      callScriptHandler (r, &scriptInfo);
      v = scriptInfo.ret;
    }
    fprintf (stderr, "done\n");
  }
  return v;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeBroadcastCall
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Node *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeBroadcastCall (AHDRuntime *r, Node *current,
  char *name, char *params)
{
  Element *element;
  ScriptHandlerInfo scriptInfo;

  while (current != NULL)
  {
    if (current->type == ELEMENT_NODE)
    {
      element = (Element *) current;
      if (XkStrEqual (element->tag, "func") && 
	XkStrEqual (ElementGetAttribute (element, "name"), name))
      {
	scriptInfo.caller = element->document->root;
	scriptInfo.element = element->parent;
	scriptInfo.script = element->rawContents;
	scriptInfo.params = params;
	scriptInfo.ret = NULL;
	scriptInfo.MIMEType = AttributeGetValue (element->attributes, "type");
	
	callScriptHandler (r, &scriptInfo);
      }
      else
      {
	AHDRuntimeBroadcastCall (r, NodeGetFirstChild (current), name, params);
      }
    }
    else if (current->type == DOCUMENT_NODE)
    {
      AHDRuntimeBroadcastCall (r, NodeGetFirstChild (current), name, params);
    }
    current = NodeGetNextSibling (current);
  }
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeGet
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Node *
 *                 char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *AHDRuntimeGet (AHDRuntime *r, Node *current, char *name)
{
  Element *var;
  char *v;

  v = NULL;
  if (strncasecmp (name, "http://", 7) == 0 ||
    strncasecmp (name, "urn:", 4) == 0)
  {
#ifdef NATIVE_NETWORKING
    NetRequest *request;

    request = NetRequestNew (METHOD_GET, name);
    NetRequestPerform (request);
    v = request->data;
    request->data = NULL;
    NetRequestDelete (request);
#endif
  }
  else
  {
    var = parentAndDelegateLookup (r, (Element *) current, "name", name);
    if (var != NULL && XkStrEqual (var->tag, "var"))
    {
      v = ElementGetContents (var);
    }
  }
  return v;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimePut
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Node *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *AHDRuntimePut (AHDRuntime *r, Node *current, char *name, char *value)
{
  Element *var;
  char *v;

  v = NULL;
  if (strncasecmp (name, "http://", 7) == 0 ||
    strncasecmp (name, "urn:", 4) == 0)
  {
#ifdef NATIVE_NETWORKING
    NetRequest *request;
    NetRRHeader header;
    char buffer[32];

    request = NetRequestNew (METHOD_PUT, name);
    request->data = XkNewString (value);
    header.next = NULL;
    header.name = XkNewString ("Content-length");
    header.value = XkNewString (buffer);
    sprintf (buffer, "%d", strlen (value));
    request->header = &header;
    NetRequestPerform (request);
    v = request->data;
    request->data = NULL;
    NetRequestDelete (request);
#endif
  }
  else
  {
    var = parentAndDelegateLookup (r, (Element *) current, "name", name);
    if (var != NULL && XkStrEqual (var->tag, "var"))
    {
      ElementSetContents (var, value);
    }
  }
  return v;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeStore
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 Node *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AHDRuntimeStore (AHDRuntime *r, Document *d, char *name, char *origin)
{
#ifdef NATIVE_NETWORKING
  NetRequest *request;
  NetRRHeader *header;
  char buffer[32];

  request = NetRequestNew (METHOD_PUT, name);
  request->data = NodeToText ((Node *) d);

  header = XkNew (NetRRHeader);
  header->next = request->header;
  header->name = XkNewString ("Content-length");
  sprintf (buffer, "%d", strlen (request->data));
  header->value = XkNewString (buffer);
  request->header = header;

  header = XkNew (NetRRHeader);
  header->next = request->header;
  header->name = XkNewString ("Content-type");
  header->value = XkNewString ("text/x-ahd");
  request->header = header;

  if (origin != NULL && strlen (origin) > 0)
  {
    header = XkNew (NetRRHeader);
    header->next = request->header;
    header->name = XkNewString ("X-AHD-Origin");
    header->value = XkNewString (origin);
    request->header = header;
  }

  NetRequestPerform (request);
  NetRequestDelete (request);
#endif
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeMatch
 * Description...: 
 *                 
 * In............: AHDRuntime
 *                 Element *
 *                 Element*
 * Out...........: NodeList *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NodeList *AHDRuntimeMatch (AHDRuntime *r, Element *source, Element *pattern)
{
  Element *funcBox;
  ScriptHandlerInfo *scriptInfo;
  NodeList *l;
  Node *n;
  void *peer;
  char *p;

  l = ElementMatch (source, pattern);
  if (NodeListGetLength (l) > 0)
  {
    n = (Node *) pattern;
    while (n != NULL)
    {
      if (!XkStrEqual (((Element *) n)->tag, "func") &&
	!XkStrEqual (((Element *) n)->tag, "var"))
      {
	peer = NULL;
	p = ElementGetAttribute ((Element *) n, "peer");
	if (p) sscanf (p, "%p", &peer);

	scriptInfo = ScriptHandlerInfoNew ();
	scriptInfo->reason = EE_MATCH;
	scriptInfo->caller = (Element *) peer;
	funcBox = attributeParentLookup ((Element *) n, "onmatch");
	if (funcBox != NULL)
	{
	  scriptInfo->element = funcBox;
	  scriptInfo->script = ElementGetAttribute (funcBox, "onmatch");
	  scriptInfo->MIMEType = AttributeGetValue (funcBox->attributes,
	    "type");

	  callScriptHandler (r, scriptInfo);
	}
	else
	{
	  funcBox = parentAndDelegateLookup (r,
	    (Element *) n, "name", "onmatch");
	  if (funcBox != NULL)
	  {
	    scriptInfo->element = funcBox->parent;
	    scriptInfo->script = funcBox->rawContents;
	    scriptInfo->MIMEType = AttributeGetValue (funcBox->attributes,
	      "type");
	  
	    callScriptHandler (r, scriptInfo);
	  }
	}
	ScriptHandlerInfoDelete (scriptInfo);
      }

      do
      {
	n = NodeGetSuccessor (n);
      }
      while (n != NULL && NodeGetNodeType (n) != ELEMENT_NODE);
    }
  }
  return l;
}

/*-------------------------------------------------------------------------
 * Function......: AHDRuntimeFindLoadedDocument
 * Description...: 
 *                 
 * In............: AHDRuntime *
 *                 char *
 * Out...........: Document *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Document *AHDRuntimeFindLoadedDocument (AHDRuntime *r, char *url)
{
  Document *d;
  LoadedDocument *l;

  readerLock (r);
  d = NULL;
  for (l = r->loadedDocuments; l != NULL; l = l->next)
  {
    if (XkStrEqual (l->url, url)) d = l->document;
  }
  readerUnlock (r);
  return d;
}

/*-------------------------------------------------------------------------
 * Function......: lock
 * Description...: 
 *                 
 * In............: AHDRuntime *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void readerLock (AHDRuntime *r)
{
#ifdef HAVE_LIBPTHREAD
  pthread_mutex_lock (&r->readerMutex);
  r->readers++;
  if (r->readers == 1)
  {
    pthread_mutex_lock (&r->writerMutex);
  }
  pthread_mutex_unlock (&r->readerMutex);
#endif
}

void readerUnlock (AHDRuntime *r)
{
#ifdef HAVE_LIBPTHREAD
  pthread_mutex_lock (&r->readerMutex);
  r->readers--;
  if (r->readers == 0)
  {
    pthread_mutex_unlock (&r->writerMutex);
  }
  pthread_mutex_unlock (&r->readerMutex);
#endif
}

void writerLock (AHDRuntime *r)
{
#ifdef HAVE_LIBPTHREAD
  pthread_mutex_lock (&r->writerMutex);
#endif
}

void writerUnlock (AHDRuntime *r)
{
#ifdef HAVE_LIBPTHREAD
  pthread_mutex_unlock (&r->writerMutex);
#endif
}

/*-------------------------------------------------------------------------
 * Function......: scriptHandler
 * Description...: 
 *                 
 * In............: HandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

#ifdef NATIVE_SCRIPTING
void scriptHandler (HandlerInfo *i)
{
  ScriptHandlerInfo *info;
  EventHandlerInfo modInfo;
  char c;
  int l, j;
  char *name, *value;
  char buffer[128];
  AHDRuntime *r;

  i->document->modified = 0;
  info = i->info.script;
  r = (AHDRuntime *) i->userData;
  if (XkStrEqual (info->MIMEType, "text/tcl"))
  {
#ifdef HAVE_LIBTCL8_0
    char *ret;
    Tcl_Interp *tclInterp;

    tclInterp = Tcl_CreateInterp ();
    Kinoparser_Init (tclInterp);
    Kinoahd_Init (tclInterp);
#ifdef NATIVE_WIDGET
    Kinowidget_Init (tclInterp);
#endif
    if (r->tclProcs != NULL)
    {
      Tcl_GlobalEval (tclInterp, r->tclProcs);
    }

    SWIG_MakePtr (buffer, i->userData, "_AHDRuntime_p");
    Tcl_SetVar (tclInterp, "ahd_ahd", buffer, 0);

    SWIG_MakePtr (buffer, i->document, "_Document_p");
    Tcl_SetVar (tclInterp, "ahd_document", buffer, 0);

    SWIG_MakePtr (buffer, info->caller, "_Element_p");
    Tcl_SetVar (tclInterp, "ahd_caller", buffer, 0);

    SWIG_MakePtr (buffer, info->element, "_Element_p");
    Tcl_SetVar (tclInterp, "ahd_element", buffer, 0);

    if (info->params)
    {
      l = 0;
      while (info->params[l] != '\0')
      {
	j = l;
	while (info->params[j] != '\0' && info->params[j] != '&') j++;
	c = info->params[j]; info->params[j] = '\0';
	urlDecode (&info->params[l], &name, &value);
	Tcl_SetVar (tclInterp, name, value, 0);
	info->params[j] = c;
	l = (info->params[j] == '&' ? j + 1 : j);
	if (name) XkFree (name);
	if (value) XkFree (value);
      }
    }

    if (Tcl_GlobalEval (tclInterp, info->script) == TCL_ERROR)
    {
      fprintf (stderr, "Script Error: %s\n", tclInterp->result);
    }
    ret = Tcl_GetVar (tclInterp, "ahd_result", 0);
    if (ret != NULL)
    {
      info->ret = XkNewString (ret);
    }

    Tcl_Eval (tclInterp, "set r \"\"
set commands [info procs]
foreach command $commands {
  append r \"proc $command {[info args $command]} {[info body $command]}\n\"
}
return $r");
    writerLock (r);
    if (r->tclProcs) XkFree (r->tclProcs);
    r->tclProcs = XkNewString (tclInterp->result);
    writerUnlock (r);

    Tcl_DeleteInterp (tclInterp);
#endif
  }
  else if (XkStrEqual (info->MIMEType, "text/lua"))
  {
#ifdef HAVE_LIBLUA
    lua_Object ret;

    lua_open ();

    if (!luaWrapperInit) 
    { 
      kinoparserlua_init ();
      kinoahdlua_init ();
      lua_userinit ();
      poslib_open ();
      loadlib_open ();
#ifdef NATIVE_WIDGET
      kinowidgetlua_init ();
#endif

      luaWrapperInit = 1;
    }

    lua_beginblock ();

    if (i->document) lua_pushuserdata (i->document); else lua_pushnil ();
    lua_setglobal ("ahd_document");
    
    if (i->userData) lua_pushuserdata (i->userData); else lua_pushnil ();
    lua_setglobal ("ahd_ahd");
    
    if (info->caller) lua_pushuserdata (info->caller); else lua_pushnil ();
    lua_setglobal ("ahd_caller");
    
    if (info->element) lua_pushuserdata (info->element); else lua_pushnil ();
    lua_setglobal ("ahd_element");

    if (info->params)
    {
      l = 0;
      while (info->params[l] != '\0')
      {
	j = l;
	while (info->params[j] != '\0' && info->params[j] != '&') j++;
	c = info->params[j]; info->params[j] = '\0';
	urlDecode (&info->params[l], &name, &value);
	lua_pushstring (value);
	lua_setglobal (name);
	info->params[j] = c;
	l = (info->params[j] == '&' ? j + 1 : j);
	if (name) XkFree (name);
	if (value) XkFree (value);
      }
    }
    lua_dostring (info->script);
    ret = lua_getglobal ("ahd_result");
    if (ret && lua_isstring (ret))
    {
      info->ret = lua_getstring (ret);
    }
    lua_endblock ();
#endif
  }
  if (i->document->modified)
  {
    modInfo.source = (Node *) i->document;
    modInfo.reason = EE_MOD_SUBTREE;
    callEventHandler ((AHDRuntime *) i->userData, &modInfo);
  }
}
#endif

/*-------------------------------------------------------------------------
 * Function......: linkHandler
 * Description...: 
 *                 
 * In............: HandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void linkHandler (HandlerInfo *i)
{
#ifdef NATIVE_NETWORKING
  LinkHandlerInfo *l;
  NetRequest *r;

  l = i->info.link;
  switch (l->parser->handling)
  {
    case HANDLE_ENTITY:
    case HANDLE_DOCTYPE:
    case HANDLE_EMBED:
      r = NetRequestNew (METHOD_GET, l->parser->URI);
      NetRequestPerform (r);
      ParserProcessData (l->parser, r->contentType, r->data);
      NetRequestDelete (r);
      break;
    case HANDLE_NEW:
    case HANDLE_REPLACE:
      break;
  }
#endif
  callLinkHandler ((AHDRuntime *) i->userData, i->info.link);
}

/*-------------------------------------------------------------------------
 * Function......: tagHandler
 * Description...: 
 *                 
 * In............: HandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void tagHandler (HandlerInfo *i)
{
  Element *element;

  element = i->info.tag->parser->element;
  if (XkStrEqual (i->info.tag->name, "func") ||
    XkStrEqual (i->info.tag->name, "var"))
  {
    element->display = DISPLAY_NONE;
  }
  else if (XkStrEqual (i->info.tag->name, "/func") ||
    XkStrEqual (i->info.tag->name, "/var"))
  {
    element->rawContents = ElementGetContents (element);
  }
  callTagHandler ((AHDRuntime *) i->userData, i->info.tag);
}

/*-------------------------------------------------------------------------
 * Function......: eventHandler
 * Description...: 
 *                 
 * In............: HandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void eventHandler (HandlerInfo *i)
{
  EventHandlerInfo *e;
  Element *funcBox;
  ScriptHandlerInfo *scriptInfo;
  char *eventName;

  e = i->info.event;
  if (e->source != NULL && e->source->type == ELEMENT_NODE)
  {
    switch (e->reason)
    {
      case EE_CLICK: eventName = "onclick"; break;
      case EE_DBL_CLICK: eventName = "ondblclick"; break;
      case EE_ENTER: eventName = "onenter"; break;
      case EE_LEAVE: eventName = "onleave"; break;
      case EE_MOTION: eventName = "onmotion"; break;
      case EE_KEY: eventName = "onkey"; break;
      default:
	eventName = NULL;
	break;
    }
    if (eventName != NULL)
    {
      scriptInfo = ScriptHandlerInfoNew ();
      scriptInfo->reason = e->reason;
      scriptInfo->event = e->event;
      funcBox = attributeParentLookup ((Element *) e->source, eventName);
      if (funcBox != NULL)
      {
	scriptInfo->caller = (Element *) e->source;
	scriptInfo->element = funcBox;
	scriptInfo->script = ElementGetAttribute (funcBox, eventName);
	scriptInfo->MIMEType = AttributeGetValue (funcBox->attributes, "TYPE");

	callScriptHandler ((AHDRuntime *) i->userData, scriptInfo);
      }
      else
      {
	funcBox = parentAndDelegateLookup ((AHDRuntime *) i->userData, 
	  (Element *) e->source, "NAME", eventName);
	if (funcBox != NULL)
	{
	  scriptInfo->caller = (Element *) e->source;
	  scriptInfo->element = funcBox->parent;
	  scriptInfo->script = funcBox->rawContents;
	  scriptInfo->MIMEType = AttributeGetValue (funcBox->attributes,
	    "TYPE");
	  
	  callScriptHandler ((AHDRuntime *) i->userData, scriptInfo);
	}
      }
      ScriptHandlerInfoDelete (scriptInfo);
    }
  }
  callEventHandler ((AHDRuntime *) i->userData, e);
}

/*-------------------------------------------------------------------------
 * Function......: attributeParentLookup
 * Description...: 
 *                 
 * In............: Element *
 *                 char *
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *attributeParentLookup (Element *p, char *name)
{
  if (p != NULL)
  {
    if (AttributeGetValue (p->attributes, name))
    {
      return p;
    }
    else
    {
      return attributeParentLookup (p->parent, name);
    }
  }
  else
  {
    return NULL;
  }
}

/*-------------------------------------------------------------------------
 * Function......: urlDecode
 * Description...: 
 *                 
 * In............: char *
 *                 char **
 *                 char **
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void urlDecode (char *string, char **name, char **value)
{
  int i, j;

  i = 0;
  *name = NULL;
  *value = NULL;
  while (string[i] != '\0' && string[i] != '=') i++;
  if (string[i] == '=')
  {
    *name = malloc (i + 1);
    *value = malloc (strlen (string) + 1);

    for (j = 0, i = 0; string[i] != '='; i++, j++)
    {
      switch (string[i])
      {
	case '+':
	  (*name)[j] = ' ';
	  break;
	case '%':
	  (*name)[j] = HEX2INT (string[i + 1]) * 16 + HEX2INT (string[i +2]);
	  i += 2;
	  break;
	default:
	  (*name)[j] = string[i];
	  break;
      }
    }
    (*name)[j] = '\0';

    for (j = 0, i = i + 1; string[i] != '\0'; i++, j++)
    {
      switch (string[i])
      {
	case '+':
	  (*value)[j] = ' ';
	  break;
	case '%':
	  (*value)[j] = HEX2INT (string[i + 1]) * 16 + HEX2INT (string[i +2]);
	  i += 2;
	  break;
	default:
	  (*value)[j] = string[i];
	  break;
      }
    }
    (*value)[j] = '\0';

  }
}

/*-------------------------------------------------------------------------
 * Function......: parentAndDelegateLookup
 * Description...: 
 *                 
 * In............: Node *
 *                 char *
 *                 char *
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *parentAndDelegateLookup (AHDRuntime *r, 
  Element *current, char *name, char *value)
{
  Node *p;
  Element *delegate;
  int abort;
  char *v;
  Document *d;

  abort = 0;
  delegate = NULL;
  d = NULL;
  while (current && !abort)
  {
    v = AttributeGetValue (current->attributes, name);
    if (XkStrEqual (v, value))
    {
      abort = 1;
    }
    else
    {
      if (current->contents)
      {
        p = current->contents;
        while (p && !abort)
        {
	  if (p->type == ELEMENT_NODE)
	  {
	    v = AttributeGetValue (((Element *) p)->attributes, name);
	    if (XkStrEqual (v, value))
	    {
	      abort = 1;
	      current = (Element *) p;
	    }
	    else if (XkStrEqual (((Element *) p)->tag, "delegate"))
	    {
	      delegate = (Element *) p;
	    }
	  }
          p = p->next;
        }
	if (!abort && delegate != NULL)	
	{
	  v = AttributeGetValue (delegate->attributes, "href");
	  if (v != NULL)
	  {
	    d = AHDRuntimeLoad (r, v); /* FIXME: this is wrong! */

	    if (d != NULL)
	    {
	      current = DocumentGetNonanonymousRoot (d);
	    }
	  }
	}
      }
    } 
    if (!abort && !(delegate != NULL && d != NULL) && current != NULL)
    {
      current = current->parent;
    }
  }
  return current;
}
