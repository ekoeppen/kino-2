/*-------------------------------------------------------------------------
 * File..........: 
 * Description...: 
 *
 * $Id: notext.c,v 1.3 1999/03/02 12:25:56 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
/* #include <Xm/ScrolledW.h> */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Kino.h>
#include <KinoP.h>

static Widget toplevel, viewport;
static KinoWidget kino;
static void processArgs (int argc, char *argv[]);
static void doText (char *text);
static void linkHandler (HandlerInfo *);

void linkHandler (HandlerInfo *i)
{
  ParserProcessData (i->info.link->parser, "", "text/xml");
}

void doText (char *text)
{
  Parser *p;
  Document *d;

  d = DocumentNew ();
  d->root = ElementNew ();
  d->root->display = DISPLAY_BLOCK;
  d->root->floating = FLOAT_NONE;
  d->root->anonymous = ANON_BOX;
  d->root->document = d;
  DocumentAddLinkHandler (d, linkHandler, NULL);

  p = ParserNew ();

  p->document = d;
  p->startElement = d->root;

  ParserProcessData (p, "text/html", text);
  ParserDelete (p);

  kino->kino.document = d;
  KinoRelayout ((Widget) kino);
}

void processArgs (int argc, char *argv[])
{
  char *text;
  int h;
  struct stat stats;
  Arg args[10];

  if (argc > 1)
  {
    if (strcmp (argv[1], "-f") == 0)
    {
      lstat (argv[2], &stats);
      text = malloc (stats.st_size + 1);
      h = open (argv[2], O_RDONLY);
      read (h, text, stats.st_size);
      close (h);
      text[stats.st_size] = '\0';
      doText (text);
      free (text);
    }
    else if (strcmp (argv[1], "-t") == 0)
    {
    }
    else
    {
      lstat (argv[1], &stats);
      text = malloc (stats.st_size + 1);
      h = open (argv[1], O_RDONLY);
      read (h, text, stats.st_size);
      close (h);
      text[stats.st_size] = '\0';
      doText (text);
      free (text);
    }

  }
  else
  {
    lstat ("test.html", &stats);
    text = malloc (stats.st_size + 1);
    h = open ("test.html", O_RDONLY);
    read (h, text, stats.st_size);
    close (h);
    text[stats.st_size] = '\0';
    doText (text);
    free (text);
  }
}

int main (int argc, char *argv[])
{
  Arg args[10];

  setbuf (stdout, NULL);
  toplevel = XtInitialize (argv[0], "notext", NULL, 0, &argc, argv);

  XtSetArg (args[0], XtNallowVert, True);
  XtSetArg (args[1], XtNforceBars, True);
  viewport = XtCreateManagedWidget ("viewport", viewportWidgetClass,
    toplevel, args, 2); 

  kino = (KinoWidget) XtCreateManagedWidget ("kino", kinoWidgetClass,
    viewport, NULL, 0);

  XtRealizeWidget (toplevel);

  processArgs (argc, argv);

  XtMainLoop ();

  return 0;
}
