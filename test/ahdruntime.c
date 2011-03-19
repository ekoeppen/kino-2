#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <KinoParser.h>

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#ifdef NATIVE_NETWORKING
#include <Net.h>
#endif

#include <AHDRuntime.h>

#ifdef HAVE_LIBTCL8_0
#include <tcl.h>
#endif

#ifdef HAVE_LIBLUA
#include <lua.h>
#endif

static AHDRuntime *runtime;
static Document *parseText (char *);

Document *parseText (char *text)
{
  Parser *p;
  Document *d;

  d = DocumentNew ();
  AHDRuntimeDocumentInit (runtime, d);
  d->root = ElementNew ();
  d->root->namespace = XkNewString ("kino2");
  d->root->display = DISPLAY_BLOCK;
  d->root->floating = FLOAT_NONE;
  d->root->anonymous = ANON_BOX;
  d->root->document = d;

  p = ParserNew ();

  p->document = d;
  p->startElement = d->root;

  ParserProcessData (p, "text/xml", text);
  ParserDelete (p);

  return d;
}

void main (int argc, char *argv[])
{
  char *text;
  int h;
  struct stat stats;
  Document *d;
  char root[1024];

/*   malloc_mutex_init (); */

  setbuf (stdout, NULL);

  strcpy (root, getenv ("HOME"));
  strcat (root, "/public_html");
  runtime = AHDRuntimeNew (9998, root);

  if (argc > 1)
  {
    lstat (argv[1], &stats);
    text = malloc (stats.st_size + 1);
    h = open (argv[1], O_RDONLY);
    read (h, text, stats.st_size);
    close (h);
    text[stats.st_size] = '\0';
    d = parseText (text);
    AHDRuntimeBroadcastCall (runtime, (Node *) d, "onload", NULL);
    free (text);
  }
  getchar ();
  AHDRuntimeDelete (runtime);
}
