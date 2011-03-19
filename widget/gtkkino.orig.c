/*-------------------------------------------------------------------------
 * gtkkino - an XML display widget, part of the Kino XML/CSS processor
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

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gtkkino.h"
#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"

#define KINO_DEFAULT_WIDTH	600
#define KINO_DEFAULT_HEIGHT	400

#define KINO_GC(w) (w->widget.style->fg_gc[w->widget.state])

static void gtk_kino_class_init (GtkKinoClass    *klass);
static void gtk_kino_init       (GtkKino         *kino);
static gint gtk_kino_expose     (GtkWidget       *widget,
				 GdkEventExpose  *event);
static void gtk_kino_finalize   (GtkObject       *object);
static void gtk_kino_size_request (GtkWidget *widget,
                                   GtkRequisition *requisition);

static void changeGC (GtkKino *, GdkFont *, KinoColor, KinoColor, int);
static void drawWord (GtkKino *, Element *,
  Text *, int, int, LargeRegion);
static void drawBox (GtkKino *, Element *, int, int, LargeRegion);
static void drawHRule (GtkKino *, Element *, int, int, int);

static void repositionInsets (GtkKino *k);
static void show (GtkKino *w, LargeRegion r, int move);

static GtkWidgetClass *parent_class;

GtkType
gtk_kino_get_type (void)
{
  static GtkType kino_type = 0;

  if (!kino_type)
    {
      GtkTypeInfo kino_info =
      {
	"GtkKino",
	sizeof (GtkKino),
	sizeof (GtkKinoClass),
	(GtkClassInitFunc) gtk_kino_class_init,
	(GtkObjectInitFunc) gtk_kino_init,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      kino_type = gtk_type_unique (gtk_widget_get_type (), &kino_info);
    }

  return kino_type;
}

static void
gtk_kino_class_init (GtkKinoClass *class)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  parent_class = gtk_type_class (gtk_widget_get_type ());

  object_class->finalize = gtk_kino_finalize;
  widget_class->expose_event = gtk_kino_expose;
  widget_class->size_request = gtk_kino_size_request;
}

static void
gtk_kino_init (GtkKino *kino)
{
  GTK_WIDGET_SET_FLAGS (kino, GTK_NO_WINDOW);
  kino->document = NULL;
}

GtkWidget*
gtk_kino_new (void)
{
  GtkKino *kino;

  kino = gtk_type_new (gtk_kino_get_type ());
  kino->fixed = GTK_FIXED (gtk_fixed_new ());

  return GTK_WIDGET (kino);
}

static void
gtk_kino_finalize (GtkObject *object)
{
  (* GTK_OBJECT_CLASS (parent_class)->finalize) (object);
}

static gint
gtk_kino_expose (GtkWidget *widget, GdkEventExpose *event)
{
  GtkKino *kino;
  LargeRegion r;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_KINO (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      kino = GTK_KINO (widget);

      r.x = event->area.x;
      r.y = event->area.y;
      r.width = event->area.width;
      r.height = event->area.height;
      if (kino->document)
      {
	gdk_window_clear_area (widget->window,
	  event->area.x, event->area.y,
	  event->area.width, event->area.height);
	show (kino, r, 0);
      }
    }

  return FALSE;
}

static void 
gtk_kino_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  GtkKino *kino;

  kino = GTK_KINO (widget);
  requisition->width = KINO_DEFAULT_WIDTH;
  if (kino->document != NULL && kino->document->root != NULL)
  {
    requisition->height = kino->document->root->height;
  }
  else
  {
    requisition->height = KINO_DEFAULT_HEIGHT;
  }
}

/*-------------------------------------------------------------------------
 * Function......: PainterRepositionInsets
 * Description...: Move all insets to their correct position
 *
 * In............: GtkKino: widget containing the insets
 * Out...........: -
 * Precondition..: -
 * Postcondition.: all insets have been moved to their correct position
 *-----------------------------------------------------------------------*/

void repositionInsets (GtkKino *k)
{
}

/*-------------------------------------------------------------------------
 * Function......: PainterShow
 * Description...: Redisplay the visible region of the widget
 *
 * In............: GtkKino: the Kino widget
 *                 LargeRegion: region to be displayed
 *                 int: move the insets?
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void show (GtkKino *w, LargeRegion r, int move)
{
#ifdef DEBUG
  fprintf (stderr, "+++ %s: (%d|%d)-(%dx%d)\n", __PRETTY_FUNCTION__,
    r.x, r.y, r.width, r.height);
#endif

  if (w->widget.window != NULL && w->document)
  {
    /* move all insets to their new places */
  
    if (move) repositionInsets (w);

    /* draw all Node elements */

    drawBox (w, w->document->root, 0, 0, r);
  }
}

/*-------------------------------------------------------------------------
 * Function......: drawBox
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void drawBox (GtkKino *w, Element *box, int x, int y, LargeRegion r)
{
  Node *p;
  GdkRectangle re, rb, intersect;

  if (box != NULL && box->display != DISPLAY_NONE &&
    (box->display == DISPLAY_INLINE ||
    !(x + box->x + box->width < r.x || x + box->x > r.x + r.width ||
      y + box->y + box->height < r.y || y + box->y > r.y + r.height)))
  {
    x = x + box->x;
    y = y + box->y;

    re.x = r.x; re.y = r.y; re.width = r.width; re.height = r.height;
    gdk_gc_set_clip_rectangle (KINO_GC (w), &re);

    if (box->display == DISPLAY_BLOCK)
    {
      if (box->background_color.alpha == 0)
      {
	changeGC (w, ((GdkFont *) box->font),
	  box->background_color,
	  box->background_color, 0);

	rb.x = x + ElementTotalLeft (box);
	rb.y = y + ElementTotalTop (box);
	rb.width = box->width -
	  ElementTotalLeft (box) - ElementTotalRight (box);
	rb.height = box->height -
	  ElementTotalTop (box) - ElementTotalBottom (box);

	gdk_rectangle_intersect (&re, &rb, &intersect);

	gdk_gc_set_clip_rectangle (KINO_GC (w), &intersect);
	gdk_draw_rectangle (w->widget.window, KINO_GC (w), 1,
	  r.x, r.y, r.width, r.height); 
	gdk_gc_set_clip_rectangle (KINO_GC (w), &re);
      }

      if (box->border_top_color.alpha == 0 &&
	box->border_top_width.effective > 0)
      {
	changeGC (w, (GdkFont *) box->font,
	  box->border_top_color,
	  box->background_color,
	  box->border_top_width.effective);
	gdk_draw_line (w->widget.window, KINO_GC (w),
	  x + box->margin_left.effective, 
	  y + box->margin_top.effective,
	  x + box->width - box->margin_right.effective,
	  y + box->margin_top.effective);
      }

      if (box->border_bottom_color.alpha == 0 &&
	box->border_bottom_width.effective > 0)
      {
	changeGC (w, (GdkFont *) box->font,
	  box->border_bottom_color,
	  box->background_color,
	  box->border_bottom_width.effective);
	gdk_draw_line (w->widget.window, KINO_GC (w),
	  x + box->margin_left.effective, 
	  y + box->height - box->margin_bottom.effective,
	  x + box->width - box->margin_right.effective,
	  y + box->height - box->margin_bottom.effective);
      }

      if (box->border_left_color.alpha == 0 &&
	box->border_left_width.effective > 0)
      {
	changeGC (w, (GdkFont *) box->font,
	  box->border_left_color,
	  box->background_color,
	  box->border_left_width.effective);
	gdk_draw_line (w->widget.window, KINO_GC (w),
	  x + box->margin_left.effective, 
	  y + box->margin_top.effective,
	  x + box->margin_left.effective, 
	  y + box->height - box->margin_bottom.effective);
      }

      if (box->border_right_color.alpha == 0 &&
	box->border_right_width.effective > 0)
      {
	changeGC (w, (GdkFont *) box->font,
	  box->border_right_color,
	  box->background_color,
	  box->border_right_width.effective);
	gdk_draw_line (w->widget.window, KINO_GC (w),
	  x + box->width - box->margin_right.effective, 
	  y + box->margin_top.effective,
	  x + box->width - box->margin_right.effective, 
	  y + box->height - box->margin_bottom.effective);
      }
    }

    changeGC (w, (GdkFont *) box->font,
      box->color, box->background_color, 1);

    p = box->contents;
    while (p != NULL)
    {
      switch (p->type)
      {
	case TEXT_NODE:
	  drawWord (w, box, (Text *) p, x, y, r);
	  break;
	case ELEMENT_NODE:
	  drawBox (w, (Element *) p, x, y, r);
	  changeGC (w, (GdkFont *) box->font,
	    box->color, box->background_color, 1);
	  break;
	default: 
	  break;
      }
      p = p->next;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: changeGC
 * Description...: Change the current graphics context according to a
 *                 given style
 *
 * In............: GtkKino: the Kino widget containing the GC
 *                 Box *: box containing the new style
 * Out...........: -
 * Precondition..: the style must contain valid font data and the
 *                 graphics context must be valid
 * Postcondition.: the graphics context has been changed
 *-----------------------------------------------------------------------*/

void changeGC (GtkKino *w, GdkFont *font,
  KinoColor f, KinoColor b, int lineWidth)
{
  GdkColor c;

  gdk_gc_set_font (KINO_GC (w), font);

  c.red = f.red; c.green = f.green; c.blue = f.blue;
  gdk_color_alloc (gdk_colormap_get_system (), &c);
  gdk_gc_set_foreground (KINO_GC (w), &c);

  c.red = b.red; c.green = b.green; c.blue = b.blue;
  gdk_color_alloc (gdk_colormap_get_system (), &c);
  gdk_gc_set_background (KINO_GC (w), &c);

  gdk_gc_set_line_attributes (KINO_GC (w), lineWidth,
    GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
}

/*-------------------------------------------------------------------------
 * Function......: drawHRule
 * Description...: Draw a horzontal rule with shadows
 *
 * In............: GtkKino: the Kino widget
 *                 Style *: style for the rule (color)
 *                 int: horizontal position
 *                 int: vertical position
 *                 int: length of the rule
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void drawHRule (GtkKino *w, Element *box, int x, int y, int len)
{
  /*  gdk_gc_set_foreground (KINO_GC (w), VisualLoadNamedColor ("gray10")); */
  gdk_draw_line (w->widget.window, KINO_GC (w), x, y, x + len, y);
  gdk_draw_line (w->widget.window, KINO_GC (w), x, y, x, y + 1);

  /*  gdk_gc_set_foreground (KINO_GC (w), VisualLoadNamedColor ("gray95")); */
  gdk_draw_line (w->widget.window,
    KINO_GC (w), x + 1, y + 1, x + len, y + 1);
  gdk_draw_line (w->widget.window,
    KINO_GC (w), x + len, y, x + len, y + 1);
  /* gdk_gc_set_foreground (KINO_GC (w), box->color); */
}

/*-------------------------------------------------------------------------
 * Function......: drawWord
 * Description...: Draw a word
 *
 * In............: GtkKino: the Kino widget
 *                 Element *: 
 *                 Text *: 
 * Out...........: -
 * Precondition..: -
 * Postcondition.: -
 *-----------------------------------------------------------------------*/

void drawWord (GtkKino *w, Element *box,
  Text *word, int x, int y, LargeRegion r)
{
  x += word->x;
  y += word->y;
  if (!(x + word->width < r.x || x > r.x + r.width ||
    y - VisualFontAscent (box->font) + word->height < r.y ||
    y - VisualFontAscent (box->font) > r.y + r.height))
  {
    gdk_draw_string (w->widget.window, (GdkFont *) box->font, KINO_GC (w),
      x, y, word->text);
    if (box->underline)
    {
      gdk_draw_line (w->widget.window, KINO_GC (w),
	x, y + 1, x + word->width, y + 1); 
    }
  }
}
