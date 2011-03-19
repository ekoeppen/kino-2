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
 * File..........: Attribute.c
 * Description...: Implementation of the Attribute structure which holds
 *                 the name and attributes of a parsed tag
 *
 * $Id: Attribute.c,v 1.3 1999/03/29 14:34:03 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/*-------------------------------------------------------------------------
 * Function......: AttributeNew
 * Description...: 
 *                 
 * In............: -
 * Out...........: Attribute *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

Attribute *AttributeNew (void)
{
  Attribute *a;

  a = XkNew (Attribute);
  a->name = NULL;
  a->namespace = NULL;
  a->value = NULL;
  a->next = NULL;

  return a;
}

/*-------------------------------------------------------------------------
 * Function......: AttributeGetValue
 * Description...: Return the value of an attribute
 *
 * In............: Attribute *: Attribute list
 *                 char *: name of the attribute
 * Out...........: char *: value of the attribute or NULL if the 
 *                 attribute was not found or does not have a value
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

char *AttributeGetValue (Attribute *a, char *name)
{
  char *r;

  r = NULL;
  while (a != NULL && !XkStrEqual (a->name, name))
  {
    a = a->next;
  }
  if (a != NULL)
  {
    r = a->value;
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: AttributeExists
 * Description...: Check if the tag has a given attribute
 *
 * In............: Attribute *: Atribute list
 *                 char *: name of the attribute
 * Out...........: int: 1, if the given attribute exists
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int AttributeExists (Attribute *a, char *name)
{
  int r;

  r = 0;
  while (a != NULL && !XkStrEqual (a->name, name))
  {
    a = a->next;
  }
  if (a != NULL)
  {
    r = 1;
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: AttributeDuplicate
 * Description...: Duplicate an attributes list
 *
 * In............: Attribute *: attributes to be copied
 * Out...........: Attribute *: copy of the attribute list
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

Attribute *AttributeDuplicate (Attribute *attributes)
{
  Attribute *a, *p, *result;

  result = NULL;
  for (p = attributes; p != NULL; p = p->next)
  {
    a = AttributeNew ();
    a->name = XkNewString (p->name);
    a->value = XkNewString (p->value);
    a->next = result;
    result = a;
  }
  return result;
}

/*-------------------------------------------------------------------------
 * Function......: AttributeDelete
 * Description...: 
 *                 
 * In............: Attribute *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AttributeDelete (Attribute *a)
{
  Attribute *h;

  while (a != NULL)
  {
    h = a;
    a = a->next;
    if (h->name != NULL) XkFree (h->name);
    if (h->namespace != NULL) XkFree (h->namespace);
    if (h->value != NULL) XkFree (h->value);
    XkFree (h);
  }
}

/*-------------------------------------------------------------------------
 * Function......: AttributeSetNSPrefix
 * Description...: 
 *                 
 * In............: Attribute *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void AttributeSetNSPrefix (Attribute *a, char *p)
{
  if (a->namespace != NULL) XkFree (a->namespace);
  a->namespace = XkNewString (p);
}

/*-------------------------------------------------------------------------
 * Function......: AttributeGetNSPrefix
 * Description...: 
 *                 
 * In............: Attribute *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *AttributeGetNSPrefix (Attribute *a)
{
  return a->namespace;
}
