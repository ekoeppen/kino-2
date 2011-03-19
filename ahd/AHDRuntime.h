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
 * File..........: AHDRuntime.h
 * Description...: 
 *
 * $Id: AHDRuntime.h,v 1.3 1999/07/12 12:11:31 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __AHDRUNTIME_H
#define __AHDRUNTIME_H

/* global types */

typedef struct _LoadedDocument
{
  struct _LoadedDocument *next;

  Document *document;
  char *url;			/* URL the document was fetched from */
} LoadedDocument;

typedef struct _AHDRuntime
{
  LoadedDocument *loadedDocuments; /* list of currently active documents */

#ifdef NATIVE_NETWORKING
  NetServer *server;		/* the associated net server */
#endif

#ifdef HAVE_LIBPTHREAD
  int readers;			/* number of reading threads */
  pthread_mutex_t readerMutex;	/* mutex for read-locking the runtime */
  pthread_mutex_t writerMutex;	/* mutex for write-locking the runtime */
#endif

  char *here;			/* the HTTP address of the runtime */

#ifdef NATIVE_SCRIPTING
#ifdef HAVE_LIBTCL8_0
  char *tclProcs;		/* globally usable procs */
#endif
#endif

  struct _Handler *scriptHandler;
  struct _Handler *tagHandler;
  struct _Handler *eventHandler;
  struct _Handler *linkHandler;
} AHDRuntime;

/* global functions */

AHDRuntime *AHDRuntimeNew (int, char *);
void AHDRuntimeDelete (AHDRuntime *);

void AHDRuntimeAddScriptHandler (AHDRuntime *, HandlerProc, void *);
void AHDRuntimeAddTagHandler (AHDRuntime *, HandlerProc, void *);
void AHDRuntimeAddEventHandler (AHDRuntime *, HandlerProc, void *);
void AHDRuntimeAddLinkHandler (AHDRuntime *, HandlerProc, void *);

void AHDRuntimeDocumentInit (AHDRuntime*, Document *);

/* core functions */

char *AHDRuntimeCall (AHDRuntime *, Node *, char *, char *);
char *AHDRuntimeGet (AHDRuntime *, Node *, char *);
char *AHDRuntimePut (AHDRuntime *, Node *, char *, char *);

/* level 1 functions */

Document *AHDRuntimeLoad (AHDRuntime *, char *);
void AHDRuntimeUnload (AHDRuntime *, Document *);
void AHDRuntimeSave (AHDRuntime *, Document *);
void AHDRuntimeStore (AHDRuntime *, Document *, char *, char *);

void AHDRuntimeBroadcastCall (AHDRuntime *, Node *, char *, char *);
NodeList *AHDRuntimeMatch (AHDRuntime *, Element *, Element *);
char *AHDRuntimeHere (AHDRuntime *);
NodeList *AHDRuntimeGetLoadedDocuments (AHDRuntime *);
char *AHDRuntimeGetOrigin (AHDRuntime *, Document *);

/* helper functions */

Document *AHDRuntimeActivate (AHDRuntime *, char *, char *);
Document *AHDRuntimeParseText (AHDRuntime *, char *, char *);
Document *AHDRuntimeFindLoadedDocument (AHDRuntime *, char *);

#endif
