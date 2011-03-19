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
 * File..........: DOM.h
 * Description...: Declaration of the DOM structures which are the most
 *                 important data structures since they hold the parsed data
 *                 of the source text and are the basis for any further
 *                 processing
 *
 * $Id: DOM.h,v 1.7 1999/06/30 17:28:15 koeppen Exp $
 *-----------------------------------------------------------------------*/

#ifndef __DOM_H
#define __DOM_H

/* global definitions */

/* maximum identifier lengths */

#define MAX_NAME_LENGTH      128
#define MAX_ATTR_LENGTH     1024
#define MAX_LITERAL_LENGTH 65536

/* forward declarations */

struct _TagHandlerInfo;
struct _LinkHandlerInfo;
struct _EventHandlerInfo;
struct _ScriptHandlerInfo;

/* global types */

typedef void *InsetData;
typedef void *KinoFont;
typedef void *KinoPixmap;

typedef void (*InsetSetValueFunc)(InsetData *, char *);
typedef char *(*InsetGetValueFunc)(InsetData *);
typedef void (*InsetDeleteFunc)(InsetData *);

typedef struct _ElementDeclaration
{
  char *name;			/* name of the element */
  Attribute *attributes;	/* default attribute list */
  int empty;			/* empty element? */
  struct _ElementDeclaration *next;
} ElementDeclaration;

/* codes for the different Node elements */

typedef enum
{
  ELEMENT_NODE                = 1,
  ATTRIBUTE_NODE              = 2,
  TEXT_NODE                   = 3,
  CDATA_SECTION_NODE          = 4,
  ENTITY_REFERENCE_NODE       = 5,
  ENTITY_NODE                 = 6,
  PROCESSING_INSTRUCTION_NODE = 7,
  COMMENT_NODE                = 8,
  DOCUMENT_NODE               = 9,
  DOCUMENT_TYPE_NODE          = 10,
  DOCUMENT_FRAGMENT_NODE      = 11,
  NOTATION_NODE               = 12,
  INSET_NODE                  = 9000,
  LINEBREAK_NODE              = 9001
} NodeType;

/* codes for the different anonymous boxes */

typedef enum
{
  ANON_NORMAL,			/* not anonymous */
  ANON_CONT,			/* box and contents anonymous */
  ANON_BOX,			/* only box anonymous */
  ANON_DELIMITER		/* this element delimits an anonymous list */
} ElementState;

typedef struct _Node		/* "superclass" of all DOM elements */
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;
} Node;

typedef struct _VisibleNode	/* "superclass" of all visible elements, */
				/* not directly instantiated */
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  int x, y;			/* position */
  int width, height;		/* dimension */
} VisibleNode;

typedef struct _Text
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  int x, y;			/* position */
  int width, height;		/* dimension */

  char *text;			/* actual text */
  int wrap:1;			/* can this element be wrapped? */
  int whitespace:1;		/* does this element contain only WS? */
  int source;			/* offset into source text */
} Text;

typedef struct _CDATASection
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  int x, y;			/* position */
  int width, height;		/* dimension */

  char *text;			/* actual text */
  int source;			/* offset into source text */
} CDATASection;

typedef struct _Inset
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  int x, y;			/* position */
  int width, height;		/* dimension */

  InsetData *inset;		/* the inset */
  InsetSetValueFunc setValue;	/* function for setting the inset's value */
  InsetGetValueFunc getValue;	/* function for getting the inset's value */
  InsetDeleteFunc delete;	/* function for deleting the inset */
  int source;			/* offset into source text */
} Inset;

typedef struct _Element
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  int x, y;			/* position */
  int width, height;		/* dimension */

  ElementState anonymous;	/* was the box not in the original text? */
  int source;			/* offset into source text */

  struct _Document *document;	/* associated document */
  char *tag;			/* tag name */
  char *namespace;		/* namespace part */
  int empty;			/* was this an empty element? (.../>) */
  int minWidth, maxWidth;

  Attribute *attributes;

  Node *contents;
  int saveRawContents;
  char* rawContents;

  struct _Table *table;

  int positioned;
  struct _Line *line;

  /* CSS properties */

  int collapsed;		/* are the margins collapsed? */

  KinoFont *font;		/* font information */

  FontFamily font_family;
  FontWeight font_weight;
  FontSlant font_slant;
  Length font_size;  

  KinoColor color;
  KinoColor background_color;
  KinoPixmap background_image;
  BGRepeat background_repeat;
  BGAttachment background_attachment;
  Length background_position_x;
  Length background_position_y;

  Length word_spacing;
  Length letter_spacing;
  int underline;
  int overline;
  int line_through;
  Length vertical_align;
  Transform text_transform;
  Align text_align;
  Length text_indent;
  Length line_height;

  Length margin_top;
  Length margin_right;
  Length margin_bottom;
  Length margin_left;

  Length padding_top;
  Length padding_right;
  Length padding_bottom;
  Length padding_left;

  Length border_top_width;
  Length border_right_width;
  Length border_bottom_width;
  Length border_left_width;
  KinoColor border_top_color;
  KinoColor border_right_color;
  KinoColor border_bottom_color;
  KinoColor border_left_color;
  LineStyle border_top_style;
  LineStyle border_right_style;
  LineStyle border_left_style;
  LineStyle border_bottom_style;

  Float floating;
  Clear clear;
  DisplayStyle display;
  Whitespace white_space;

  ListStyle list_style_type;
  KinoPixmap list_style_image;
  ListPosition list_style_position;
} Element;

typedef struct _Document
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  Element *root;
  char *doctype;
  char *externalID;
  char *publicID;
  char *systemID;
  char *internalSubset;
  int subsetStart;
  ElementDeclaration *elements;
  Entity *entities;
  struct _RuleSet *cssRules;
  char *MIMEType;
  int useHTMLStyles;		/* apply HTML CSS rules? */

  int modified;
#ifdef HAVE_LIBPTHREAD
  int readers;			/* number of reading threads */
  pthread_mutex_t readerMutex;	/* mutex for read-locking the document */
  pthread_mutex_t writerMutex;	/* mutex for write-locking the document */
#endif

  struct _Handler *tagHandler;
  struct _Handler *linkHandler;
  struct _Handler *eventHandler;
  void *userData;
} Document;

typedef struct _ProcessingInstruction
{
  struct _Node *next;
  NodeType type;		/* type identifier */
  struct _Element *parent;

  char *target;			/* first token after <?... */
  char *data;			/* contents of the instruction */
  Attribute *attributes;	/* possibly available attributes (informal!) */
} ProcessingInstruction;

typedef struct
{
  Element *start;
  Element *current;
  char *name;
  char *id;
  char *tag;
  char *class;
} NodeIterator;

typedef struct
{
  int length;
  Node **nodes;
} NodeList;

/* global functions */

/* DOM-ready functions */

NodeType NodeGetNodeType (Node *);
Element *NodeGetParentNode (Node *);
Node *NodeGetPreviousSibling (Node *);
Node *NodeGetNextSibling (Node *);
Node *NodeCloneNode (Node *);

char *ElementGetTagName (Element *);
char *ElementGetAttribute (Element *, char *);
void ElementSetAttribute (Element *, char *, char *);

char *TextGetData (Text *);
void TextSetData (Text *, char *);

/* DOM-equivalent functions */

int NodeHasChildNodes (Node *);
Node* NodeGetFirstChild (Node *);
Node* NodeGetLastChild (Node *);
void NodeAppendChild (Node *, Node *);
void NodeInsertBefore (Node *, Node *refChild, Node *newChild);
Node *NodeReplaceChild (Node *, Node *oldChild, Node *newChild);
Node *NodeRemoveChild (Node *, Node *newChild);

NodeList *ElementGetElementsByTagName (Element *, char *);
NodeList *DocumentGetElementsByTagName (Document *, char *);

/* non-DOM functions */

NodeList *NodeListNew (void);
int NodeListAppendItem (NodeList *, Node *);
Node *NodeListGetItem (NodeList *, int);
Node *NodeListRemoveItem (NodeList *, Node *);
int NodeListGetLength (NodeList *);
void NodeListDelete (NodeList *);

Node *NodeNew (void);
void NodeFree (Node *);
void NodeDelete (Node *);
Element *ElementNew (void);
Node *LinebreakNew (void);
Document *DocumentNew (void);
Text *TextNew (void);
CDATASection *CDATASectionNew (void);
Inset *InsetNew (void);
ProcessingInstruction *ProcessingInstructionNew (void);

void NodePrint (Element *, int);
char *NodeToText (Node *);
char *NodeToPostScript (Element *);
Inset *NodeFindInset (Element *, InsetData *);
int NodeIsSpace (Node *);
void NodePositionAbsolute (Node *);
Element *NodeNextBlock (Node *);

Node *NodeGetSuccessor (Node *);

Element *NodeParentLookup (Node *, char *, char *);
Element *NodeChildLookup (Node *, char *, char *);
Document *NodeGetOwnerDocument (Node *);

char *NodeGetRange (Node *);
void NodeDeleteRange (Node *);
Node *NodeChangeRange (Node *, char *);

void ElementSetInherited (Element *);

void ElementSetContents (Element *, char *);
char *ElementGetContents (Element *);
char *ElementGetRawContents (Element *);
void ElementSetTagName (Element *, char *);

void ElementSetNSPrefix (Element *, char *);
char *ElementGetNSPrefix (Element *);

void ElementCalculateWidth (Element *);
int ElementLeftMargin (Element *, int);
int ElementRightMargin (Element *, int);
int ElementTotalTop (Element *);
int ElementTotalRight (Element *);
int ElementTotalLeft (Element *);
int ElementTotalBottom (Element *);

NodeList *ElementMatch (Element *, Element *);

void DocumentAddRuleSet (Document *, char *, char *);
void DocumentAddDefaultAttribute (Document *, char *, char *, char *);
Attribute *DocumentGetDefaultAttributes (Document *, char *);
char *DocumentGetDefaultAttribute (Document *, char *, char *);
void ElementSetDefaultAttributes (Element *);
void DocumentSetRoot (Document *, Element *);
Element *DocumentGetRoot (Document *);
Element *DocumentGetNonanonymousRoot (Document *);

void DocumentAddTagHandler (Document *, HandlerProc, void *);
void DocumentAddLinkHandler (Document *, HandlerProc, void *);
void DocumentAddEventHandler (Document *, HandlerProc, void *);

void DocumentCallTagHandler (Document *,
  struct _TagHandlerInfo *);
void DocumentCallLinkHandler (Document *, 
  struct _LinkHandlerInfo *);
void DocumentCallEventHandler (Document *, 
  struct _EventHandlerInfo *);

NodeIterator *NodeIteratorNew (Element *, char *, char *, char *, char *);
void NodeIteratorDelete (NodeIterator *);
Element *NodeIteratorNext (NodeIterator *);
Element *NodeIteratorPrev (NodeIterator *);
Element *NodeIteratorFirst (NodeIterator *);
Element *NodeIteratorLast (NodeIterator *);

void ElementDeclarationDelete (ElementDeclaration *);

#endif

