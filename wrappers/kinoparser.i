
%{
#ifdef NATIVE_NETWORKING
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include <Net.h>
#endif

#include <KinoParser.h>
#include <AHDRuntime.h>

%}

%typemap(ret) char * ElementGetContents, char * NodeToText {
	free ($source);
}

Node * NodeNew (void);
/* Creates and initializes a new node */

Element * ElementNew (void);
/* Creates and initializes a new element */

Document * DocumentNew (void);
/* Creates and initializes a new document */

Text * TextNew (void);
/* Creates and initializes a new text element */

Inset * InsetNew (void);
/* Creates and initializes a new inset element */

CDATASection * CDATASectionNew (void);
/* Creates and initializes a new CDATA section element */

void NodeDelete (Node *);
/* Removes a node from the DOM tree and frees it, including child nodes */

int NodeGetNodeType (Node *);
/* Returns the type of the node (ELEMENT_NODE = 1, ATTRIBUTE_NODE = 2, TEXT_NODE = 3, CDATA_SECTION_NODE = 4, ENTITY_REFERENCE_NODE = 5, ENTITY_NODE = 6, PROCESSING_INSTRUCTION_NODE = 7, COMMENT_NODE = 8, DOCUMENT_NODE = 9, DOCUMENT_TYPE_NODE = 10, DOCUMENT_FRAGMENT_NODE = 11, NOTATION_NODE = 12, INSET_NODE = 9000, LINEBREAK_NODE = 9001)  */

Node * NodeGetParentNode (Node *);
/* Returns the parent Node */

Document * NodeGetOwnerDocument (Node *);
/* Returns the containing document of the Node */

Node * NodeGetPreviousSibling (Node *);
/* Returns the previous sibling (within the child list of the parent node) of a node */

Node * NodeGetNextSibling (Node *);
/* Returns the previous sibling (within the child list of the parent node) of a node */

int NodeHasChildNodes (Node *);
/* returns 1 if a node has child nodes, 0 otherwise */

Node* NodeGetFirstChild (Node *);
/* Returns the first child node of a node */

Node* NodeGetLastChild (Node *);
/* Returns the last child node of a node */

void NodeAppendChild (Node *, Node *);
/* Appends a node at the end of the child list of a given node */

void NodeInsertBefore (Node *, Node *, Node *);
/* Inserts a node before a given node in the child list of a parent node */

Node * NodeReplaceChild (Node *, Node *, Node *);
/* Replaces a child node and returns the replaced node */

Node * NodeRemoveChild (Node *, Node *);
/* Removes a node from the child list and returns the removed node */

void NodePrint (Node *, int);
/* Prints an internal representation of a node and its children */

char * NodeToText (Node *);
/* Returns the XML source text for the given node and its children */

Node * NodeParentLookup (Node *, char *, char *);
/* Looks for a node with a given attribute value in the parent chain of a node */

Node * NodeChildLookup (Node *, char *, char *);
/* Looks for a node with a given attribute value among the children of a node */

Node * NodeGetSuccessor (Node *);
/* Returns the next node in a depth-first traversal */

char * TextGetData (Text *);
/* Returns a reference to the value of a text element */

void TextSetData (Text *, char *);
/* Sets the value of a text element, releases memory held by the old value */

char * CDATASectionGetData (CDATASection *);
/* Returns a reference to the value of a CDATA section element */

void CDATASectionSetData (CDATASection *, char *);
/* Sets the value of a CDATA section element, releases memory held by the old value */

NodeList * DocumentGetElementsByTagName (Document *, char *);
/* Returns a list of sub-elements which have a given tag name or all sub-elements if the given name is "*" */

char * ElementGetTagName (Element *);
/* Returns the tag name of an element */

void ElementSetTagName (Element *, char *);
/* Sets the tag name of an element, releases memory held by old tag name */

char * ElementGetNSPrefix (Element *);
/* Returns the namespace prefix of an element */

void ElementSetNSPrefix (Element *, char *);
/* Sets the namespace prefix of an element, releases memory held by old prefix */

char * ElementGetAttribute (Element *, char *);
/* Returns the value of a given attribute */

void ElementSetAttribute (Element *, char *, char *);
/* Sets the value of an attribute, releases memory held by old attribute value */

void ElementSetContents (Element *, char *);
/* Sets the textual contents of a Element. The contents is stored in a single Text element. Any previous contents is deleted */

volatile char * ElementGetContents (Element *);
/* Returns the textual contents of a Element. Any non-textual context is ignored. The result is a new allocated string which has to be freed by the caller */

void ElementSetDefaultAttributes (Element *);
/* Sets the default attribute values according to the DTD */

void ElementSetInherited (Element *);
/* Copies the inheritable CSS attributes from the parent element */

NodeList * ElementGetElementsByTagName (Element *, char *);
/* Returns a list of sub-elements which have a given tag name or all sub-elements if the given name is "*" */

NodeList * NodeListNew (void);
/* Creates a new NodeList */

void NodeListDelete (NodeList *);
/* Deletes a NodeList (not its contents!) */

int NodeListAppendItem (NodeList *, Node *);
/* Appends a Node at the end of a NodeList */

int NodeListGetLength (NodeList *);
/* Returns the number of items in a NodeList */

Node * NodeListGetItem (NodeList *, int);
/* Returns the Node at the given position */

Node * NodeListRemoveItem (NodeList *, Node *);
/* Removes all occurences of a node from the list */

NodeList * ElementMatch (Element *, Element *);
/* Matches a given Element hierarchy against a pattern hierarchy */

Parser * ParserNew ();
/* Creates a new parser object */

void ParserDelete (Parser *);
/* Deletes a parser object */

void ParserSetStartElement (Parser *, Element *);
/* Sets the start element for a following parse process */

void ParserSetDocument (Parser *, Document *);
/* Sets the start document for the parsing process */

void ParserSetURI (Parser *, char *);

Element * ParserProcessData (Parser *, char *, char *);
/* Process a piece of data after the parser has been prepared with an initial document, URI and start element */
