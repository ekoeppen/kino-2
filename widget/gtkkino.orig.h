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

#ifndef __GTK_KINO_H__
#define __GTK_KINO_H__


#include <gdk/gdk.h>
#include <gtk/gtkmisc.h>
#include <gtk/gtkfixed.h>
#include "KinoParser.h"


#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */


#define GTK_TYPE_KINO			 (gtk_kino_get_type ())
#define GTK_KINO(obj)			 (GTK_CHECK_CAST ((obj), GTK_TYPE_KINO, GtkKino))
#define GTK_KINO_CLASS(class)		 (GTK_CHECK_CLASS_CAST ((class), GTK_TYPE_KINO, GtkKinoClass))
#define GTK_IS_KINO(obj)		 (GTK_CHECK_TYPE ((obj), GTK_TYPE_KINO))
#define GTK_IS_KINO_CLASS(class)	 (GTK_CHECK_CLASS_TYPE ((class), GTK_TYPE_KINO))


typedef struct _GtkKino	GtkKino;
typedef struct _GtkKinoClass	GtkKinoClass;

struct _GtkKino
{
  GtkWidget widget;

  GtkFixed *fixed;
  Document *document;
};

struct _GtkKinoClass
{
  GtkMiscClass parent_class;
};


GtkType	   gtk_kino_get_type	 (void);
GtkWidget* gtk_kino_new	         (void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_KINO_H__ */
