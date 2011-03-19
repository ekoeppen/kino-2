/*-------------------------------------------------------------------------
 * kinoparser - an XML/CSS parser, part of the Kino XML/CSS processor
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
 * File..........: StyleBase.c
 * Description...: Implementation of the Style structure which holds
 *                 information about text styles (color, font, etc)
 *
 * $Id: StyleBase.c,v 1.6 1999/03/29 14:34:23 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include "KinoParser.h"

PredefinedRule StylePredefinedRules[] =
{
  {"A", "display: inline;
background: transparent;
width: auto;
height: auto;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"A:link", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: underline;
margin: 0;
padding: 0;
border: 0;
color: #0000ff;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"ACRONYM", "font-variant: small-caps;
letter-spacing: 0.1em; 
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"ADDRESS", "font-style: italic;
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"APPLET", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"AREA", ""},
  {"B", "font-weight: bolder;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"BASE", ""},
  {"BASEFONT", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"BDO", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"BIG", "font-size: 1.17em;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"BLOCKQUOTE", "margin-left: 40px;
margin-right: 40px;
margin: 1.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"BODY", "font-size: 1em;
line-height: 1.33em;
margin: 8px;
background-position: -8px -8px; 
word-spacing: normal;
letter-spacing: normal;
text-transform: none;
text-align: left;
text-indent: 0;
white-space: normal;
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"BR", ""},
/*   {"BUTTON", "display: inline; */
  {"BUTTON", "background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"CAPTION", "text-align: center;
display: inline;
vertical-align: baseline;
font-size: 1em;
line-height: 1.33em;
word-spacing: normal;
letter-spacing: normal;
text-transform: none;
text-align: left;
text-indent: 0;
white-space: normal;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"CENTER", "text-align: center;
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"CITE", "font-style: italic;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"CODE", "font-family: monospace;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"COL", "background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"COLGROUP", "background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"DD", "margin-left: 40px; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"DEL", "text-decoration: line-through;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"DFN", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"DIR", "margin-left: 40px; 
margin: 1.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"DIV", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"DL", "margin: 1.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"DT", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"EM", "font-style: italic;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"FIELDSET", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"FONT", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"FORM", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"FRAME", "display: block"},
  {"FRAMESET", "display: block"},
  {"H1", "font-weight: bolder;
font-size: 2em;
margin: .67em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"H2", "font-weight: bolder;
font-size: 1.5em;
margin: .83em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"H3", "font-weight: bolder;
font-size: 1.17em;
line-height: 1.17em;
margin: 1em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"H4", "font-weight: bolder;
margin: 1.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"H5", "font-weight: bolder;
font-size: .83em;
line-height: 1.17em;
margin: 1.67em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"H6", "font-weight: bolder;
font-size: .67em;
margin: 2.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"HEAD", "display: none"},
  {"HR", "border: 1px inset;
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"HTML", "display: block; float: none"},
  {"I", "font-style: italic;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"IFRAME", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"IMG", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
/*   {"INPUT", "display: inline"}, */
  {"INS", "text-decoration: underline;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"ISINDEX", "display: inline"},
  {"KBD", "font-family: monospace;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"LABEL", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"LEGEND", "display: inline;
vertical-align: baseline;
font-size: 1em;
line-height: 1.33em;
word-spacing: normal;
letter-spacing: normal;
text-transform: none;
text-align: left;
text-indent: 0;
white-space: normal;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"LI", "list-style-type: lower-roman
list-style-type: lower-alpha;
list-style-type: decimal;
display: list-item;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"KINO2:LIBULLET", "font-family: symbol; margin-right: 0.5ex"},
  {"LINK", "display: none"},
  {"MAP", "display: inline"},
  {"MENU", "margin-left: 40px; 
margin: 1.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"META", "display: none"},
  {"NOFRAMES", "display: block"},
  {"NOSCRIPT", "display: block"},
  {"OBJECT", "display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"OL", "list-style-type: decimal;
margin-left: 40px; 
margin-top: 1.33em; 
margin-bottom: 1.33em; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none"},
  {"OPTION", ""},
  {"P", "margin: 1.33em 0; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"PARAM", "display: none"},
  {"PRE", "white-space: pre;
font-family: monospace;
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 1.33em 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"LISTING", "white-space: pre;
font-family: monospace;
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 1.33em 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"Q", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"S", "text-decoration: line-through;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"SAMP", "font-family: monospace;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"SCRIPT", ""},
  {"SELECT", "display: inline"},
  {"SMALL", "font-size: .83em;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"SPAN", "display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"STRIKE", "text-decoration: line-through;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"STRONG", "font-weight: bolder;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"STYLE", "display: none"},
  {"SUB", "vertical-align: sub;
font-size: .83em;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"SUP", "vertical-align: super;
font-size: .83em;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"TABLE", "display: block;
background-position: top left;
width: auto;
height: auto;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"TBODY", "display: block;
background-position: top left;
width: auto;
height: auto;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"TD", "display: block;
vertical-align: baseline;
font-size: 1em;
word-spacing: normal;
letter-spacing: normal;
text-transform: none;
text-align: left;
text-indent: 0;
white-space: normal;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: left;
clear: none"},
/*   {"TEXTAREA", "display: inline"}, */
  {"TFOOT", "display: block;
background-position: top left;
width: auto;
height: auto;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"TH", "font-weight: bolder;
text-align: center;
display: inline;
vertical-align: baseline;
font-size: 1em;
line-height: 1.33em;
word-spacing: normal;
letter-spacing: normal;
text-transform: none;
text-align: left;
text-indent: 0;
white-space: normal;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"THEAD", "display: block;
background-position: top left;
width: auto;
height: auto;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"TITLE", "display: none"},
  {"TR", "display: block;
background-position: top left;
width: auto;
height: auto;
background: transparent;
text-decoration: none;
margin: 1px;
padding: 1px;
border: none;
float: none;
clear: none"},
  {"TT", "font-family: monospace;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"U", "text-decoration: underline;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"UL", "margin-left: 40px; 
margin-top: 1.33em; 
margin-bottom: 1.33em; 
display: block;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"},
  {"VAR", "font-style: italic;
display: inline;
background: transparent;
width: auto;
height: auto;
text-decoration: none;
margin: 0;
padding: 0;
border: 0;
float: none;
clear: none;
vertical-align: baseline;
list-style-image: none;
list-style-type: disc;
list-style-position: outside"}
};

/*-------------------------------------------------------------------------
 * Function......: RuleSetDelete
 * Description...: 
 *                 
 * In............: RuleSet *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void RuleSetDelete (RuleSet *r)
{
  RuleSet *current;

  while (r != NULL)
  {
    current = r;
    r = r->next;
    if (current->selector != NULL) XkFree (current->selector);
    if (current->declaration != NULL) XkFree (current->declaration);
    XkFree (current);
  }
}

/*-------------------------------------------------------------------------
 * Function......: StyleBaseSetBoxDisplay
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void StyleBaseSetBoxDisplay (Element *box)
{
  char style[MAX_ATTR_LENGTH + 1], *s;
  char name[MAX_NAME_LENGTH + 1], value[MAX_ATTR_LENGTH + 1];

  StyleBaseGetBoxStyles (box, style);
  s = style;
  while (StyleBaseParsePropertyName (&s, name) &&
    StyleBaseParsePropertyValue (&s, value))
  {
    if (XkStrEqual (name, "display"))
    {
      StyleBaseConvertDisplay (&box->display, value);
    }
    else if (XkStrEqual (name, "float"))
    {
      StyleBaseConvertFloating (&box->floating, value);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: StyleBaseParsePropertyName
 * Description...: 
 *                 
 * In............: char **:
 *                 char *:
 * Out...........: int :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int StyleBaseParsePropertyName (char **s, char *n)
{
  int r;

  r = 1;
  while (**s != '\0' && !isalpha (**s)) (*s)++;
  if (**s != '\0')
  {
    while (isalnum (**s) || **s == '-' || **s == '_')
    {
      *n = **s;
      n++;
      (*s)++;
    }
    *n = '\0';
  }
  else
  {
    r = 0;
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: StyleBaseParsePropertyValue
 * Description...: 
 *                 
 * In............: char **:
 *                 char *:
 * Out...........: int :
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int StyleBaseParsePropertyValue (char **s, char *v)
{
  int r;

  r = 1;
  while (**s != '\0' && **s != ':') (*s)++;
  if (**s == ':') (*s)++;
  while (**s != '\0' && isspace (**s)) (*s)++;
  if (**s != '\0')
  {
    while (**s != '\0' && **s != ';')
    {
      *v = **s;
      v++;
      (*s)++;
    }
    *v = '\0';
  }
  else
  {
    r = 0;
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: StyleBaseGetBoxStyles
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: char *:
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void StyleBaseGetBoxStyles (Element *box, char *style)
{
  RuleSet *r;
  PredefinedRule *p;
  char *s;
  char selector[MAX_NAME_LENGTH + 1];
  char *class;

  style[0] = '\0';
  selector[0] = '\0';
  class = AttributeGetValue (box->attributes, "CLASS");
  if (box->tag)
  {
    strcpy (selector, box->tag);
  }
  if (class)
  {
    strcat (selector, ".");
    strcat (selector, class);
  }

  if (box->document && box->document->useHTMLStyles)
  {
    for (p = StylePredefinedRules;
	 p - StylePredefinedRules < XkNumber (StylePredefinedRules);
	 p++)
    {
      if (((p->selector[0] == '.' || p->selector[0] == ':') &&
	XkStrEqual (class, p->selector + 1)) ||
	XkStrEqual (selector, p->selector) ||
	XkStrEqual (box->tag, p->selector))
      {
	if (strlen (style) + strlen (p->declaration) + 2 < MAX_ATTR_LENGTH)
	{
	  strcat (style, p->declaration);
	  strcat (style, "; ");
	}
      }
    }
  }
  if (box->document)
  {
    for (r = box->document->cssRules; r != NULL; r = r->next)
    {
      if (((r->selector[0] == '.' || r->selector[0] == ':') &&
	XkStrEqual (class, r->selector + 1)) ||
	XkStrEqual (selector, r->selector) ||
	XkStrEqual (box->tag, r->selector))
      {
	if (strlen (style) + strlen (r->declaration) + 2 < MAX_ATTR_LENGTH)
	{
	  strcat (style, r->declaration);
	  strcat (style, "; ");
	}
      }
    }
  }
  s = AttributeGetValue (box->attributes, "STYLE");
  if (s && strlen (style) + strlen (s) < MAX_ATTR_LENGTH) strcat (style, s);
}

/*-------------------------------------------------------------------------
 * Function......: StyleBaseConvertFloating
 * Description...: 
 *                 
 * In............: Float *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void StyleBaseConvertFloating (Float *floating, char *value)
{
  char token[MAX_ATTR_LENGTH + 1], *t;

  strcpy (token, value); t = strtok (token, " \r\t\n");
  *floating = FLOAT_NONE;
  if (value != NULL)
  {
    if (XkStrEqual (t, "LEFT"))
    {
      *floating = FLOAT_LEFT;
    }
    else if (XkStrEqual (t, "RIGHT"))
    {
      *floating = FLOAT_RIGHT;
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: StyleBaseConvertDisplay
 * Description...: 
 *                 
 * In............: DisplayStyle *:
 *                 char *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void StyleBaseConvertDisplay (DisplayStyle *display, char *value)
{
  *display = DISPLAY_BLOCK;
  if (XkStrEqual (value, "INLINE"))
  {
    *display = DISPLAY_INLINE;
  }
  else if (XkStrEqual (value, "NONE"))
  {
    *display = DISPLAY_NONE;
  }
  else if (XkStrEqual (value, "LIST-ITEM"))
  {
    *display = DISPLAY_BLOCK;
    /*    *display = DISPLAY_LIST_ITEM; */
  }
}

/*-------------------------------------------------------------------------
 * Function......: StyleBasePrintProperties
 * Description...: 
 *                 
 * In............: Element *:
 * Out...........: -
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void StyleBasePrintProperties (Element *b)
{
  PredefinedRule *p;
  char style[MAX_ATTR_LENGTH + 1];

  for (p = StylePredefinedRules;
       p - StylePredefinedRules < XkNumber (StylePredefinedRules);
       p++)
  {
    b->tag = p->selector;
    StyleBaseGetBoxStyles (b, style);
    printf ("{\"%s\", \"%s\"}\n", b->tag, style);
  }
}
