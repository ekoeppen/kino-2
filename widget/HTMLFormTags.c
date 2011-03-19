/*-------------------------------------------------------------------------
 * kinowidget - an XML display widget, part of the Kino XML/CSS processor
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
 * File..........: HTMLFormTags.c
 * Description...: Functions to create HTML FORM elements
 *
 * $Id$
 *-----------------------------------------------------------------------*/

#include <limits.h>
#include <gtk/gtkcontainer.h>
#include "gtkkino.h"
#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"

/* local defintions */

/* local functions */

/*-------------------------------------------------------------------------
 * Function......: HTMLFormTagsHandler
 * Description...: 
 *                 
 * In............: HandlerInfo *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void HTMLFormTagsHandler (HandlerInfo *i)
{
  InsetData *inset;
  Element *parent;
  InsetSetValueFunc setValue;
  InsetGetValueFunc getValue;
  InsetDeleteFunc delete;
  char *v;

  inset = NULL;
  setValue = NULL;
  getValue = NULL;
  delete = NULL;
  parent = i->info.tag->parser->element;
  if (XkStrEqual (i->info.tag->name, "button"))
  {
    inset = InsetDataButtonNew (i->userData, parent);
    getValue = InsetDataButtonGetValue;
    setValue = InsetDataButtonSetValue;
    delete = InsetDataButtonDelete;
    v = AttributeGetValue (i->info.tag->attributes, "value");
    if (v != NULL)
    {
      InsetDataButtonSetValue (inset, v);
    }
  }
  else if (XkStrEqual (i->info.tag->name, "input"))
  {
    inset = InsetDataInputNew (i->userData, parent);
    getValue = InsetDataInputGetValue;
    setValue = InsetDataInputSetValue;
    delete = InsetDataInputDelete;
  }
  else if (XkStrEqual (i->info.tag->name, "textarea"))
  {
    inset = InsetDataTextareaNew (i->userData, parent);
    getValue = InsetDataTextareaGetValue;
    setValue = InsetDataTextareaSetValue;
    delete = InsetDataTextareaDelete;
  }
  if (inset != NULL)
  {
    ParserAddInset (i->info.tag->parser, inset);
    ((Inset *) parent->contents)->setValue = setValue;
    ((Inset *) parent->contents)->getValue = getValue;
    ((Inset *) parent->contents)->delete = delete;
  }
}

/*-------------------------------------------------------------------------
 * Function......: HTMLInputElementNew
 * Description...: 
 *                 
 * In............: 
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *HTMLInputElementNew (Document *d)
{
  Element *parent;
  Inset *inset;

  parent = ElementNew ();
  ElementSetTagName (parent, "input");
  parent->display = DISPLAY_INLINE;
  inset = InsetNew ();
  NodeAppendChild ((Node *) parent, (Node *) inset);
  inset->inset = InsetDataInputNew ((VisualWidget) d->userData, parent);
  inset->getValue = InsetDataInputGetValue;
  inset->setValue = InsetDataInputSetValue;
  inset->delete = InsetDataInputDelete;

  return parent;
}

/*-------------------------------------------------------------------------
 * Function......: HTMLTextareaElementNew
 * Description...: 
 *                 
 * In............: 
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *HTMLTextareaElementNew (Document *d)
{
  Element *parent;
  Inset *inset;

  parent = ElementNew ();
  ElementSetTagName (parent, "input");
  parent->display = DISPLAY_INLINE;
  inset = InsetNew ();
  NodeAppendChild ((Node *) parent, (Node *) inset);
  inset->inset = InsetDataTextareaNew ((VisualWidget) d->userData, parent);
  inset->getValue = InsetDataTextareaGetValue;
  inset->setValue = InsetDataTextareaSetValue;
  inset->delete = InsetDataTextareaDelete;

  return parent;
}

/*-------------------------------------------------------------------------
 * Function......: HTMLButtonElementNew
 * Description...: 
 *                 
 * In............: 
 * Out...........: Element *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Element *HTMLButtonElementNew (Document *d)
{
  Element *parent;
  Inset *inset;

  parent = ElementNew ();
  ElementSetTagName (parent, "button");
  parent->display = DISPLAY_INLINE;
  inset = InsetNew ();
  NodeAppendChild ((Node *) parent, (Node *) inset);
  inset->inset = InsetDataButtonNew ((VisualWidget) d->userData, parent);
  inset->delete = InsetDataButtonDelete;

  return parent;
}
