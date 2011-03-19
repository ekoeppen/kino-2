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
 * File..........: Style.c
 * Description...: Simple CSS implementation
 *
 * $Id: Style.c,v 1.6 1999/09/29 14:35:21 koeppen Exp $
 *-----------------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include "Kino.h"
#include "KinoP.h"

/* local definitions */

#define DPI 100

/* local types */

typedef struct
{
  char *name;
  int red;
  int green;
  int blue;
  int alpha;
} NamedColor;

/* local functions */

static PropertySetHandler setFontFamily;
static PropertySetHandler setFontVariant;
static PropertySetHandler setFontWeight;
static PropertySetHandler setFontStyle;
static PropertySetHandler setFontSize;
static PropertySetHandler setColor;
static PropertySetHandler setBackground;
static PropertySetHandler setBackgroundColor;
static PropertySetHandler setBackgroundImage;
static PropertySetHandler setBackgroundRepeat;
static PropertySetHandler setBackgroundAttachment;
static PropertySetHandler setBackgroundPosition;
static PropertySetHandler setWordspacing;
static PropertySetHandler setLetterspacing;
static PropertySetHandler setVerticalAlign;
static PropertySetHandler setTextTransform;
static PropertySetHandler setTextDecoration;
static PropertySetHandler setTextAlign;
static PropertySetHandler setTextIndent;
static PropertySetHandler setLineHeight;
static PropertySetHandler setMargin;
static PropertySetHandler setMarginTop;
static PropertySetHandler setMarginRight;
static PropertySetHandler setMarginBottom;
static PropertySetHandler setMarginLeft;
static PropertySetHandler setPadding;
static PropertySetHandler setPaddingTop;
static PropertySetHandler setPaddingRight;
static PropertySetHandler setPaddingBottom;
static PropertySetHandler setPaddingLeft;
static PropertySetHandler setBorder;
static PropertySetHandler setBorderWidth;
static PropertySetHandler setBorderTopWidth;
static PropertySetHandler setBorderRightWidth;
static PropertySetHandler setBorderBottomWidth;
static PropertySetHandler setBorderLeftWidth;
static PropertySetHandler setBorderColor;
static PropertySetHandler setBorderTopColor;
static PropertySetHandler setBorderRightColor;
static PropertySetHandler setBorderBottomColor;
static PropertySetHandler setBorderLeftColor;
static PropertySetHandler setBorderStyle;
static PropertySetHandler setBorderTopStyle;
static PropertySetHandler setBorderRightStyle;
static PropertySetHandler setBorderLeftStyle;
static PropertySetHandler setBorderBottomStyle;
static PropertySetHandler setFloating;
static PropertySetHandler setClear;
static PropertySetHandler setDisplay;
static PropertySetHandler setWhitespace;
static PropertySetHandler setListStyleType;
static PropertySetHandler setListStyleImage;
static PropertySetHandler setListStylePosition;

static PropertyGetHandler getFontFamily;
static PropertyGetHandler getFontVariant;
static PropertyGetHandler getFontWeight;
static PropertyGetHandler getFontStyle;
static PropertyGetHandler getFontSize;
static PropertyGetHandler getColor;
static PropertyGetHandler getBackground;
static PropertyGetHandler getBackgroundColor;
static PropertyGetHandler getBackgroundImage;
static PropertyGetHandler getBackgroundRepeat;
static PropertyGetHandler getBackgroundAttachment;
static PropertyGetHandler getBackgroundPosition;
static PropertyGetHandler getWordspacing;
static PropertyGetHandler getLetterspacing;
static PropertyGetHandler getVerticalAlign;
static PropertyGetHandler getTextTransform;
static PropertyGetHandler getTextDecoration;
static PropertyGetHandler getTextAlign;
static PropertyGetHandler getTextIndent;
static PropertyGetHandler getLineHeight;
static PropertyGetHandler getMargin;
static PropertyGetHandler getMarginTop;
static PropertyGetHandler getMarginRight;
static PropertyGetHandler getMarginBottom;
static PropertyGetHandler getMarginLeft;
static PropertyGetHandler getPadding;
static PropertyGetHandler getPaddingTop;
static PropertyGetHandler getPaddingRight;
static PropertyGetHandler getPaddingBottom;
static PropertyGetHandler getPaddingLeft;
static PropertyGetHandler getBorder;
static PropertyGetHandler getBorderWidth;
static PropertyGetHandler getBorderTopWidth;
static PropertyGetHandler getBorderRightWidth;
static PropertyGetHandler getBorderBottomWidth;
static PropertyGetHandler getBorderLeftWidth;
static PropertyGetHandler getBorderColor;
static PropertyGetHandler getBorderTopColor;
static PropertyGetHandler getBorderRightColor;
static PropertyGetHandler getBorderBottomColor;
static PropertyGetHandler getBorderLeftColor;
static PropertyGetHandler getBorderStyle;
static PropertyGetHandler getBorderTopStyle;
static PropertyGetHandler getBorderRightStyle;
static PropertyGetHandler getBorderLeftStyle;
static PropertyGetHandler getBorderBottomStyle;
static PropertyGetHandler getFloating;
static PropertyGetHandler getClear;
static PropertyGetHandler getDisplay;
static PropertyGetHandler getWhitespace;
static PropertyGetHandler getListStyleType;
static PropertyGetHandler getListStyleImage;
static PropertyGetHandler getListStylePosition;

#ifdef WAFE_AUS_EK
int wafeCloseColor(Display *, Colormap, XColor *, XColor **, Pixel *);
#endif

extern PredefinedRule predefinedRules[];

static NamedColor namedColors[] =
{
  {"black",    0x00, 0x00, 0x00, 0x00},
  {"silver",   0xC0, 0xC0, 0xC0, 0x00},
  {"gray",     0x80, 0x80, 0x80, 0x00},
  {"white",    0xFF, 0xFF, 0xFF, 0x00},
  {"maroon",   0x80, 0x00, 0x00, 0x00},
  {"red",      0xFF, 0x00, 0x00, 0x00},
  {"purple",   0x80, 0x00, 0x80, 0x00},
  {"fuchsia",  0xFF, 0x00, 0xFF, 0x00},
  {"green",    0x00, 0x80, 0x00, 0x00},
  {"lime",     0x00, 0xFF, 0x00, 0x00},
  {"olive",    0x80, 0x80, 0x00, 0x00},
  {"yellow",   0xFF, 0xFF, 0x00, 0x00},
  {"navy",     0x00, 0x00, 0x80, 0x00},
  {"blue",     0x00, 0x00, 0xFF, 0x00},
  {"teal",     0x00, 0x80, 0x80, 0x00},
  {"aqua",     0x00, 0xFF, 0xFF, 0x00}
};

static Property fontHandler[] =
{
  {"font-family", setFontFamily, getFontFamily},
  {"font-variant", setFontVariant, getFontVariant},
  {"font-weight", setFontWeight, getFontWeight},
  {"font-style", setFontStyle, getFontStyle},
  {"font-size", setFontSize, getFontSize}
};

/* C code produced by gperf version 2.7 */
/* Command-line: gperf -E -H propertyHandlerHash -N getPropertyHandler -t -S 1 -a -k 1,2,8,15 -7 -C css  */
/* maximum key range = 200, duplicates = 0 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
propertyHandlerHash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205,  10, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
      205, 205, 205, 205, 205, 205, 205,  20,   0,   0,
       45,   0,  15,  40,  10,  55, 205, 205,  35,  40,
        0,   0,   0, 205,   5,  45,  16,  15,  25,  50,
      205,  40, 205, 205, 205, 205, 205, 205
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 15:
        hval += asso_values[str[14]];
      case 14:
      case 13:
      case 12:
      case 11:
      case 10:
      case 9:
      case 8:
        hval += asso_values[str[7]];
      case 7:
      case 6:
      case 5:
      case 4:
      case 3:
      case 2:
        hval += asso_values[str[1]];
      case 1:
        hval += asso_values[str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const Property *
getPropertyHandler (str, len)
     register const char *str;
     register unsigned int len;
{
  enum
    {
      TOTAL_KEYWORDS = 48,
      MIN_WORD_LENGTH = 5,
      MAX_WORD_LENGTH = 21,
      MIN_HASH_VALUE = 5,
      MAX_HASH_VALUE = 204
    };

  static const Property wordlist[] =
    {
      {"color", setColor, getColor},
      {"border", setBorder, getBorder},
      {"border-color", setBorderColor, getBorderColor},
      {"border-bottom-color", setBorderBottomColor, getBorderBottomColor},
      {"border-right-color", setBorderRightColor, getBorderRightColor},
      {"padding", setPadding, getPadding},
      {"text-decoration", setTextDecoration, getTextDecoration},
      {"border-top-color", setBorderTopColor, getBorderTopColor},
      {"border-right-style", setBorderRightStyle, getBorderRightStyle},
      {"clear", setClear, getClear},
      {"padding-top", setPaddingTop, getPaddingTop},
      {"padding-left", setPaddingLeft, getPaddingLeft},
      {"padding-right", setPaddingRight, getPaddingRight},
      {"padding-bottom", setPaddingBottom, getPaddingBottom},
      {"background", setBackground, getBackground},
      {"border-top-width", setBorderTopWidth, getBorderTopWidth},
      {"text-transform", setTextTransform, getTextTransform},
      {"background-color", setBackgroundColor, getBackgroundColor},
      {"background-repeat", setBackgroundRepeat, getBackgroundRepeat},
      {"float", setFloating, getFloating},
      {"border-style", setBorderStyle, getBorderStyle},
      {"border-width", setBorderWidth, getBorderWidth},
      {"border-bottom-style", setBorderBottomStyle, getBorderBottomStyle},
      {"margin", setMargin, getMargin},
      {"border-top-style", setBorderTopStyle, getBorderTopStyle},
      {"border-bottom-width", setBorderBottomWidth, getBorderBottomWidth},
      {"white-space", setWhitespace, getWhitespace},
      {"text-indent", setTextIndent, getTextIndent},
      {"margin-bottom", setMarginBottom, getMarginBottom},
      {"vertical-align", setVerticalAlign, getVerticalAlign},
      {"background-attachment", setBackgroundAttachment, getBackgroundAttachment},
      {"margin-right", setMarginRight, getMarginRight},
      {"border-right-width", setBorderRightWidth, getBorderRightWidth},
      {"text-align", setTextAlign, getTextAlign},
      {"word-spacing", setWordspacing, getWordspacing},
      {"margin-top", setMarginTop, getMarginTop},
      {"border-left-color", setBorderLeftColor, getBorderLeftColor},
      {"background-image", setBackgroundImage, getBackgroundImage},
      {"border-left-style", setBorderLeftStyle, getBorderLeftStyle},
      {"letter-spacing", setLetterspacing, getLetterspacing},
      {"border-left-width", setBorderLeftWidth, getBorderLeftWidth},
      {"margin-left", setMarginLeft, getMarginLeft},
      {"display", setDisplay, getDisplay},
      {"background-position", setBackgroundPosition, getBackgroundPosition},
      {"list-style-type", setListStyleType, getListStyleType},
      {"line-height", setLineHeight, getLineHeight},
      {"list-style-image", setListStyleImage, getListStyleImage},
      {"list-style-position", setListStylePosition, getListStylePosition}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = propertyHandlerHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const Property *resword;

          switch (key - 5)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 1:
                resword = &wordlist[1];
                goto compare;
              case 7:
                resword = &wordlist[2];
                goto compare;
              case 14:
                resword = &wordlist[3];
                goto compare;
              case 18:
                resword = &wordlist[4];
                goto compare;
              case 22:
                resword = &wordlist[5];
                goto compare;
              case 26:
                resword = &wordlist[6];
                goto compare;
              case 27:
                resword = &wordlist[7];
                goto compare;
              case 34:
                resword = &wordlist[8];
                goto compare;
              case 35:
                resword = &wordlist[9];
                goto compare;
              case 36:
                resword = &wordlist[10];
                goto compare;
              case 37:
                resword = &wordlist[11];
                goto compare;
              case 38:
                resword = &wordlist[12];
                goto compare;
              case 39:
                resword = &wordlist[13];
                goto compare;
              case 40:
                resword = &wordlist[14];
                goto compare;
              case 43:
                resword = &wordlist[15];
                goto compare;
              case 45:
                resword = &wordlist[16];
                goto compare;
              case 46:
                resword = &wordlist[17];
                goto compare;
              case 47:
                resword = &wordlist[18];
                goto compare;
              case 50:
                resword = &wordlist[19];
                goto compare;
              case 52:
                resword = &wordlist[20];
                goto compare;
              case 57:
                resword = &wordlist[21];
                goto compare;
              case 59:
                resword = &wordlist[22];
                goto compare;
              case 61:
                resword = &wordlist[23];
                goto compare;
              case 62:
                resword = &wordlist[24];
                goto compare;
              case 64:
                resword = &wordlist[25];
                goto compare;
              case 66:
                resword = &wordlist[26];
                goto compare;
              case 67:
                resword = &wordlist[27];
                goto compare;
              case 68:
                resword = &wordlist[28];
                goto compare;
              case 69:
                resword = &wordlist[29];
                goto compare;
              case 71:
                resword = &wordlist[30];
                goto compare;
              case 72:
                resword = &wordlist[31];
                goto compare;
              case 73:
                resword = &wordlist[32];
                goto compare;
              case 76:
                resword = &wordlist[33];
                goto compare;
              case 77:
                resword = &wordlist[34];
                goto compare;
              case 81:
                resword = &wordlist[35];
                goto compare;
              case 82:
                resword = &wordlist[36];
                goto compare;
              case 86:
                resword = &wordlist[37];
                goto compare;
              case 87:
                resword = &wordlist[38];
                goto compare;
              case 89:
                resword = &wordlist[39];
                goto compare;
              case 92:
                resword = &wordlist[40];
                goto compare;
              case 101:
                resword = &wordlist[41];
                goto compare;
              case 102:
                resword = &wordlist[42];
                goto compare;
              case 104:
                resword = &wordlist[43];
                goto compare;
              case 140:
                resword = &wordlist[44];
                goto compare;
              case 151:
                resword = &wordlist[45];
                goto compare;
              case 181:
                resword = &wordlist[46];
                goto compare;
              case 199:
                resword = &wordlist[47];
                goto compare;
            }
          return 0;
        compare:
          {
            register const char *s = resword->name;

            if (*str == *s && !strcmp (str + 1, s + 1))
              return resword;
          }
        }
    }
  return 0;
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetCSSProperties (Element *box)
{
  char style[MAX_ATTR_LENGTH + 1], *s;
  char name[MAX_NAME_LENGTH + 1], value[MAX_ATTR_LENGTH + 1];
  Property *p;
  Element *next;

  if (box->parent != NULL)
  {
    box->color = box->parent->color;
    box->font_size = box->parent->font_size;
    box->font_family = box->parent->font_family;
    box->font_weight = box->parent->font_weight;
    box->font_slant = box->parent->font_slant;
    box->text_align = box->parent->text_align;
    box->underline = box->parent->underline;
  }
  else
  {
#ifdef FIXME
    box->color = box->document->kino->kino.foreground;
    box->font_size.effective = box->document->kino->kino.defaultFontSize;
#endif
  }

  StyleBaseGetBoxStyles (box, style);
  s = style;
  while (StyleBaseParsePropertyName (&s, name) &&
    StyleBaseParsePropertyValue (&s, value))
  {
    p = fontHandler;
    while (p - fontHandler < XkNumber (fontHandler) &&
      strcmp (name, p->name))
    {
      p++;
    }
    if (p - fontHandler < XkNumber (fontHandler))
    {
      (p->setHandler)(box, value);
    }
  }

  /* ATTENTION: font info has to be set first, since some length
     values may depend on the chosen font! */

  ElementSetCSSFont (box);
  box->line_height.unit = box->font_size.unit;
  box->line_height.effective = box->font_size.effective;
  box->line_height.value = box->font_size.value;
  
  s = style;
  while (StyleBaseParsePropertyName (&s, name) &&
    StyleBaseParsePropertyValue (&s, value))
  {
    ElementSetCSSProperty (box, name, value);
  }

  next = NodeNextBlock ((Node *) box);
  if (next != NULL)
  {
    box->margin_bottom.effective = max (box->margin_bottom.effective,
      next->margin_top.effective);
    next->margin_top.effective = 0;
    next->collapsed = 1;
  }

  box->minWidth = ElementTotalLeft (box) + ElementTotalRight (box);
  box->maxWidth = ElementTotalLeft (box) + ElementTotalRight (box);
  box->height = ElementTotalTop (box) + ElementTotalBottom (box);
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetCSSProperty
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetCSSProperty (Element *box, char *name, char *value)
{
  const Property *h;
  int fontChanged;

  h = getPropertyHandler (name, strlen (name));
  if (h != NULL) h->setHandler (box, value);

  fontChanged = 0;
  h = fontHandler;
  while (h - fontHandler < XkNumber (fontHandler) &&
    strcmp (name, h->name))
  {
    h++;
  }
  if (h - fontHandler < XkNumber (fontHandler))
  {
    (h->setHandler)(box, value);
    fontChanged = 1;
  }
  if (fontChanged)
  {
    ElementSetCSSFont (box);
  }
}

/*-------------------------------------------------------------------------
 * Function......: ElementGetCSSProperty
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *ElementGetCSSProperty (Element *box, char *name)
{
  const Property *h;
  char *r;

  r = NULL;
  h = getPropertyHandler (name, strlen (name));
  if (h != NULL) r = h->getHandler (box);

  h = fontHandler;
  while (h - fontHandler < XkNumber (fontHandler) &&
    strcmp (name, h->name))
  {
    h++;
  }
  if (h - fontHandler < XkNumber (fontHandler))
  {
    r = (h->getHandler)(box);
  }

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: ElementSetCSSDisplay
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetCSSDisplay (Element *box)
{
  char style[MAX_ATTR_LENGTH + 1], *s;
  char name[MAX_NAME_LENGTH + 1], value[MAX_ATTR_LENGTH + 1];

  StyleBaseGetBoxStyles (box, style);
  s = style;
  while (StyleBaseParsePropertyName (&s, name) &&
    StyleBaseParsePropertyValue (&s, value))
  {
    if (!strcmp (name, "display") || !strcmp (name, "float"))
    {
      ElementSetCSSProperty (box, name, value);
    }
  }
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void ElementSetCSSFont (Element *box)
{
  box->font = VisualLoadFont (box->font_family,
    (int) box->font_size.effective, box->font_weight, box->font_slant,  0);
#ifdef FIXME
    box->document->kino->kino.fontScaling);
#endif
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

void setFontFamily (Element *box, char *value)
{
  if (!strcmp (value, "helvetica") ||
    !strcmp (value, "arial") ||
    !strcmp (value, "sans-serif"))
  {
    box->font_family = FONT_FAMILY_HELVETICA;
  }
  else if (!strcmp (value, "courier") ||
    !strcmp (value, "monospace"))
  {
    box->font_family = FONT_FAMILY_COURIER;
  }
  else if (!strcmp (value, "symbol"))
  {
    box->font_family = FONT_FAMILY_SYMBOL;
  }
  else
  {
    box->font_family = FONT_FAMILY_TIMES;
  }
}

void setFontVariant (Element *box, char *value)
{
}

void setFontWeight (Element *box, char *value)
{
  if (!strcmp (value, "normal") ||
    !strcmp (value, "medium"))
  {
    box->font_weight = FONT_WEIGHT_MEDIUM;
  }
  else
  {
    box->font_weight = FONT_WEIGHT_BOLD;
  }
}

void setFontStyle (Element *box, char *value)
{
  if (!strcmp (value, "italic") ||
    !strcmp (value, "oblique"))
  {
    box->font_slant = FONT_SLANT_ITALIC;
  }
  else if (!strcmp (value, "normal"))
  {
    box->font_slant = FONT_SLANT_ROMAN;
  }
}

void setFontSize (Element *box, char *value)
{
  KinoFont *font;

  if (box->parent != NULL) font = box->parent->font; else font = NULL;
  if (StyleConvertLength (font, &box->font_size, value))
  {
    switch (box->font_size.unit)
    {
      case UNIT_EM:
	if (box->parent != NULL)
	{
	  box->font_size.effective = 
	    box->font_size.value *
	    box->parent->font_size.effective;
	}
	break;
      case UNIT_PERCENT:
	if (box->parent != NULL)
	{
	  box->font_size.effective = 
	    box->font_size.value *
	    box->parent->font_size.effective / 100;
	}
	break;
      case UNIT_PIXEL:
      case UNIT_POINT:
      case UNIT_MM:
      case UNIT_EX:
	box->font_size.effective = box->font_size.effective * 72 / 100;
	break;
      case UNIT_ENUM:
      case UNIT_NONE:
	break;
    }
  }
  else
  {
  }
}

void setColor (Element *box, char *value)
{
  StyleConvertColor (&box->color, value);
}

void setBackground (Element *box, char *value)
{
  setBackgroundColor (box, value);
}

void setBackgroundColor (Element *box, char *value)
{
  char token[MAX_ATTR_LENGTH + 1], *t;

  strcpy (token, value); t = strtok (token, " \r\t\n");
  if (strcmp (box->tag, "BODY"))
  {
    StyleConvertColor (&box->background_color, value);
  }
#ifdef FIXME
  else
  {
    if (strcmp (t, "transparent"))
    {
      StyleConvertColor (&p, value);
      XtSetArg (arg[0], XtNbackground, p);
      XtSetValues ((Widget) box->document->kino, arg, 1);
    }
  }
#endif
}

void setBackgroundImage (Element *box, char *value)
{
}

void setBackgroundRepeat (Element *box, char *value)
{
}

void setBackgroundAttachment (Element *box, char *value)
{
}

void setBackgroundPosition (Element *box, char *value)
{
}

void setWordspacing (Element *box, char *value)
{
}

void setLetterspacing (Element *box, char *value)
{
}

void setVerticalAlign (Element *box, char *value)
{
  StyleConvertVAlign (box->font, &box->vertical_align, value);
}

void setTextTransform (Element *box, char *value)
{
}

void setTextDecoration (Element *box, char *value)
{
  char token[MAX_ATTR_LENGTH + 1], *t;

  strcpy (token, value); t = strtok (token, " \r\t\n");
  if (!strcmp (value, "underline")) box->underline = 1;
  else if (!strcmp (value, "overline")) box->overline = 1;
  else if (!strcmp (value, "line-through")) box->line_through = 1;
}

void setTextAlign (Element *box, char *value)
{
  char token[MAX_ATTR_LENGTH + 1], *t;

  strcpy (token, value); t = strtok (token, " \r\t\n");
  if (!strcmp (t, "center")) box->text_align = ALIGN_CENTER;
  else if (!strcmp (t, "left")) box->text_align = ALIGN_LEFT;
  else if (!strcmp (t, "right")) box->text_align = ALIGN_RIGHT;
  else if (!strcmp (t, "justify")) box->text_align = ALIGN_JUSTIFY;
}

void setTextIndent (Element *box, char *value)
{
}

void setLineHeight (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->line_height, value);
}

void setMargin (Element *box, char *value)
{
  char v[MAX_ATTR_LENGTH + 1];
  char t[4][MAX_ATTR_LENGTH + 1];
  char *token;
  int i;

  strcpy (v, value);
  i = 0;
  token = strtok (v, " \r\n\t;");
  while (token != NULL && i < 4)
  {
    strcpy (t[i], token);
    token = strtok (NULL, " \r\n\t;");
    i++;
  }
  switch (i)
  {
    case 1:
      StyleConvertLength (box->font, &box->margin_bottom, t[0]);
      StyleConvertLength (box->font, &box->margin_left, t[0]);
      StyleConvertLength (box->font, &box->margin_right, t[0]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->margin_top, t[0]);
      break;
    case 2:
      StyleConvertLength (box->font, &box->margin_bottom, t[0]);
      StyleConvertLength (box->font, &box->margin_left, t[1]);
      StyleConvertLength (box->font, &box->margin_right, t[1]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->margin_top, t[0]);
      break;
    case 3:
      StyleConvertLength (box->font, &box->margin_right, t[1]);
      StyleConvertLength (box->font, &box->margin_bottom, t[2]);
      StyleConvertLength (box->font, &box->margin_left, t[2]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->margin_top, t[0]);
      break;
    case 4:
      StyleConvertLength (box->font, &box->margin_bottom, t[1]);
      StyleConvertLength (box->font, &box->margin_left, t[2]);
      StyleConvertLength (box->font, &box->margin_right, t[3]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->margin_top, t[0]);
      break;
  }
}

void setMarginTop (Element *box, char *value)
{
  if (!box->collapsed)
    StyleConvertLength (box->font, &box->margin_top, value);
}

void setMarginRight (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->margin_right, value);
}

void setMarginBottom (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->margin_bottom, value);
}

void setMarginLeft (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->margin_left, value);
}

void setPadding (Element *box, char *value)
{
  char v[MAX_ATTR_LENGTH + 1];
  char t[4][MAX_ATTR_LENGTH + 1];
  char *token;
  int i;

  strcpy (v, value);
  i = 0;
  token = strtok (v, " \r\n\t;");
  while (token != NULL && i < 4)
  {
    strcpy (t[i], token);
    token = strtok (NULL, " \r\n\t;");
    i++;
  }
  switch (i)
  {
    case 1:
      StyleConvertLength (box->font, &box->padding_bottom, t[0]);
      StyleConvertLength (box->font, &box->padding_left, t[0]);
      StyleConvertLength (box->font, &box->padding_right, t[0]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->padding_top, t[0]);
      break;
    case 2:
      StyleConvertLength (box->font, &box->padding_bottom, t[0]);
      StyleConvertLength (box->font, &box->padding_left, t[1]);
      StyleConvertLength (box->font, &box->padding_right, t[1]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->padding_top, t[0]);
      break;
    case 3:
      StyleConvertLength (box->font, &box->padding_right, t[1]);
      StyleConvertLength (box->font, &box->padding_bottom, t[2]);
      StyleConvertLength (box->font, &box->padding_left, t[2]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->padding_top, t[0]);
      break;
    case 4:
      StyleConvertLength (box->font, &box->padding_bottom, t[1]);
      StyleConvertLength (box->font, &box->padding_left, t[2]);
      StyleConvertLength (box->font, &box->padding_right, t[3]);
      if (!box->collapsed)
	StyleConvertLength (box->font, &box->padding_top, t[0]);
      break;
  }
}

void setPaddingTop (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->padding_top, value);
}

void setPaddingRight (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->padding_right, value);
}

void setPaddingBottom (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->padding_bottom, value);
}

void setPaddingLeft (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->padding_left, value);
}

void setBorder (Element *box, char *value)
{
  char v[MAX_ATTR_LENGTH + 1];
  char *token;
  int w;

  strcpy (v, value);
  token = strtok (v, " ,\r\n\t");
  w = 0;
  while (token != NULL)
  {
    if (!strcmp (token, "thin"))
    {
      w = 1;
    }
    else if (!strcmp (token, "medium"))
    {
      w = 2;
    }
    else if (!strcmp (token, "thick"))
    {
      w = 3;
    }
    token = strtok (NULL, " ,\r\n\t");
  }
  box->border_top_width.unit = UNIT_POINT;
  box->border_left_width.unit = UNIT_POINT;
  box->border_right_width.unit = UNIT_POINT;
  box->border_bottom_width.unit = UNIT_POINT;
  box->border_top_width.effective = w * DPI / 72;
  box->border_left_width.effective = w * DPI / 72;
  box->border_right_width.effective = w * DPI / 72;
  box->border_bottom_width.effective = w * DPI / 72;
}

void setBorderWidth (Element *box, char *value)
{
  char v[MAX_ATTR_LENGTH + 1];
  char t[4][MAX_ATTR_LENGTH + 1];
  char *token;
  int i;

  strcpy (v, value);
  i = 0;
  token = strtok (v, " \r\n\t;");
  while (token != NULL && i < 4)
  {
    strcpy (t[i], token);
    token = strtok (NULL, " \r\n\t;");
    i++;
  }
  switch (i)
  {
    case 1:
      StyleConvertLength (box->font, &box->border_bottom_width, t[0]);
      StyleConvertLength (box->font, &box->border_left_width, t[0]);
      StyleConvertLength (box->font, &box->border_right_width, t[0]);
      StyleConvertLength (box->font, &box->border_top_width, t[0]);
      break;
    case 2:
      StyleConvertLength (box->font, &box->border_bottom_width, t[0]);
      StyleConvertLength (box->font, &box->border_left_width, t[1]);
      StyleConvertLength (box->font, &box->border_right_width, t[1]);
      StyleConvertLength (box->font, &box->border_top_width, t[0]);
      break;
    case 3:
      StyleConvertLength (box->font, &box->border_right_width, t[1]);
      StyleConvertLength (box->font, &box->border_bottom_width, t[2]);
      StyleConvertLength (box->font, &box->border_left_width, t[2]);
      StyleConvertLength (box->font, &box->border_top_width, t[0]);
      break;
    case 4:
      StyleConvertLength (box->font, &box->border_bottom_width, t[1]);
      StyleConvertLength (box->font, &box->border_left_width, t[2]);
      StyleConvertLength (box->font, &box->border_right_width, t[3]);
      StyleConvertLength (box->font, &box->border_top_width, t[0]);
      break;
  }
}

void setBorderTopWidth (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->border_top_width, value);
}

void setBorderRightWidth (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->border_right_width, value);
}

void setBorderBottomWidth (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->border_bottom_width, value);
}

void setBorderLeftWidth (Element *box, char *value)
{
  StyleConvertLength (box->font, &box->border_left_width, value);
}

void setBorderColor (Element *box, char *value)
{
  char v[MAX_ATTR_LENGTH + 1];
  char t[4][MAX_ATTR_LENGTH + 1];
  char *token;
  int i;

  strcpy (v, value);
  i = 0;
  token = strtok (v, " \r\n\t;");
  while (token != NULL && i < 4)
  {
    strcpy (t[i], token);
    token = strtok (NULL, " \r\n\t;");
    i++;
  }
  switch (i)
  {
    case 1:
      StyleConvertColor (&box->border_bottom_color, t[0]);
      StyleConvertColor (&box->border_left_color, t[0]);
      StyleConvertColor (&box->border_right_color, t[0]);
      StyleConvertColor (&box->border_top_color, t[0]);
      break;
    case 2:
      StyleConvertColor (&box->border_bottom_color, t[0]);
      StyleConvertColor (&box->border_left_color, t[1]);
      StyleConvertColor (&box->border_right_color, t[1]);
      StyleConvertColor (&box->border_top_color, t[0]);
      break;
    case 3:
      StyleConvertColor (&box->border_right_color, t[1]);
      StyleConvertColor (&box->border_bottom_color, t[2]);
      StyleConvertColor (&box->border_left_color, t[2]);
      StyleConvertColor (&box->border_top_color, t[0]);
      break;
    case 4:
      StyleConvertColor (&box->border_bottom_color, t[1]);
      StyleConvertColor (&box->border_left_color, t[2]);
      StyleConvertColor (&box->border_right_color, t[3]);
      StyleConvertColor (&box->border_top_color, t[0]);
      break;
  }
}

void setBorderTopColor (Element *box, char *value)
{
  StyleConvertColor (&box->border_top_color, value);
}

void setBorderRightColor (Element *box, char *value)
{
  StyleConvertColor (&box->border_right_color, value);
}

void setBorderBottomColor (Element *box, char *value)
{
  StyleConvertColor (&box->border_bottom_color, value);
}

void setBorderLeftColor (Element *box, char *value)
{
  StyleConvertColor (&box->border_left_color, value);
}

void setBorderStyle (Element *box, char *value)
{
}

void setBorderTopStyle (Element *box, char *value)
{
}

void setBorderRightStyle (Element *box, char *value)
{
}

void setBorderLeftStyle (Element *box, char *value)
{
}

void setBorderBottomStyle (Element *box, char *value)
{
}

void setFloating (Element *box, char *value)
{
  StyleConvertFloating (&box->floating, value);
}

void setClear (Element *box, char *value)
{
}

void setDisplay (Element *box, char *value)
{
  StyleConvertDisplay (&box->display, value);
  VisualChangeManagedInsets (box);
}

void setWhitespace (Element *box, char *value)
{
}

void setListStyleType (Element *box, char *value)
{
}

void setListStyleImage (Element *box, char *value)
{
}

void setListStylePosition (Element *box, char *value)
{
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

char *getFontFamily (Element *box)
{
  static char r[256];

  return r;
}

char *getFontVariant (Element *box)
{
  static char r[256];

  return r;
}

char *getFontWeight (Element *box)
{
  static char r[256];

  return r;
}

char *getFontStyle (Element *box)
{
  static char r[256];

  return r;
}

char *getFontSize (Element *box)
{
  static char r[256];

  return r;
}

char *getColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBackground (Element *box)
{
  static char r[256];

  return r;
}

char *getBackgroundColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBackgroundImage (Element *box)
{
  static char r[256];

  return r;
}

char *getBackgroundRepeat (Element *box)
{
  static char r[256];

  return r;
}

char *getBackgroundAttachment (Element *box)
{
  static char r[256];

  return r;
}

char *getBackgroundPosition (Element *box)
{
  static char r[256];

  return r;
}

char *getWordspacing (Element *box)
{
  static char r[256];

  return r;
}

char *getLetterspacing (Element *box)
{
  static char r[256];

  return r;
}

char *getVerticalAlign (Element *box)
{
  static char r[256];

  return r;
}

char *getTextTransform (Element *box)
{
  static char r[256];

  return r;
}

char *getTextDecoration (Element *box)
{
  static char r[256];

  return r;
}

char *getTextAlign (Element *box)
{
  static char r[256];

  return r;
}

char *getTextIndent (Element *box)
{
  static char r[256];

  return r;
}

char *getLineHeight (Element *box)
{
  static char r[256];

  return r;
}

char *getMargin (Element *box)
{
  static char r[256];

  return r;
}

char *getMarginTop (Element *box)
{
  static char r[256];

  return r;
}

char *getMarginRight (Element *box)
{
  static char r[256];

  return r;
}

char *getMarginBottom (Element *box)
{
  static char r[256];

  return r;
}

char *getMarginLeft (Element *box)
{
  static char r[256];

  return r;
}

char *getPadding (Element *box)
{
  static char r[256];

  return r;
}

char *getPaddingTop (Element *box)
{
  static char r[256];

  return r;
}

char *getPaddingRight (Element *box)
{
  static char r[256];

  return r;
}

char *getPaddingBottom (Element *box)
{
  static char r[256];

  return r;
}

char *getPaddingLeft (Element *box)
{
  static char r[256];

  return r;
}

char *getBorder (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderWidth (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderTopWidth (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderRightWidth (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderBottomWidth (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderLeftWidth (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderTopColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderRightColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderBottomColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderLeftColor (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderTopStyle (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderStyle (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderRightStyle (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderLeftStyle (Element *box)
{
  static char r[256];

  return r;
}

char *getBorderBottomStyle (Element *box)
{
  static char r[256];

  return r;
}

char *getFloating (Element *box)
{
  char *r;

  switch (box->floating)
  {
    case FLOAT_NONE: r = "none"; break;
    case FLOAT_LEFT: r = "left"; break;
    case FLOAT_RIGHT: r = "right"; break;
    default: r = "invalid"; break;
  }
  return r;
}

char *getClear (Element *box)
{
  static char r[256];

  return r;
}

char *getDisplay (Element *box)
{
  char *r;

  switch (box->display)
  {
    case DISPLAY_NONE: r = "none"; break;
    case DISPLAY_BLOCK: r = "block"; break;
    case DISPLAY_LIST_ITEM: r = "list-item"; break;
    case DISPLAY_INLINE: r = "inline"; break;
    default: r = "invalid"; break;
  }
  return r;
}

char *getWhitespace (Element *box)
{
  static char r[256];

  return r;
}

char *getListStyleType (Element *box)
{
  static char r[256];

  return r;
}

char *getListStyleImage (Element *box)
{
  static char r[256];

  return r;
}

char *getListStylePosition (Element *box)
{
  static char r[256];

  return r;
}

/*-------------------------------------------------------------------------
 * Function......: 
 * Description...: 
 *
 * In............: 
 * Out...........: 
 * Precondition..: 
 * Postcondition.: 
 *-----------------------------------------------------------------------*/

int StyleConvertLength (KinoFont *font, Length *length, char *value)
{
  int r;
  char unit[128];
  char multiplier[128];
  float m;
  int dummy;
  int ascent, descent;

  r = 0;
  unit[0] = '\0';
  multiplier[0] = '\0';
  m = 1;
  if (value != NULL)
  {
    if (sscanf (value, "%127[-+0-9.] %127s", multiplier, unit) >= 1)
    {
      length->value = atof (multiplier);
      r = 1;
      if (!strcmp (unit, "px") || unit[0] == '\0')
      {
	length->unit = UNIT_PIXEL;
      }
      else if (!strcmp (unit, "%"))
      {
	length->unit = UNIT_PERCENT;
      }
      else if (!strcmp (unit, "em") && font != NULL)
      {
	length->unit = UNIT_EM;
	VisualTextExtents (font, "M", &dummy, &dummy, &dummy,
	  &ascent, &descent);
	m = ascent + descent;
      }
      else if (!strcmp (unit, "ex") && font != NULL)
      {
	length->unit = UNIT_EX;
	VisualTextExtents (font, "x", &dummy, &dummy, &dummy,
	  &ascent, &descent);
	m = ascent;
      }
      else if (!strcmp (unit, "pt"))
      {
	length->unit = UNIT_POINT;
	m = DPI / 72.0;
      }
      else if (!strcmp (unit, "pc"))
      {
	length->unit = UNIT_POINT;
	length->value *= 6.0;
	m = DPI / 72.0;
      }
      else if (!strcmp (unit, "in"))
      {
	length->unit = UNIT_MM;
	length->value *= 25.4;
	m = DPI / 25.4;
      }
      else if (!strcmp (unit, "mm"))
      {
	length->unit = UNIT_MM;
	m = DPI / 25.4;
      }
      else if (!strcmp (unit, "cm"))
      {
	length->unit = UNIT_MM;
	length->value *= 10.0;
	m = DPI / 2.54;
      }
      length->effective = length->value * m;
    }
  }
  return r;
}

void StyleConvertColor (KinoColor *color, char *value)
{
  int i, n;
  char token[MAX_ATTR_LENGTH + 1], *t;

  color->alpha = 0;
  if (value != NULL)
  {
    if (value[0] == '#') value++;
    n = sscanf (value, "%2x%2x%2x",
      (unsigned *) &color->red,
      (unsigned *) &color->green,
      (unsigned *) &color->blue);
    if (n < 3)
    {
      n = sscanf (value, "%2X%2X%2X",
	(unsigned *) &color->red,
	(unsigned *) &color->green,
	(unsigned *) &color->blue);
    }
    if (n == 3)
    {
      color->red *= 0x100;
      color->green *= 0x100;
      color->blue *= 0x100;
    }
    else
    {
      n = sscanf (value, "%1X%1X%1X",
	(unsigned *) &color->red,
	(unsigned *) &color->green,
	(unsigned *) &color->blue);
      if (n < 3)
      {
	n = sscanf (value, "%1x%1x%1x",
	  (unsigned *) &color->red,
	  (unsigned *) &color->green,
	  (unsigned *) &color->blue);
      }
      if (n == 3)
      {
	color->red = (color->red * 0x10 + 0x0f) * 0x100;
	color->green = (color->green * 0x10 + 0x0f) * 0x100;
	color->blue = (color->blue * 0x10 + 0x0f) * 0x100;
      }
      else
      {
	strcpy (token, value);
	t = strtok (token, " \t\r\n");
	for (i = 0; i < XkNumber (namedColors); i++)
	{
	  if (!strcmp (t, namedColors[i].name))
	  {
	    color->red = namedColors[i].red * 0x100;
	    color->green = namedColors[i].green * 0x100;
	    color->blue = namedColors[i].blue * 0x100;
	    color->alpha = namedColors[i].alpha * 0x100;
	  }
	}
      }
    }
  }
}

void StyleConvertFloating (Float *floating, char *value)
{
  char token[MAX_ATTR_LENGTH + 1], *t;

  strcpy (token, value); t = strtok (token, " \r\t\n");
  *floating = FLOAT_NONE;
  if (value != NULL)
  {
    if (!strcmp (t, "left"))
    {
      *floating = FLOAT_LEFT;
    }
    else if (!strcmp (t, "right"))
    {
      *floating = FLOAT_RIGHT;
    }
  }
}

void StyleConvertDisplay (DisplayStyle *display, char *value)
{
  *display = DISPLAY_BLOCK;
  if (!strcmp (value, "inline"))
  {
    *display = DISPLAY_INLINE;
  }
  else if (!strcmp (value, "none"))
  {
    *display = DISPLAY_NONE;
  }
  else if (!strcmp (value, "list-item"))
  {
    *display = DISPLAY_BLOCK;
    /*    *display = DISPLAY_LIST_ITEM; */
  }
}

int StyleConvertVAlign (KinoFont *font, Length *length,
  char *value)
{
  int r;
  char token[MAX_ATTR_LENGTH + 1], *t;

  r = 1;
  if (value != NULL)
  {
    if (!StyleConvertLength (font, length, value))
    {
      strcpy (token, value); t = strtok (token, " \r\t\n");
      length->unit = UNIT_ENUM;
      length->value = VALIGN_TOP;
      if (!strcmp (t, "baseline"))
      {
	length->value = VALIGN_BASELINE;
      }
      else if (!strcmp (t, "sub"))
      {
	length->value = VALIGN_SUB;
      }
      else if (!strcmp (t, "super"))
      {
	length->value = VALIGN_SUPER;
      }
      else if (!strcmp (t, "text-top"))
      {
	length->value = VALIGN_TEXT_TOP;
      }
      else if (!strcmp (t, "middle"))
      {
	length->value = VALIGN_MIDDLE;
      }
      else if (!strcmp (t, "bottom"))
      {
	length->value = VALIGN_BOTTOM;
      }
      else if (!strcmp (t, "text-bottom"))
      {
	length->value = VALIGN_TEXT_BOTTOM;
      }
      else
      {
	r = 0;
      }
    }
  }
  else
  {
    r = 0;
  }

  return r;
}

int StyleConvertTextAlign (Align *align, char *value)
{
  int r;
  char token[MAX_ATTR_LENGTH + 1], *t;

  strcpy (token, value); t = strtok (token, " \r\t\n");
  r = 1;
  if (value != NULL)
  {
    if (!strcmp (t, "right")) *align = ALIGN_RIGHT;
    else if (!strcmp (t, "center")) *align = ALIGN_CENTER;
    else if (!strcmp (t, "justify")) *align = ALIGN_JUSTIFY;
    else r = 0;
  }
  else
  {
    r = 0;
  }

  return r;
}

