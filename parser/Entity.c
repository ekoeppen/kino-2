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
 * Description...: Implementation of the Entity structure which holds
 *                 the name and value of an entity
 *
 * $Id: Entity.c,v 1.3 1999/03/29 14:34:08 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

/* local types */

typedef struct			/* character entity according to SGML */
{
  char *name;			/* name of the entity */
  unsigned char character;     	/* corresponding character */
} CharEntity;

/* local variables */

static CharEntity entity[] =	/* table of the know character
				   entities */
{
  {"lt",     '<'},
  {"gt",     '>'},
  {"quot",   '"'},
  {"apos",   '\''},
  {"amp",    '&'},
  {"nbsp",   ' '},
  {"iexcl",  161},
  {"cent",   162},
  {"pound",  163},
  {"curren", 164},
  {"yen",    165},
  {"brvbar", 166},
  {"sect",   167},
  {"uml",    168},
  {"copy",   169},
  {"ordf",   170},
  {"laquo",  171},
  {"not",    172},
  {"shy",    173},
  {"reg",    174},
  {"macr",   175},
  {"deg",    176},
  {"plusmn", 177},
  {"sup2",   178},
  {"sup3",   179},
  {"acute",  180},
  {"micro",  181},
  {"para",   182},
  {"middot", 183},
  {"cedil",  184},
  {"sup1",   185},
  {"ordm",   186},
  {"raquo",  187},
  {"frac14", 188},
  {"frac12", 189},
  {"frac34", 190},
  {"iquest", 191},
  {"Agrave", 192},
  {"Aacute", 193},
  {"Acirc",  194},
  {"Atilde", 195},
  {"Auml",   196},
  {"Aring",  197},
  {"AElig",  198},
  {"Ccedil", 199},
  {"Egrave", 200},
  {"Eacute", 201},
  {"Ecirc",  202},
  {"Euml",   203},
  {"Igrave", 204},
  {"Iacute", 205},
  {"Icirc",  206},
  {"Iuml",   207},
  {"ETH",    208},
  {"Ntilde", 209},
  {"Ograve", 210},
  {"Oacute", 211},
  {"Ocirc",  212},
  {"Otilde", 213},
  {"Ouml",   214},
  {"times",  215},
  {"Oslash", 216},
  {"Ugrave", 217},
  {"Uacute", 218},
  {"Ucirc",  219},
  {"Uuml",   220},
  {"Yacute", 221},
  {"THORN",  222},
  {"szlig",  223},
  {"agrave", 224},
  {"aacute", 225},
  {"acirc",  226},
  {"atilde", 227},
  {"auml",   228},
  {"aring",  229},
  {"aelig",  230},
  {"ccedil", 231},
  {"egrave", 232},
  {"eacute", 233},
  {"ecirc",  234},
  {"euml",   235},
  {"igrave", 236},
  {"iacute", 237},
  {"icirc",  238},
  {"iuml",   239},
  {"eth",    240},
  {"ntilde", 241},
  {"ograve", 242},
  {"oacute", 243},
  {"ocirc",  244},
  {"otilde", 245},
  {"ouml",   246},
  {"divide", 247},
  {"oslash", 248},
  {"ugrave", 249},
  {"uacute", 250},
  {"ucirc",  251},
  {"uuml",   252},
  {"yacute", 253},
  {"thorn",  254},
  {"yuml",   255}
};

/* local functions */

char lookupCharacterEntity (char *);
int convertNumericReference (char *);

/*-------------------------------------------------------------------------
 * Function......: EntityLookup
 * Description...: Find an entity in the table of known entities
 *
 * In............: Entity *: entity table
 *                 char *: name of the entity
 * Out...........: char *: corresponding string or "" if the entity
 *                 does not exist
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

char *EntityLookup (Entity *entity, char *name)
{
  char *r;
  static char buffer[2];

  r = buffer;
  buffer[1] = '\0';
  if (name[0] == '&') name++;
  if (name[0] == '#')
  {
    buffer[0] = (unsigned char) convertNumericReference (name);
  }
  else
  {
    buffer[0] = lookupCharacterEntity (name);
    if (buffer[0] == '\0')
    {
      while (entity != NULL && strcmp (entity->name, name) != 0)
      {
	entity = entity->next;
      }
      if (entity != NULL)
      {
	r = entity->value;
      }
    }
  }
  return r[0] ? r : NULL;
}

/*-------------------------------------------------------------------------
 * Function......: EntityDelete
 * Description...: 
 *                 
 * In............: Entity *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void EntityDelete (Entity *e)
{
  Entity *current;

  while (e != NULL)
  {
    current = e;
    e = e->next;
    if (current->name != NULL) XkFree (current->name);
    if (current->value != NULL) XkFree (current->value);
    XkFree (current);
  }
}

/*-------------------------------------------------------------------------
 * Function......: convertNumericReference
 * Description...: Convert a numeric character reference to the
 *                 corresponding ISO Latin 1 character
 *
 * In............: char *: numeric reference
 * Out...........: char: corresponding character or ' ' if the entity
 *                 does not exist
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

int convertNumericReference (char *ref)
{
  char buffer[MAX_NAME_LENGTH];
  char *b;
  int c;
  int decimal;
  int end;

  b = buffer;
  end = 0;
  decimal = 1;
  ref++;
  if (!isdigit (*ref))
  {
    decimal = 0;
    ref++;
  }
  while (*ref != '\0' && *ref != ';' && !isspace (*ref))
  {
    *b = *ref;
    if (b - buffer < MAX_NAME_LENGTH)
    {
      b++;
    }
    ref++;
  }
  *b = '\0';
  c = strtol (buffer, NULL, decimal ? 10 : 16);

  return c;
}

/*-------------------------------------------------------------------------
 * Function......: lookupCharacterEntity
 * Description...: 
 *                 
 * In............: char *:
 * Out...........: char :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char lookupCharacterEntity (char *name)
{
  int i;
  char c;
  
  i = 0;
  while (i < XkNumber (entity) && strcmp (entity[i].name, name) != 0)
  {
    i++;
  }
  if (i < XkNumber (entity))
  {
    c = entity[i].character;
  }
  else
  {
    c = '\0';
  }

  return c;
}

