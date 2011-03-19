$PACKAGE = "KINOPARSER"; $MOTIFPREFIX = 1;

~handles
	KINOPARSER

~require
	kinoParser.inc
	init Tcl_InitHashTable(&kinoParserTable, TCL_STRING_KEYS);

 
$stringTo{"Parser *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%Parser *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"Parser_",kinoParserTable)';
$toString{"Parser *"} = 'wafePtrToString(INPUT, "Parser_", kinoParserTable)';
$toStringGarbage{"Parser *"} = "TCL_VOLATILE";

$stringTo{"TagHandlerInfo *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%TagHandlerInfo *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"TagHandlerInfo_",kinoParserTable)';
$toString{"TagHandlerInfo *"} = 'wafePtrToString(INPUT, "TagHandlerInfo_", kinoParserTable)';
$toStringGarbage{"TagHandlerInfo *"} = "TCL_VOLATILE";

$stringTo{"LinkHandlerInfo *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%LinkHandlerInfo *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"LinkHandlerInfo_",kinoParserTable)';
$toString{"LinkHandlerInfo *"} = 'wafePtrToString(INPUT, "LinkHandlerInfo_", kinoParserTable)';
$toStringGarbage{"LinkHandlerInfo *"} = "TCL_VOLATILE";

$stringTo{"EventHandlerInfo *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%EventHandlerInfo *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"EventHandlerInfo_",kinoParserTable)';
$toString{"EventHandlerInfo *"} = 'wafePtrToString(INPUT, "EventHandlerInfo_", kinoParserTable)';
$toStringGarbage{"EventHandlerInfo *"} = "TCL_VOLATILE";

$stringTo{"ScriptHandlerInfo *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%ScriptHandlerInfo *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"ScriptHandlerInfo_",kinoParserTable)';
$toString{"ScriptHandlerInfo *"} = 'wafePtrToString(INPUT, "ScriptHandlerInfo_", kinoParserTable)';
$toStringGarbage{"ScriptHandlerInfo *"} = "TCL_VOLATILE";

$stringTo{"NodeList *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"NodeList *"} = 'wafePtrToString(INPUT, "NodeList_", kinoParserTable)';
$toStringGarbage{"NodeList *"} = "TCL_VOLATILE";

$stringTo{"Node *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"Node *"} = 'wafePtrToString(INPUT, "Node_", kinoParserTable)';
$toStringGarbage{"Node *"} = "TCL_VOLATILE";

$stringTo{"Element *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%Element *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"Node_",kinoParserTable)';
$toString{"Element *"} = 'wafePtrToString(INPUT, "Node_", kinoParserTable)';
$toStringGarbage{"Element *"} = "TCL_VOLATILE";

$stringTo{"Document *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"%Document *"} = 'wafeStringAppendPtr(OUTPUT, INPUT,"Node_",kinoParserTable)';
$toString{"Document *"} = 'wafePtrToString(INPUT, "Node_", kinoParserTable)';
$toStringGarbage{"Document *"} = "TCL_VOLATILE";

$stringTo{"Text *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"Text *"} = 'wafePtrToString(INPUT, "Node_", kinoParserTable)';
$toStringGarbage{"Text *"} = "TCL_VOLATILE";

$stringTo{"Inset *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"Inset *"} = 'wafePtrToString(INPUT, "Node_", kinoParserTable)';
$toStringGarbage{"Inset *"} = "TCL_VOLATILE";

$stringTo{"AHDRuntime *"} = "wafeStringToPtr(INPUT, kinoParserTable, (void *)&OUTPUT)";
$toString{"AHDRuntime *"} = 'wafePtrToString(INPUT, "AHDRuntime_", kinoParserTable)';
$toStringGarbage{"AHDRuntime *"} = "TCL_VOLATILE";

#> Creates and initializes a new node

Node *
NodeNew


#> Creates and initializes a new element

Element *
ElementNew


#> Creates and initializes a new document

Document *
DocumentNew


#> Creates and initializes a new text element

Text *
TextNew


#> Creates and initializes a new inset element

Inset *
InsetNew


#> Removes a node from the DOM tree and frees it, including child nodes

void
NodeDelete
	in: Node *	# the node to be freed

#> Returns the type of the node (ELEMENT_NODE = 1, ATTRIBUTE_NODE = 2, TEXT_NODE = 3, CDATA_SECTION_NODE = 4, ENTITY_REFERENCE_NODE = 5, ENTITY_NODE = 6, PROCESSING_INSTRUCTION_NODE = 7, COMMENT_NODE = 8, DOCUMENT_NODE = 9, DOCUMENT_TYPE_NODE = 10, DOCUMENT_FRAGMENT_NODE = 11, NOTATION_NODE = 12, INSET_NODE = 9000, LINEBREAK_NODE = 9001) 

int
NodeGetNodeType
	in: Node *	# the current node

#> Returns the parent Node

Node *
NodeGetParentNode
	in: Node *	# the current node

#> Returns the containing document of the Node

Document *
NodeGetOwnerDocument
	in: Node *	# the current node

#> Returns the previous sibling (within the child list of the parent node) of a node

Node *
NodeGetPreviousSibling
	in: Node *	# the current node

#> Returns the previous sibling (within the child list of the parent node) of a node

Node *
NodeGetNextSibling
	in: Node *	# the current node

#> returns 1 if a node has child nodes, 0 otherwise

int
NodeHasChildNodes
	in: Node *	# the current node

#> Returns the first child node of a node

Node*
NodeGetFirstChild
	in: Node *	# the current node

#> Returns the last child node of a node

Node*
NodeGetLastChild
	in: Node *	# the current node

#> Appends a node at the end of the child list of a given node

void
NodeAppendChild
	in: Node *	# the node containing the child list
	in: Node *	# the node to be appended

#> Inserts a node before a given node in the child list of a parent node

void
NodeInsertBefore
	in: Node *	# the node containing the child list
	in: Node *	# the node before the new node is to be inserted or NULL if the new node is to be appended at the end of the child list
	in: Node *	# the node to be inserted

#> Replaces a child node and returns the replaced node

Node *
NodeReplaceChild
	in: Node *	# the node containing the child list
	in: Node *	# the node to be replaced
	in: Node *	# the new node

#> Removes a node from the child list and returns the removed node

Node *
NodeRemoveChild
	in: Node *	# the node containing the child list
	in: Node *	# the node to be removed

#> Prints an internal representation of a node and its children

void
NodePrint
	in: Node *	# the node to be printed
	in: int	# the indentiation level

#> Returns the XML source text for the given node and its children

char *
NodeToText
	in: Node *	# the node to be inspected

#> Looks for a node with a given attribute value in the parent chain of a node

Node *
NodeParentLookup
	in: Node *	# the current node
	in: char *	# the attribute name
	in: char *	# the attribute value to look for

#> Looks for a node with a given attribute value among the children of a node

Node *
NodeChildLookup
	in: Node *	# the current node
	in: char *	# the attribute name
	in: char *	# the attribute value to look for

#> Returns the next node in a depth-first traversal

Node *
NodeGetSuccessor
	in: Node *	# the current node

#> Returns a reference to the value of a text element

char *
TextGetData
	in: Text *	# the current element

#> Sets the value of a text element, releases memory held by the old value

void
TextSetData
	in: Text *	# the current element
	in: char *	# the new value

#> Returns a list of sub-elements which have a given tag name or all sub-elements if the given name is "*"

NodeList *
DocumentGetElementsByTagName
	in: Document *	# the document where the search starts
	in: char *	# the tag name to look for

#> Returns the tag name of an element

char *
ElementGetTagName
	in: Element *	# the current element

#> Sets the tag name of an element, releases memory held by old tag name

void
ElementSetTagName
	in: Element *	# the current element
	in: char *	# the new tag name

#> Returns the value of a given attribute

char *
ElementGetAttribute
	in: Element *	# the current element
	in: char *	# the attribute name

#> Sets the value of an attribute, releases memory held by old attribute value

void
ElementSetAttribute
	in: Element *	# the current element
	in: char *	# the attribute name
	in: char *	# the new attribute value

#> Sets the textual contents of a Element. The contents is stored in a single Text element. Any previous contents is deleted

void
ElementSetContents
	in: Element *	# the current element
	in: char *	# the new contents

#> Returns the textual contents of a Element. Any non-textual context is ignored. The result is a new allocated string which has to be freed by the caller

char *
ElementGetContents
	in: Element *	# the current element

#> Sets the default attribute values according to the DTD

void
ElementSetDefaultAttributes
	in: Element *	# the current element

#> Copies the inheritable CSS attributes from the parent element

void
ElementSetInherited
	in: Element *	# the current element

#> Returns a list of sub-elements which have a given tag name or all sub-elements if the given name is "*"

NodeList *
ElementGetElementsByTagName
	in: Element *	# the element where the search starts
	in: char *	# the tag name to look for

#> Creates a new NodeList

NodeList *
NodeListNew


#> Deletes a NodeList (not its contents!)

void
NodeListDelete
	in: NodeList *	# the NodeList to be deleted

#> Appends a Node at the end of a NodeList

int
NodeListAppendItem
	in: NodeList *	# the NodeList where the Node is appended
	in: Node *	# the Node to be appended

#> Returns the number of items in a NodeList

int
NodeListGetLength
	in: NodeList *	# the NodeList

#> Returns the Node at the given position

Node *
NodeListGetItem
	in: NodeList *	# the NodeList
	in: int	# the position (starting at zero)

#> Removes all occurences of a node from the list

Node *
NodeListRemoveItem
	in: NodeList *	# the NodeList
	in: Node *	# the Node to be removed

#> Matches a given Element hierarchy against a pattern hierarchy

NodeList *
ElementMatch
	in: Element *	# the given hierarchy
	in: Element *	# the pattern hierarchy

#> Creates a new parser object

Parser *
ParserNew

#> Deletes a parser object

void
ParserDelete
	in: Parser *	# The parser object to be deleted

#> Sets the start element for a following parse process

void
ParserSetStartElement
	in: Parser *
	in: Element *

#> Sets the start document for the parsing process

void
ParserSetDocument
	in: Parser *
	in: Document *

void
ParserSetURI
	in: Parser *
	in: char *

#> Process a piece of data after the parser has been prepared with an initial document, URI and start element

Element *
ParserProcessData
	in: Parser *	# The parser to be used for parsing
	in: char *	# the MIME type of the text to be parsed (possible values are currently text/xml, text/html, text/css and text/plain)
	in: char *	# the text to be parsed

