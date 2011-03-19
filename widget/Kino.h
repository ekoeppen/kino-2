/*-------------------------------------------------------------------------
 * kinowidget - an XML display widget, part of the Kino XML/CSS processor
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
 * File..........: Kino.c
 * Description...: Declaration of the public part of the Kino widget
 *-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * $Id: Kino.h,v 1.3 1999/04/14 14:34:01 koeppen Exp $
 *
 * $Log: Kino.h,v $
 * Revision 1.3  1999/04/14 14:34:01  koeppen
 * renamed startBox and box in the parser structure, added LGPL comments
 *
 * Revision 1.2  1999/03/09 17:48:11  koeppen
 * removed Xk prefix from widget records
 *
 * Revision 1.1.1.1  1998/12/21 17:16:44  koeppen
 * initial checkin
 *
 * Revision 1.4  1998/12/01 20:49:42  koeppen
 * Vorbereitung für Komponenten
 *
 * Revision 1.3  1998/04/23 14:02:56  koeppen
 * Versionsnummer neu
 *
 * Revision 1.2  1998/02/18 13:03:20  koeppen
 * Xk... entfernt
 *
 * Revision 1.1  1998/01/14 16:05:18  koeppen
 * Initial revision
 *
 * Revision 2.0  1997/06/14 20:54:42  root
 * Vorbereitung fuer XML und CSS
 *
 *-----------------------------------------------------------------------*/

#ifndef __KINO_H
#define __KINO_H

/* global definitions */

/* resource names */

#define XtNtext              "text"
#define XtNhtmlTitle         "title"
#define XtNfontFamily        "fontFamily"
#define XtNfontSize          "fontSize"
#define XtNdefaultLMargin    "defaultLMargin"
#define XtNdefaultRMargin    "defaultRMargin"
#define XtNfirstLine         "firstLine"
#define XtNfirstColumn       "firstColumn"
#define XtNtotalLines        "totalLines"
#define XtNtotalColumns      "totalColumns"
#define XtNtagCallback       "tagCallback"
#define XtNscriptCallback    "scriptCallback"
#define XtNeventCallback     "eventCallback"
#define XtNlinkCallback      "linkCallback"
#define XtNtagCallback       "tagCallback"
#define XtNpositionCallback  "positionCallback"
#define XtNsizeCallback      "sizeCallback"
#define XtNdefaultAutoFormat "defaultAutoFormat"
#define XtNautoFormat        "autoFormat"
#define XtNfontScaling       "fontScaling"
#define XtNpainterEnable     "painterEnable"
#define XtNlayouterEnable    "layouterEnable"
#define XtNversion           "version"
#define XtNlinkColor         "linkColor"
#define XtNaLinkColor        "aLinkColor"
#define XtNvLinkColor        "vLinkColor"
#define XtNlinkCursor        "linkCursor"
#define XtNuseHTMLStyles     "useHTMLStyles"

/* resource classes */

#define XtRFontFamily       "FontFamily"

/* global types */

typedef struct _KinoClassRec *KinoWidgetClass;
typedef struct _KinoRec *KinoWidget;

/* global variables */

extern WidgetClass kinoWidgetClass;

/* global functions */

void KinoRefresh (Widget);
void KinoRelayout (Widget);

#endif
