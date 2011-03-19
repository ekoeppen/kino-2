/*-------------------------------------------------------------------------
 * File..........: 
 * Description...: 
 *
 * $Id$
 *-----------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "gtkkino.h"
#include "HTMLFormTags.h"
#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"
#include <pthread.h>
#include <Net.h>
#include <AHDRuntime.h>

static GtkWidget *kino;
static AHDRuntime *runtime;

static void processArgs (int argc, char *argv[]);
static void doText (char *text);
static void linkHandler (HandlerInfo *);
static void tagHandler (HandlerInfo *);
static void eventHandler (HandlerInfo *);

static void quitCallback (GtkWidget *w, gpointer data)
{
  if (GTK_KINO (kino)->document != NULL)
  {
    NodeDelete (GTK_KINO (kino)->document);
    GTK_KINO (kino)->document = NULL;
  }
  AHDRuntimeDelete (runtime);
  gtk_main_quit ();
}

static void resizeCallback (GtkWidget *w, gpointer data)
{
}

void tagHandler (HandlerInfo *i)
{
  GtkWidget *t;

  if (XkStrEqual (i->info.tag->name, "label"))
  {
    t = gtk_label_new ("Label");
    gtk_kino_put (GTK_KINO (kino), t, 0, 0);
    gtk_widget_show (t);

    ParserAddInset (i->info.tag->parser, t);
  }
  else if (XkStrEqual (i->info.tag->name, "button"))
  {
    t = gtk_button_new_with_label ("Button");
    gtk_kino_put (GTK_KINO (kino), t, 0, 0);
    gtk_widget_show (t);

    ParserAddInset (i->info.tag->parser, t);
  }
  else if (XkStrEqual (i->info.tag->name, "entry"))
  {
    t = gtk_entry_new ();
    gtk_kino_put (GTK_KINO (kino), t, 0, 0);
    gtk_widget_show (t);

    ParserAddInset (i->info.tag->parser, t);
  }
  else if (XkStrEqual (i->info.tag->name, "textfield"))
  {
    t = gtk_text_new (NULL, NULL);
    gtk_text_set_editable (t, TRUE);
    gtk_kino_put (GTK_KINO (kino), t, 0, 0);
    gtk_widget_show (t);

    ParserAddInset (i->info.tag->parser, t);
  }
}

void linkHandler (HandlerInfo *i)
{
  ParserProcessData (i->info.link->parser, "text/xml", "");
}

void eventHandler (HandlerInfo *i)
{
  Layouter *l;
  GtkWidget *kino;
  GdkRectangle r;

  if (i->info.event->reason == EE_MOD_SUBTREE)
  {
    kino = (GtkWidget *) i->userData;

    l = LayouterNew ();
    l->document = GTK_KINO (kino)->document;
    LayouterReformat (l);
    LayouterDelete (l);

    gtk_widget_set_usize (kino, kino->allocation.x, kino->allocation.y);

/*     r.x = kino->allocation.x; */
/*     r.y = kino->allocation.y; */
/*     r.width = kino->allocation.width; */
/*     r.height = kino->allocation.height; */
/*     gtk_widget_draw (kino, &r); */
  }
}

void doText (char *text)
{
  Parser *p;
  Document *d;
  Layouter *l;

  d = DocumentNew ();
  d->root = ElementNew ();
  d->root->display = DISPLAY_BLOCK;
  d->root->floating = FLOAT_NONE;
  d->root->anonymous = ANON_BOX;
  d->root->document = d;
  d->root->width = 600;
  DocumentAddLinkHandler (d, linkHandler, NULL);
/*   DocumentAddTagHandler (d, tagHandler, NULL); */
  AHDRuntimeDocumentInit (runtime, d);

  p = ParserNew ();

  p->document = d;
  p->startElement = d->root;

  ParserProcessData (p, "text/html", text);
  ParserDelete (p);

  l = LayouterNew ();
  l->document = d;
  LayouterReformat (l);
  LayouterDelete (l);
  GTK_KINO (kino)->document = d;
  d->userData = kino;
}

void processArgs (int argc, char *argv[])
{
  char *text;
  int h;
  struct stat stats;

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
  GtkWidget *window;
  GtkWidget *scrolled_win;
  char root[1024];

  setbuf (stdout, NULL);

  strcpy (root, getenv ("HOME"));
  strcat (root, "/public_html");

  gtk_init (&argc, &argv);
    
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize (window, 700, 400);
  gtk_widget_set_uposition (window, 0, 0);
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  kino = gtk_kino_new ();

  gtk_signal_connect (GTK_OBJECT (window),
    "destroy", GTK_SIGNAL_FUNC (quitCallback), NULL);

  /*  gtk_signal_connect (GTK_OBJECT (kino),
      "size_allocate", GTK_SIGNAL_FUNC (resizeCallback), NULL); */

  gtk_container_add (GTK_CONTAINER (window), scrolled_win);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_win),
    kino);
    
  runtime = AHDRuntimeNew (9999, root);
  AHDRuntimeAddEventHandler (runtime, eventHandler, kino);
  AHDRuntimeAddTagHandler (runtime, HTMLFormTagsHandler, kino);

  processArgs (argc, argv);

  gtk_widget_show (kino);
  gtk_widget_show (scrolled_win);
  gtk_widget_show (window);

  gtk_main ();
                        
  return 0;
}
