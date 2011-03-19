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
 * File..........: Net.h
 * Description...: 
 *
 * $Id: Net.h,v 1.3 1999/07/12 12:10:59 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __NET_H
#define __NET_H

/* global types */

struct _AHDRuntime;

typedef enum
{
  METHOD_GET = 0,
  METHOD_PUT = 1,
  METHOD_POST = 2,
  METHOD_HEAD = 3
} NetRequestMethod;

typedef struct _NetServer
{
  char *root;
  int port;
  int masterSocket;
#ifdef HAVE_LIBPTHREAD
  pthread_t thread;
#endif
  struct _AHDRuntime *ahdRuntime;
} NetServer;

typedef struct _NetRRHeader
{
  struct _NetRRHeader *next;

  char *name;
  char *value;
} NetRRHeader;

typedef struct _NetRequest
{
  NetRequestMethod method;
  char *url;
  int status;
  char *data;
  char *contentType;
  NetRRHeader *header;
} NetRequest;

/* global functions */

NetServer *NetServerNew (int, char *, struct _AHDRuntime *);
void NetServerDelete (NetServer *);

NetRequest *NetRequestNew (NetRequestMethod, char *);
void NetRequestPerform (NetRequest *);
void NetRequestDelete (NetRequest *);

char *NetGet (char *);

#endif
