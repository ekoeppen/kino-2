$PACKAGE = "KINOAHD"; $MOTIFPREFIX = 1;

~handles
	KINOAHD

~require
	kinoAHD.inc

 
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

#> Create a new instance of an AHD runtime

AHDRuntime *
AHDRuntimeNew
	in: int	# port number of the internal AHD server or 0 if no server is to be used
	in: NULL | char *	# root directory of the internal AHD server or NULL if no server is to be used

#> Deletes a runtime object

void
AHDRuntimeDelete
	in: AHDRuntime *	# The object to be deleted

#> Add a script handler to the runtime

void
AHDRuntimeAddScriptHandler
	in: AHDRuntime *	# The AHD runtime
	in: HandlerProc	# Pointer to the script handler
	in: void *	# Callback data

#> Initialize a document to be used with a runtime

void
AHDRuntimeDocumentInit
	in: AHDRuntime *	# The AHD runtime
	in: Document *	# The document to be initialized

#> Load a remote document referenced by a URI

Document *
AHDRuntimeLoad
	in: AHDRuntime *	# The AHD runtime
	in: char *	# The document's URI

#> Removes a document from the runtime

void
AHDRuntimeUnload
	in: AHDRuntime *	# The AHD runtime
	in: Document *	# The document to be unloaded

#> Saves a document under its original location (only for runtime-local documents)

void
AHDRuntimeSave
	in: AHDRuntime *	# The AHD runtime
	in: Document *	# The document to be saved

#> Calls an AHD function within a document using parent delegation

char *
AHDRuntimeCall
	in: AHDRuntime *	# The AHD runtime
	in: Node *	# The current node where the call originates
	in: char *	# The function name
	in: char *	# The URL encoded parameters in name/value pairs

#> Broadcast call of an AHD function within a document

void
AHDRuntimeBroadcastCall
	in: AHDRuntime *	# The AHD runtime
	in: Node *	# The current node where the call originates
	in: char *	# The function name
	in: char *	# The URL encoded parameters in name/value pairs

#> Returns the value of an AHD variable using parent delegation

char *
AHDRuntimeGet
	in: AHDRuntime *	# The AHD runtime
	in: Node *	# The current node where the call originates
	in: char *	# The variable name

#> Sets the value of an AHD variable using parent delegation

void
AHDRuntimePut
	in: AHDRuntime *	# The AHD runtime
	in: Node *	# The current node where the call originates
	in: char *	# The variable name
	in: char *	# The value of the variable

#> Matches a given Element hierarchy against a pattern hierarchy, triggering onmatch events

NodeList *
AHDRuntimeMatch
	in: AHDRuntime *	# the runtime executing the onmatch events
	in: Element *	# the given hierarchy
	in: Element *	# the pattern hierarchy


void
AHDRuntimeSetScriptHandler
	in: AHDRuntime *
	in: String
 