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
 * File..........: Net.c
 * Description...: 
 *
 * $Id: Net.c,v 1.4 1999/07/12 12:10:44 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include "KinoParser.h"
#include "Net.h"
#include "AHDRuntime.h"

/* #define DEBUG */

#define THREADED_SERVER

/* local types */

typedef struct _ServerTaskContext
{
  NetServer *server;
  int socket;
} ServerTaskContext;

/* local variables */

static char *mimeTypes[][2] = 
{
  {"xml",  "text/xml"},
  {"html", "text/html"},
  {"ahd",  "text/x-ahd"},
  {"css",  "text/css"},
  {"txt",  "text/plain"},
};

static char *HTTPMethods[4] =
{
  "GET", "PUT", "POST", "HEAD"
};

/* local functions */

static int openSocket(char *, unsigned short);
static char *getContentType (char *);
static char *readLine (FILE *);
static void respondHEAD (NetServer *, FILE *, NetRRHeader *, char *, char *);
static void respondGET (NetServer *, FILE *, NetRRHeader *,
  char *, char *, char *, char *);
static void respondPOST (NetServer *, FILE *, NetRRHeader *,
  char *, char *, char *, char *, char *, int);
static void respondPUT (NetServer *, FILE *, NetRRHeader *,
  char *, char *, char *, char *, int);
static void processRequest (NetServer *, FILE *);
static void *serverTask (void *arg);
static char *resolveURL (char *, char *);
static char *getHeaderValue (NetRRHeader *, char *);

/*-------------------------------------------------------------------------
 * Function......: NetRequestNew
 * Description...: 
 *                 
 * In............: NetRequestMethod
 *                 char *
 * Out...........: NetRequest *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NetRequest *NetRequestNew (NetRequestMethod m, char *url)
{
  NetRequest *r;

  r = XkNew (NetRequest);
  r->method = m;
  r->url = url;
  r->status = 501;
  r->contentType = NULL;
  r->data = NULL;
  r->header = NULL;

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NetRequestPerform
 * Description...: 
 *                 
 * In............: NetRequest *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NetRequestPerform (NetRequest *r)
{
  int socket;
  char buffer[1024];
  int l, n;
  regmatch_t matches[4];
  char hostname[512];
  int port;
  char *line;
  FILE *f;
  NetRRHeader *header;
  regex_t urlExp;
  regex_t resultExp;
  regex_t headerExp;

  regcomp (&urlExp, "http://([^:/]+):?([0-9]+)?(/.*)",
    REG_ICASE | REG_EXTENDED);
  regcomp (&resultExp, "^HTTP/([0-9.]+) +([0-9]+)",
    REG_ICASE | REG_EXTENDED);
  regcomp (&headerExp, "^([^:]*): *(.*)", REG_ICASE | REG_EXTENDED);
  if (!regexec (&urlExp, r->url, 4, matches, 0))
  {
    l = matches[1].rm_eo - matches[1].rm_so;
    l = min (l, 511);
    memcpy (hostname, &r->url[matches[1].rm_so], l);
    hostname[l] = '\0';

    l = matches[2].rm_eo - matches[2].rm_so;
    if (l > 0)
    {
      l = min (l, 79);
      memcpy (buffer, &r->url[matches[2].rm_so], l);
      buffer[l] = '\0';
      port = atoi (buffer);
    }
    else
    {
      port = 80;
    }

    socket = openSocket (hostname, port);
    
    if (socket != -1)
    {
      f = fdopen (socket, "r+");
      l = 0;
      fputs (HTTPMethods[r->method], f);
      fputs (" ", f);
      fwrite (&r->url[matches[3].rm_so],
	1, matches[3].rm_eo - matches[3].rm_so, f);
      fputs (" HTTP/1.0\r\n", f);
      for (header = r->header; header != NULL; header = header->next)
      {
	fputs (header->name, f);
	fputs (": ", f);
	fputs (header->value, f);
	fputs ("\r\n", f);
      }
      fputs ("\r\n", f);
      if (r->data != NULL)
      {
	fputs (r->data, f);
      } 
      fflush (f); 
      
      line = readLine (f);
      if (!regexec (&resultExp, line, 3, matches, 0))
      {
	line[matches[2].rm_eo] = '\0';
	r->status = atoi (&line[matches[2].rm_so]);
      }
      free (line);

      do
      {
	line = readLine (f);
	l = strlen (line);
	if (!regexec (&headerExp, line, 3, matches, 0))
	{
	  if (strncasecmp (&line[matches[1].rm_so], "content-type", 12) == 0)
	  {
	    line[matches[2].rm_eo] = '\0';
	    r->contentType = XkNewString (&line[matches[2].rm_so]);
	  }
	}
	free (line);
      }
      while (l > 0);

      while ((n = fread (buffer, 1, 1024, f)) > 0)
      {
	r->data = (char *) realloc (r->data, l + n + 1);
	memcpy (r->data + l, buffer, n);
	l += n;
	r->data[l] = '\0';
      }

      fclose (f);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: NetRequestDelete
 * Description...: 
 *                 
 * In............: NetRequest *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NetRequestDelete (NetRequest *r)
{
  NetRRHeader *h;

  if (r->data) XkFree (r->data);
  if (r->contentType) XkFree (r->contentType);
  while (r->header != NULL)
  {
    h = r->header;
    r->header = r->header->next;
    if (h->name) XkFree (h->name);
    if (h->value) XkFree (h->value);
    XkFree (h);
  }

  XkFree (r);
}

/*-------------------------------------------------------------------------
 * Function......: NetGet
 * Description...: 
 *                 
 * In............: char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *NetGet (char *url)
{
  NetRequest *r;
  char *data;

  r = NetRequestNew (METHOD_GET, url);
  NetRequestPerform (r);
  data = r->data;
  r->data = NULL;
  NetRequestDelete (r);
  return data;
}

/*-------------------------------------------------------------------------
 * Function......: getContentType
 * Description...: 
 *                 
 * In............: char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *getContentType (char *fileName)
{
  int i;
  char *r;

  r = "text/plain";
  for (i = 0; i < sizeof (mimeTypes) / sizeof (mimeTypes[0]); i++)
  {
    if (strcmp (mimeTypes[i][0],
      &fileName[strlen (fileName) - strlen (mimeTypes[i][0])]) == 0)
    {
      r = mimeTypes[i][1];
    }
  }
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: readLine
 * Description...: 
 *                 
 * In............: FILE *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *readLine (FILE *f)
{
  char *r;
  int c;
  int l;

  r = (char *) malloc (256);
  l = 0;
  while ((c = fgetc (f)) != EOF && c != '\n')
  {
    if (c != '\r')
    {
      r[l++] = c;
      if (l % 256 == 0)
      {
	l += 256;
	r = (char *) realloc (r, l);
      }
    }
  }
  r[l] = '\0';
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: respondHEAD
 * Description...: 
 *                 
 * In............: NetServer *
 *                 FILE *
 *                 NetRRHeader *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void respondHEAD (NetServer *server, FILE *f, NetRRHeader *h,
  char *url, char *path)
{
}

/*-------------------------------------------------------------------------
 * Function......: respondGET
 * Description...: 
 *                 
 * In............: NetServer *
 *                 FILE *
 *                 NetRRHeader *
 *                 char *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void respondGET (NetServer *server, FILE *out, NetRRHeader *header,
  char *url, char *path, char *fragment, char *params)
{
  FILE *in;
  int c;
  Document *d;
  char *v;
  int r;

  r = 0;
  if (fragment != NULL)
  {
    d = AHDRuntimeActivate (server->ahdRuntime, path, url);
    v = AHDRuntimeGet (server->ahdRuntime,
      (Node *) DocumentGetNonanonymousRoot (d), fragment);
    if (v != NULL)
    {
      r = 1;
      fputs ("HTTP/1.0 200 OK\r\n", out);
      fputs ("Content-Type: text/plain", out);
      fputs ("\r\n\r\n", out);
      fputs (v, out);
    }
  }
  else
  {
    d = AHDRuntimeFindLoadedDocument (server->ahdRuntime, url);
    if (d != NULL)
    {
      r = 1;
      fputs ("HTTP/1.0 200 OK\r\n", out);
      fputs ("Content-Type: text/x-ahd", out);
      fputs ("\r\n\r\n", out);
      fputs (NodeToText ((Node *) d), out);
    }
    else
    {
      in = fopen (path, "r");
      if (in)
      {
	r = 1;
	fputs ("HTTP/1.0 200 OK\r\n", out);
	fputs ("Content-Type: ", out);
	fputs (getContentType (path), out);
	fputs ("\r\n\r\n", out);
	while ((c = fgetc (in)) != EOF)
	{
	  fputc (c, out);
	}
	fclose (in);
      }
    }
  }
  if (!r)
  {
    fputs ("HTTP/1.0 404 Not Found\r\n", out);
  }
}

/*-------------------------------------------------------------------------
 * Function......: respondPOST
 * Description...: 
 *                 
 * In............: NetServer *
 *                 FILE *
 *                 NetRRHeader *
 *                 char *
 *                 char *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void respondPOST (NetServer *server, FILE *out, NetRRHeader *header,
  char *url, char *path, char *fragment, char *params, char *data, int len)
{
  Document *d;
  char *r;

/*   if (fragment != NULL) */
/*   { */
    d = AHDRuntimeActivate (server->ahdRuntime, path, url);
    fputs ("HTTP/1.0 200 Ok\r\n", out);
    r = AHDRuntimeCall (server->ahdRuntime,
      (Node *) DocumentGetNonanonymousRoot (d),
      fragment != NULL ? fragment : "default",
      data);
    if (r != NULL) 
    {
      fputs (r, out);
      fflush (out);
    }
/*   } */
/*   else */
/*   { */
/*     fputs ("HTTP/1.0 501 Not implemented\r\n\r\n", out); */
/*   } */
}

/*-------------------------------------------------------------------------
 * Function......: respondPUT
 * Description...: 
 *                 
 * In............: NetServer *
 *                 FILE *
 *                 NetRRHeader *
 *                 char *
 *                 char *
 *                 char *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void respondPUT (NetServer *server, FILE *f, NetRRHeader *header,
  char *url, char *path, char *fragment, char *data, int len)
{
  Document *d;
  FILE *out;
  char *origin;
/*   char buffer[1024]; */

  if (fragment != NULL)
  {
    d = AHDRuntimeActivate (server->ahdRuntime, path, url);
    AHDRuntimePut (server->ahdRuntime,
      (Node *) DocumentGetNonanonymousRoot (d), fragment, data);
  }
  else
  {
    origin = getHeaderValue (header, "X-AHD-Origin");
    if (origin == NULL)
    {
/*       strcpy (buffer, server->ahdRuntime->here); */
/*       strcat (buffer, path); */
      origin = url;
      out = fopen (path, "w");
      fwrite (data, len, 1, out);
      fclose (out);
    }
    if (XkStrEqual (getHeaderValue (header, "Content-type"), "text/x-ahd"))
    {
      AHDRuntimeParseText (server->ahdRuntime, data, origin);
    }
  }

  XkFree (data);
}

/*-------------------------------------------------------------------------
 * Function......: processRequest
 * Description...: 
 *                 
 * In............: NetServer *
 *                 FILE *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void processRequest (NetServer *server, FILE *f)
{
  char *line;
  regmatch_t matches[5];
  int l, n, c, contentLength;
  char *method;
  char *path;
  char *fragment;
  char *params;
  char *data;
  char *url;
  NetRRHeader *header, *h;
  regex_t headerExp;
  regex_t requestExp;

  regcomp (&headerExp, "^([^:]*): *(.*)", REG_ICASE | REG_EXTENDED);
  regcomp (&requestExp, "(POST|GET|PUT|HEAD) ([^?# ]+)#?([^? ]*)[?]?([^ ]*)",
    REG_ICASE | REG_EXTENDED);
  line = readLine (f);
  if (!regexec (&requestExp, line, 5, matches, 0))
  {
    l = matches[1].rm_eo - matches[1].rm_so;
    method = (char *) malloc (l + 1);
    memcpy (method, &line[matches[1].rm_so], l);
    method[l] = '\0';
    
    l = matches[2].rm_eo - matches[2].rm_so;
    path = (char *) malloc (strlen (server->root) + l + 11);
    strcpy (path, server->root);
    strncat (path, &line[matches[2].rm_so], l);
    path[strlen (server->root) + l] = '\0';
    if (path[strlen (path)] == '/') strcat (path, "index.html");

    url = (char *) malloc (strlen (server->ahdRuntime->here) + l + 1);
    strcpy (url, server->ahdRuntime->here);
    strncat (url, &line[matches[2].rm_so + 1], l);
    url[strlen (server->ahdRuntime->here) + l - 1] = '\0';
    
    l = matches[3].rm_eo - matches[3].rm_so;
    if (l)
    {
      fragment = (char *) malloc (l + 1);
      memcpy (fragment, &line[matches[3].rm_so], l);
      fragment[l] = '\0';
    }
    else fragment = NULL;
    
    l = matches[4].rm_eo - matches[4].rm_so;
    if (l)
    {
      params = (char *) malloc (l + 1);
      memcpy (params, &line[matches[4].rm_so], l);
      params[l] = '\0';
    }
    else params = NULL;

    free (line);

    contentLength = 0;
    header = NULL;
    do
    {
      line = readLine (f);
      l = strlen (line);
      if (!regexec (&headerExp, line, 3, matches, 0))
      {
	h = XkNew (NetRRHeader);
	h->next = header;
	header = h;

	n = matches[1].rm_eo - matches[1].rm_so;
	h->name = (char *) malloc (n + 1);
	memcpy (h->name, &line[matches[1].rm_so], n);
	h->name[n] = '\0';

	n = matches[2].rm_eo - matches[2].rm_so;
	h->value = (char *) malloc (n + 1);
	memcpy (h->value, &line[matches[2].rm_so], n);
	h->value[n] = '\0';

	if (XkStrEqual (h->name, "Content-length"))
	{
	  contentLength = atoi (h->value);
	}
      }
      free (line);
    }
    while (l > 0);

    data = malloc (1024);
    l = 0;
    while (l < contentLength && (c = fgetc (f)) != EOF)
    {
      data[l] = (char ) c;
      l++;
      if (l % 1024 == 0)
      {
	data = realloc (data, l + 1024);
      }
    }
    data[l] = '\0';

    if (strcmp (method, "HEAD") == 0)
    {
      respondHEAD (server, f, header, url, path);
    }
    else if (strcmp (method, "GET") == 0)
    {
      respondGET (server, f, header, url, path, fragment, params);
    }
    else if (strcmp (method, "POST") == 0)
    {
      respondPOST (server, f, header, url, path, fragment, params, data, l);
    }
    else if (strcmp (method, "PUT") == 0)
    {
      respondPUT (server, f, header, url, path, fragment, data, l);
    }

    free (method);
    free (path);
    free (url);
    if (fragment) free (fragment);
    if (params) free (params);
    while (header != NULL)
    {
      h = header;
      header = header->next;
      XkFree (h->name);
      XkFree (h->value);
      XkFree (h);
    }
  }
  else
  {
    free (line);
  }
}

/*-------------------------------------------------------------------------
 * Function......: serverSubTask
 * Description...: 
 *                 
 * In............: void *
 * Out...........: void *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void *serverSubTask (void *arg)
{
  ServerTaskContext *context;
  FILE *f;

  context = (ServerTaskContext *) arg;
  f = fdopen (context->socket, "a+");
  processRequest (context->server, f);
  fclose (f);
  XkFree (context);
  return NULL;
}

/*-------------------------------------------------------------------------
 * Function......: serverTask
 * Description...: 
 *                 
 * In............: void *
 * Out...........: void *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void *serverTask (void *arg)
{
#ifdef HAVE_LIBPTHREAD
  int r;
  int childSocket;
  struct sockaddr_in addrmaster, addrchild;
  int addrchildlen;
  NetServer *server;
  ServerTaskContext *context;
  pthread_t subTask;

  server = (NetServer *) arg;
  server->masterSocket = socket (AF_INET, SOCK_STREAM, 0);

  addrmaster.sin_family = AF_INET ;
  addrmaster.sin_addr.s_addr = htonl (INADDR_ANY) ;
  addrmaster.sin_port = htons (server->port) ;

  setsockopt (server->masterSocket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof (1));
  r = bind (server->masterSocket,
    (struct sockaddr *) &addrmaster, sizeof (addrmaster));
  r = listen (server->masterSocket, 1);
  while (1)
  {
    childSocket = accept (server->masterSocket,
      (struct sockaddr *) &addrchild, &addrchildlen);

    context = XkNew (ServerTaskContext);
    context->server = server;
    context->socket = childSocket;
#ifdef THREADED_SERVER
    pthread_create (&subTask, NULL, serverSubTask, context);
#else
    serverSubTask (context);
#endif
  }
#endif
}

/*-------------------------------------------------------------------------
 * Function......: NetNetServerNew
 * Description...: 
 *                 
 * In............: int 
 *                 char *
 * Out...........: NetServer *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

NetServer *NetServerNew (int port, char *root, AHDRuntime *runtime)
{
  NetServer *r;

  r = (NetServer *) XkNew (NetServer);
  r->port = port;
  r->root = XkNewString (root);
  r->ahdRuntime = runtime;
  runtime->server = r;
#ifdef HAVE_LIBPTHREAD
#ifdef THREADED_SERVER
  pthread_create (&r->thread, NULL, serverTask, r);
#else
  serverTask (r);
#endif
#endif
  return r;
}

/*-------------------------------------------------------------------------
 * Function......: NetServerDelete
 * Description...: 
 *                 
 * In............: NetServer *
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void NetServerDelete (NetServer *server)
{
  shutdown (server->masterSocket, 0);
  close (server->masterSocket);
  XkFree (server->root);
  XkFree (server);
}

/*-------------------------------------------------------------------------
 * Function......: resolveURL
 * Description...: 
 *                 
 * In............: char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *resolveURL (char *base, char *url)
{
  return NULL;
}

/*-------------------------------------------------------------------------
 * Function......: openSocket
 * Description...: 
 *                 
 * In............: char *
 *                 unsigned 
 * Out...........: int 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int openSocket (char *hostname, unsigned short portnum)
{
  struct sockaddr_in sa;
  struct hostent *hp;
  int s;
  
  s = -1;
  hp = gethostbyname (hostname);
  if (hp != NULL)
  {
    memset (&sa,0,sizeof(sa));
    memcpy ((char *) &sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons ((u_short) portnum);
    
    s = socket (hp->h_addrtype, SOCK_STREAM, 0);
    if (s != -1)
    {
      connect (s, (struct sockaddr *) &sa, sizeof (sa));
    }
  }

  return s;
}

/*-------------------------------------------------------------------------
 * Function......: getHeaderValue
 * Description...: 
 *                 
 * In............: NetRRHeader *
 *                 char *
 * Out...........: char *
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *getHeaderValue (NetRRHeader *header, char *name)
{
  char *r;

  r = NULL;
  while (r == NULL && header != NULL)
  {
    if (XkStrEqual (header->name, name)) r = header->value;
    header = header->next;
  }

  return r;
}
