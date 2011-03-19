/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gtkkino.h"
#include "KinoParser.h"
#include "Layouter.h"
#include "Visual.h"

enum {
  CLICKED,
  LAST_SIGNAL
};

#define KINO_DEFAULT_WIDTH	600
#define KINO_DEFAULT_HEIGHT	400

#define KINO_GC(w) (w->container.widget.style->fg_gc[w->container.widget.state])

static void gtk_kino_class_init    (GtkKinoClass    *klass);
static void gtk_kino_realize       (GtkWidget *widget);
static void gtk_kino_init          (GtkKino         *kino);
static void gtk_kino_map           (GtkWidget        *widget);
static void gtk_kino_realize       (GtkWidget        *widget);
static void gtk_kino_size_request  (GtkWidget        *widget,
				     GtkRequisition   *requisition);
static void gtk_kino_size_allocate (GtkWidget        *widget,
				     GtkAllocation    *allocation);
static void gtk_kino_paint         (GtkWidget        *widget,
				     GdkRectangle     *area);
static void gtk_kino_draw          (GtkWidget        *widget,
				     GdkRectangle     *area);
static gint gtk_kino_expose        (GtkWidget        *widget,
				     GdkEventExpose   *event);
static void gtk_kino_add           (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_kino_remove        (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_kino_forall        (GtkContainer     *container,
				     gboolean 	       include_internals,
				     GtkCallback       callback,
				     gpointer          callback_data);
static GtkType gtk_kino_child_type (GtkContainer     *container);
static gint gtk_kino_button_release (GtkWidget      *widget,
			             GdkEventButton *event);


static void changeGC (GtkKino *, GdkFont *, KinoColor, KinoColor, int);
static void drawWord (GtkKino *, Element *,
  Text *, int, int, LargeRegion);
static void drawBox (GtkKino *, Element *, int, int, LargeRegion);
static void drawHRule (GtkKino *, Element *, int, int, int);

static void repositionInsets (GtkKino *k);
static void show (GtkKino *w, LargeRegion r, int move);

static guint kino_signals[LAST_SIGNAL] = { 0 };
static GtkContainerClass *parent_class = NULL;


GtkType
gtk_kino_get_type (void)
{
  static GtkType kino_type = 0;

  if (!kino_type)
    {
      static const GtkTypeInfo kino_info =
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

      kino_type = gtk_type_unique (GTK_TYPE_CONTAINER, &kino_info);
    }

  return kino_type;
}

static void
gtk_kino_class_init (GtkKinoClass *class)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  container_class = (GtkContainerClass*) class;

  parent_class = gtk_type_class (GTK_TYPE_CONTAINER);

  widget_class->map = gtk_kino_map;
  widget_class->realize = gtk_kino_realize;
  widget_class->size_request = gtk_kino_size_request;
  widget_class->size_allocate = gtk_kino_size_allocate;
  widget_class->draw = gtk_kino_draw;
  widget_class->expose_event = gtk_kino_expose;

  container_class->add = gtk_kino_add;
  container_class->remove = gtk_kino_remove;
  container_class->forall = gtk_kino_forall;
  container_class->child_type = gtk_kino_child_type;

  kino_signals[CLICKED] = gtk_signal_new ("clicked",
    GTK_RUN_FIRST | GTK_RUN_ACTION,
    object_class->type,
    GTK_SIGNAL_OFFSET (GtkKinoClass, clicked),
    gtk_marshal_NONE__NONE,
    GTK_TYPE_NONE, 0);

  gtk_object_class_add_signals (object_class, kino_signals, LAST_SIGNAL);

  widget_class->button_release_event = gtk_kino_button_release;
}

static GtkType
gtk_kino_child_type (GtkContainer     *container)
{
  return GTK_TYPE_WIDGET;
}

static void
gtk_kino_init (GtkKino *kino)
{
  GTK_WIDGET_UNSET_FLAGS (kino, GTK_NO_WINDOW);
 
  kino->document = NULL;
  kino->children = NULL;
}

GtkWidget*
gtk_kino_new (void)
{
  GtkKino *kino;

  kino = gtk_type_new (GTK_TYPE_KINO);
  return GTK_WIDGET (kino);
}

void
gtk_kino_put (GtkKino       *kino,
               GtkWidget      *widget,
               gint16         x,
               gint16         y)
{
  GtkKinoChild *child_info;

  g_return_if_fail (kino != NULL);
  g_return_if_fail (GTK_IS_KINO (kino));
  g_return_if_fail (widget != NULL);

  child_info = g_new (GtkKinoChild, 1);
  child_info->widget = widget;
  child_info->x = x;
  child_info->y = y;

  gtk_widget_set_parent (widget, GTK_WIDGET (kino));

  kino->children = g_list_append (kino->children, child_info); 

  if (GTK_WIDGET_REALIZED (kino))
    gtk_widget_realize (widget);

  if (GTK_WIDGET_VISIBLE (kino) && GTK_WIDGET_VISIBLE (widget))
    {
      if (GTK_WIDGET_MAPPED (kino))
        gtk_widget_map (widget);
      
      gtk_widget_queue_resize (GTK_WIDGET (kino));
    }
}

void
gtk_kino_move (GtkKino       *kino,
                GtkWidget      *widget,
                gint16         x,
                gint16         y)
{
  GtkKinoChild *child;
  GList *children;

  g_return_if_fail (kino != NULL);
  g_return_if_fail (GTK_IS_KINO (kino));
  g_return_if_fail (widget != NULL);

  children = kino->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (child->widget == widget)
        {
          child->x = x;
          child->y = y;

	  /*
	    if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (kino))
	    gtk_widget_queue_resize (GTK_WIDGET (kino));
	  */

          break;
        }
    }
}

static void
gtk_kino_map (GtkWidget *widget)
{
  GtkKino *kino;
  GtkKinoChild *child;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_KINO (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
  kino = GTK_KINO (widget);

  children = kino->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget) &&
	  !GTK_WIDGET_MAPPED (child->widget))
	gtk_widget_map (child->widget);
    }

  gdk_window_show (widget->window);
}

static void
gtk_kino_realize (GtkWidget *widget)
{
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_KINO (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
			    GDK_BUTTON_RELEASE_MASK);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, 
				   attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

static void
gtk_kino_size_request (GtkWidget      *widget,
			GtkRequisition *requisition)
{
  GtkKino *kino;  
  GtkKinoChild *child;
  GList *children;
  GtkRequisition child_requisition;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_KINO (widget));
  g_return_if_fail (requisition != NULL);

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

  children = kino->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
          gtk_widget_size_request (child->widget, &child_requisition);

          requisition->height = MAX (requisition->height,
                                     child->y +
                                     child_requisition.height);
          requisition->width = MAX (requisition->width,
                                    child->x +
                                    child_requisition.width);
	}
    }

  requisition->height += GTK_CONTAINER (kino)->border_width * 2;
  requisition->width += GTK_CONTAINER (kino)->border_width * 2;
}

static void
gtk_kino_size_allocate (GtkWidget     *widget,
			 GtkAllocation *allocation)
{
  GtkKino *kino;
  GtkKinoChild *child;
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  GList *children;
  guint16 border_width;
  Layouter *l;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_KINO(widget));
  g_return_if_fail (allocation != NULL);

  kino = GTK_KINO (widget);

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
			    allocation->x, 
			    allocation->y,
			    allocation->width, 
			    allocation->height);

  border_width = GTK_CONTAINER (kino)->border_width;
  
  children = kino->children;
  while (children)
    {
      child = children->data;
      children = children->next;
      
      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  gtk_widget_get_child_requisition (child->widget, &child_requisition);
	  child_allocation.x = child->x + border_width;
	  child_allocation.y = child->y + border_width;
	  child_allocation.width = child_requisition.width;
	  child_allocation.height = child_requisition.height;
	  gtk_widget_size_allocate (child->widget, &child_allocation);
	}
    }

  if (kino->document != NULL)
  {
    l = LayouterNew ();
    l->document = kino->document;
    l->document->root->width = allocation->width;
    LayouterReformat (l);
    LayouterDelete (l);
    kino->document->root->height = max (kino->document->root->height,
      allocation->height);
  }
}

static void
gtk_kino_paint (GtkWidget    *widget,
		 GdkRectangle *area)
{
  GtkKino *kino;
  LargeRegion r;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_KINO (widget));
  g_return_if_fail (area != NULL);

  if (GTK_WIDGET_DRAWABLE (widget))
  {
    kino = GTK_KINO (widget);
    gdk_window_clear_area (widget->window,
			   area->x, area->y,
			   area->width, area->height);
    r.x = area->x;
    r.y = area->y;
    r.width = area->width;
    r.height = area->height;
    if (kino->document)
    {
      show (kino, r, 0);
    }
  }
}

static void
gtk_kino_draw (GtkWidget    *widget,
		GdkRectangle *area)
{
  GtkKino *kino;
  GtkKinoChild *child;
  GdkRectangle child_area;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_KINO (widget));

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      kino = GTK_KINO (widget);
      gtk_kino_paint (widget, area);

      children = kino->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if (gtk_widget_intersect (child->widget, area, &child_area))
	    gtk_widget_draw (child->widget, &child_area);
	}
    }
}

static gint
gtk_kino_expose (GtkWidget      *widget,
		  GdkEventExpose *event)
{
  GtkKino *kino;
  GtkKinoChild *child;
  GdkEventExpose child_event;
  GList *children;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_KINO (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      kino = GTK_KINO (widget);
      gtk_kino_paint (widget, &event->area);

      child_event = *event;

      children = kino->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if (GTK_WIDGET_NO_WINDOW (child->widget) &&
	      gtk_widget_intersect (child->widget, &event->area, 
				    &child_event.area))
	    gtk_widget_event (child->widget, (GdkEvent*) &child_event);
	}
    }

  return FALSE;
}

static void
gtk_kino_add (GtkContainer *container,
	       GtkWidget    *widget)
{
  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_KINO (container));
  g_return_if_fail (widget != NULL);

  gtk_kino_put (GTK_KINO (container), widget, 0, 0);
}

static void
gtk_kino_remove (GtkContainer *container,
		  GtkWidget    *widget)
{
  GtkKino *kino;
  GtkKinoChild *child;
  GList *children;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_KINO (container));
  g_return_if_fail (widget != NULL);

  kino = GTK_KINO (container);

  children = kino->children;
  while (children)
    {
      child = children->data;

      if (child->widget == widget)
	{
	  gboolean was_visible = GTK_WIDGET_VISIBLE (widget);
	  
	  gtk_widget_unparent (widget);

	  kino->children = g_list_remove_link (kino->children, children);
	  g_list_free (children);
	  g_free (child);

	  if (was_visible && GTK_WIDGET_VISIBLE (container))
	    gtk_widget_queue_resize (GTK_WIDGET (container));

	  break;
	}

      children = children->next;
    }
}

static void
gtk_kino_forall (GtkContainer *container,
		  gboolean	include_internals,
		  GtkCallback   callback,
		  gpointer      callback_data)
{
  GtkKino *kino;
  GtkKinoChild *child;
  GList *children;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_KINO (container));
  g_return_if_fail (callback != NULL);

  kino = GTK_KINO (container);

  children = kino->children;
  while (children)
  {
    child = children->data;
    children = children->next;
    
    (* callback) (child->widget, callback_data);
  }
}

static gint
gtk_kino_button_release (GtkWidget      *widget,
			   GdkEventButton *event)
{
  GtkKino *kino;
  Element *element;
  EventHandlerInfo info;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_KINO (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (event->button == 1)
  {
    kino = GTK_KINO (widget);
    gtk_grab_remove (GTK_WIDGET (kino));
    info.event = (struct _KinoEvent *) event;
    info.reason = EE_CLICK;
    if (kino->document != NULL && kino->document->root != NULL)
    {
      element = LayouterFindBox (kino->document->root, event->x, event->y);
      info.source = (Node *) element;
      if (element != NULL)
      {
	DocumentCallEventHandler (kino->document, &info);
      }
    }
  }

  return TRUE;
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

  if (w->container.widget.window != NULL && w->document)
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
	gdk_draw_rectangle (w->container.widget.window, KINO_GC (w), 1,
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
	gdk_draw_line (w->container.widget.window, KINO_GC (w),
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
	gdk_draw_line (w->container.widget.window, KINO_GC (w),
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
	gdk_draw_line (w->container.widget.window, KINO_GC (w),
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
	gdk_draw_line (w->container.widget.window, KINO_GC (w),
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
  gdk_draw_line (w->container.widget.window, KINO_GC (w), x, y, x + len, y);
  gdk_draw_line (w->container.widget.window, KINO_GC (w), x, y, x, y + 1);

  /*  gdk_gc_set_foreground (KINO_GC (w), VisualLoadNamedColor ("gray95")); */
  gdk_draw_line (w->container.widget.window,
    KINO_GC (w), x + 1, y + 1, x + len, y + 1);
  gdk_draw_line (w->container.widget.window,
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
    gdk_draw_string (w->container.widget.window, (GdkFont *) box->font, KINO_GC (w),
      x, y, word->text);
    if (box->underline)
    {
      gdk_draw_line (w->container.widget.window, KINO_GC (w),
	x, y + 1, x + word->width, y + 1); 
    }
  }
}
