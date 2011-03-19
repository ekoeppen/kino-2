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
#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"
#include <pthread.h>
#include <Net.h>
#include <AHDRuntime.h>

static void processArgs (int argc, char *argv[]);
static Document *parseText (char *text);
static void linkHandler (HandlerInfo *);
static void fillAHDList (void);

static void quitCallback (GtkWidget *w, gpointer data);
static void refreshCallback (GtkWidget *w, gpointer data);
static void dumpCallback (GtkWidget *w, gpointer data);
static void unloadCallback (GtkWidget *w, gpointer data);
static void selectCallback (GtkWidget *w,
  gint row, gint col, GdkEventButton *event, gpointer data);

static AHDRuntime *runtime;
static GtkWidget *window, *mainPane, *ahdList, *buttonPane, *scrolledWin;
static LoadedDocument *selection = NULL;

char *entries[] =
{
  "eins"
};

void quitCallback (GtkWidget *w, gpointer data)
{
  AHDRuntimeDelete (runtime);
  gtk_main_quit ();
}

void refreshCallback (GtkWidget *w, gpointer data)
{
  fillAHDList ();
}

void selectCallback (GtkWidget *w,
  gint row, gint col, GdkEventButton *event, gpointer data)
{
  selection = (LoadedDocument *) gtk_clist_get_row_data (GTK_CLIST (w),
    row);
}

void dumpCallback (GtkWidget *w, gpointer data)
{
  char *t;

  if (selection != NULL)
  {
    NodePrint (selection->document->root, 0);
    /*
      t = NodeToText ((Node *) selection->document);
      if (t != NULL)
      {
      puts (t);
      free (t);
      }
    */
  }
}

void unloadCallback (GtkWidget *w, gpointer data)
{
  if (selection != NULL)
  {
    AHDRuntimeUnload (runtime, selection->document);
    fillAHDList ();
  }
}

void linkHandler (HandlerInfo *i)
{
  ParserProcessData (i->info.link->parser, "", "text/xml");
}

Document *parseText (char *text)
{
  Parser *p;
  Document *d;

  d = DocumentNew ();
  AHDRuntimeDocumentInit (runtime, d);
  DocumentAddLinkHandler (d, linkHandler, NULL);
  d->root = ElementNew ();
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

void processArgs (int argc, char *argv[])
{
  char *text;
  int h;
  struct stat stats;
  Document *d;

  if (argc > 1)
  {
    lstat (argv[1], &stats);
    text = malloc (stats.st_size + 1);
    h = open (argv[1], O_RDONLY);
    read (h, text, stats.st_size);
    close (h);
    text[stats.st_size] = '\0';
    d = parseText (text);
    free (text);
    AHDRuntimeBroadcastCall (runtime, (Node *) d, "onload", NULL);
  }
}

void fillAHDList (void)
{
  LoadedDocument *l;
  char *entry[1];
  gint row;

  gtk_clist_clear (GTK_CLIST (ahdList));
  selection = NULL;
  for (l = runtime->loadedDocuments; l != NULL; l = l->next)
  {
    entry[0] = l->url;
    row = gtk_clist_append (GTK_CLIST (ahdList), entry);
    gtk_clist_set_row_data (GTK_CLIST (ahdList), row, l);
  }
}

void setupWidgets (void)
{
  GtkWidget *button;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (window),
    "destroy", GTK_SIGNAL_FUNC (quitCallback), NULL);
  gtk_widget_set_usize (GTK_WIDGET (window), 300, 150);

  mainPane = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (window), mainPane);

  buttonPane = gtk_vbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (mainPane), buttonPane, FALSE, TRUE, 0);
  gtk_widget_show (buttonPane);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonPane), 2);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonPane), GTK_BUTTONBOX_START);

  button = gtk_button_new_with_label ("Refresh");
  gtk_container_add (GTK_CONTAINER (buttonPane), button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
    GTK_SIGNAL_FUNC (refreshCallback), NULL);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Dump");
  gtk_container_add (GTK_CONTAINER (buttonPane), button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
    GTK_SIGNAL_FUNC (dumpCallback), NULL);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Unload");
  gtk_container_add (GTK_CONTAINER (buttonPane), button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
    GTK_SIGNAL_FUNC (unloadCallback), NULL);
  gtk_widget_show (button);

  scrolledWin = gtk_scrolled_window_new (NULL, NULL);
  gtk_box_pack_start (GTK_BOX (mainPane), scrolledWin, TRUE, TRUE, 0);
  gtk_widget_show (scrolledWin);

  ahdList = gtk_clist_new (1);
  gtk_container_add (GTK_CONTAINER (scrolledWin), ahdList);
  gtk_signal_connect (GTK_OBJECT (ahdList), "select_row",
    GTK_SIGNAL_FUNC (selectCallback), NULL);
  gtk_clist_set_column_width (GTK_CLIST (ahdList), 0, 200);
  gtk_widget_show (ahdList);
    
  gtk_widget_show (mainPane);
  gtk_widget_show (window);

}

int main (int argc, char *argv[])
{
  char *root;

/*   malloc_mutex_init (); */

  setbuf (stdout, NULL);

  root = malloc (strlen (getenv ("HOME")) + 20);

  strcpy (root, getenv ("HOME"));
  strcat (root, "/public_html");

  runtime = AHDRuntimeNew (9998, root);

  processArgs (argc, argv);

  gtk_init (&argc, &argv);

  setupWidgets ();
    
  gtk_main ();
                        
  return 0;
}
