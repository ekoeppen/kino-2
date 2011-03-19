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
 * File..........: KinoUtil.h
 * Description...: Declaration of utility functions and macros
 *-----------------------------------------------------------------------*/

#ifndef __KINOUTIL_H
#define __KINOUTIL_H

#ifdef MALLOC_TRACE
#include "dmalloc.h"
#endif

/* private definitions */

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

/* memory management utility macros */

#define XkNew(type)         ((type *) malloc ((unsigned) sizeof (type))) 
#define XkNewString(str) \
  ((str) != NULL ? \
    (strcpy (malloc ((unsigned) strlen (str) + 1), str)) : \
    NULL)
#define XkRealloc(ptr,size) realloc (ptr, size)
#define XkFree(ptr)         free (ptr)

#define XkNumber(array) (sizeof(array)/sizeof(array[0]))
  
/* utility macros */

#define XkStrEqual(str,other) \
  ((str) != NULL && (other) != NULL ? \
    !strcasecmp ((str), (other)) : 0)
  
#endif
