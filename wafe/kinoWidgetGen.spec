$PACKAGE = "KINOWIDGET"; $MOTIFPREFIX = 1;

~handles
	KINOWIDGET

~require
	kinoWidget.inc
	rConv "VALIGN" VAlign arg \
		"baseline","top","sub","super","text_top",\
		"middle","bottom","text_bottom" \
	      	VALIGN_BASELINE,VALIGN_TOP,VALIGN_SUB,\
		VALIGN_SUPER,VALIGN_TEXT_TOP,\
		VALIGN_MIDDLE,VALIGN_BOTTOM,VALIGN_TEXT_BOTTOM
	rConv "EventCode" EventCode arg \
		"none","click","dbl_click",\
		"motion","enter","leave","key" \
		EE_NONE,EE_CLICK,EE_DBL_CLICK,\
		EE_MOTION,EE_ENTER,EE_LEAVE,EE_KEY

 
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

~widgetClass
Kino
	include <KinoParser.h>
	include <Kino.h>
	include <KinoP.h>

#########################################################################

void
KinoRelayout
	in: Widget

void
KinoRefresh
	in: Widget

void
KinoSetDocument
	in: KinoWidget
	in: NULL | Document *

void
KinoDocumentSetHandlers
	in: Document *
	in: KinoWidget

Document *
KinoGetDocument
	in: KinoWidget

void
StyleSetBoxProperty
	in: Element *
	in: String
	in: String

void
StyleSetBoxProperties
	in: Element *

String
StyleGetBoxProperty
	in: Element *
	in: String
