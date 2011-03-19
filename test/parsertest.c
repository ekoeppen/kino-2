#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <KinoParser.h>

void tagHandler (HandlerInfo *i)
{
  printf ("Parser: %p, Tag: %s, Userdata: %p\n",
    i->info.tag->parser, 
    i->info.tag->name, 
    i->userData);
}

void linkHandler (HandlerInfo *i)
{
  printf ("Parser: %p, Link: %s, Userdata: %p\n",
    i->info.link->parser,
    AttributeGetValue (i->info.link->element->attributes, "XML:LINK"),
    i->userData);
}

void main (int argc, char *argv[])
{
  Parser *p;
  Document *d;
  char *t;
  char *text;
  int h;
  struct stat stats;

  d = DocumentNew ();
  d->root = ElementNew ();
  d->root->anonymous = ANON_BOX;
  d->root->document = d;
  DocumentAddTagHandler (d, tagHandler, NULL);
  DocumentAddLinkHandler (d, linkHandler, NULL);

  p = ParserNew ();
  p->document = d;
  p->startElement = d->root;
  if (argc > 1)
  {
    lstat (argv[1], &stats);
    text = malloc (stats.st_size + 1);
    h = open (argv[1], O_RDONLY);
    read (h, text, stats.st_size);
    close (h);
    text[stats.st_size] = '\0';
  }
  else
  {
    text = XkNewString ("<h1>Hello World <![CDATA[Hello, world]]></h1>");
  }
  ParserProcessData (p, "text/html", text);
  free (text);

  NodePrint (d->root, 0);

  t = NodeToText (d);
  puts (t);
  free (t);
  NodeDelete ((Node *) d);
  ParserDelete (p);
}
