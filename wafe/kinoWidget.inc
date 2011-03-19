/* $Id: kinoWidget.inc,v 1.5 1999/02/17 16:33:53 koeppen Exp $ */
/* -*- C -*- */

#include "Kino.h"
#include "KinoP.h"

extern Tcl_HashTable kinoParserTable;  /* for stringToP and friends */

#define kinoInsetAlign int

extern XrmQuark qcallback;

static void
wafeStringAppendAttributeValueList(ws, attributes)
wafeString ws;
Attribute  *attributes;
    {
    Attribute  *p;
    int oldLength = ws->length;

    for(p=attributes; p; p=p->next)
	{
	wafeStringAppendListItemEscaped(ws,p->name);
	wafeStringAppendListItemEscaped(ws,p->value);
	}
    if (ws->length != oldLength) 
	{
	ws->length--;
	ws->buffer[ws->length] = 0;
	}
    }

void KinoSetDocument (KinoWidget w, Document *d)
{
  DisplayStyle display;

  if (w->kino.document != NULL)
  {
    if (w->kino.document->root != NULL)
    {
      display = w->kino.document->root->display;
      w->kino.document->root->display = DISPLAY_NONE;
      VisualChangeManagedInsets (w->kino.document->root);
      w->kino.document->root->display = display;
    }
  }

  EventReset (w);

/*   DocumentAddTagHandler (d, KinoTagHandler, w); */
/*   DocumentAddLinkHandler (d, KinoLinkHandler, w); */
/*   DocumentAddEventHandler (d, KinoEventHandler, w); */
/*   DocumentAddScriptHandler (d, KinoScriptHandler, w); */

  if (d != NULL && d->root != NULL)
  {
    VisualChangeManagedInsets (d->root);
  }
  w->kino.document = d;
}    

void KinoDocumentSetHandlers (Document *d, KinoWidget w)
{
  DocumentAddTagHandler (d, KinoTagHandler, w);
  DocumentAddLinkHandler (d, KinoLinkHandler, w);
  DocumentAddEventHandler (d, KinoEventHandler, w);
}    

Document *KinoGetDocument (KinoWidget w)
{
  return w->kino.document;
}    

/*
 * Local Variables:
 * mode: c
 */
