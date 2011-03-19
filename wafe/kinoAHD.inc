/* $Id: kinoAHD.inc,v 1.1 1999/01/25 11:03:37 koeppen Exp $ */

#include "KinoParser.h"
#include "AHDRuntime.h"

extern Tcl_HashTable kinoParserTable;  /* for stringToP and friends */

extern XrmQuark qcallback;

void wafeScriptHandler (HandlerInfo *info)
{
  char *cmd;
  
  cmd = malloc (256 + max (strlen (info->info.script->script),
    strlen (info->info.script->params)));
  Tcl_GlobalEval (wafeInterpreter, "unset handlerInfo");

  sprintf (cmd, "set handlerInfo(document) \"%s\"", 
    wafePtrToString (info->document, "Node_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(element) \"%s\"", 
    wafePtrToString (info->info.script->element, "Node_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(caller) \"%s\"", 
    wafePtrToString (info->info.script->caller, "Node_", kinoParserTable));
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(script) {%s}", info->info.script->script);
  Tcl_GlobalEval (wafeInterpreter, cmd);

  sprintf (cmd, "set handlerInfo(params) {%s}", info->info.script->params);
  Tcl_GlobalEval (wafeInterpreter, cmd);

  if (Tcl_GlobalEval (wafeInterpreter, info->userData) != TCL_OK)
  {
    fprintf (stderr, "ScriptCallback error: %s",
      Tcl_GetStringResult (wafeInterpreter));
  }
  info->info.script->ret = Tcl_GetStringResult (wafeInterpreter);
  free (cmd);
}

void AHDRuntimeSetScriptHandler (AHDRuntime *r, char *scriptHandler)
{
  AHDRuntimeAddScriptHandler (r, wafeScriptHandler,
    XkNewString (scriptHandler));
}

/*
 * Local Variables:
 * mode: c
 */
