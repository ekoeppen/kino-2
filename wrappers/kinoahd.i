
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

AHDRuntime * AHDRuntimeNew (int, char *);
/* Create a new instance of an AHD runtime */

void AHDRuntimeDelete (AHDRuntime *);
/* Deletes a runtime object */

void AHDRuntimeAddScriptHandler (AHDRuntime *, HandlerProc, void *);
/* Add a script handler to the runtime */

void AHDRuntimeDocumentInit (AHDRuntime *, Document *);
/* Initialize a document to be used with a runtime */

Document * AHDRuntimeLoad (AHDRuntime *, char *);
/* Load a remote document referenced by a URI */

void AHDRuntimeUnload (AHDRuntime *, Document *);
/* Removes a document from the runtime */

void AHDRuntimeSave (AHDRuntime *, Document *);
/* Saves a document under its original location (only for runtime-local documents) */

void AHDRuntimeStore (AHDRuntime *, Document *, char *, char *);
/* Stores a document in a remote runtime and activates it there */

char * AHDRuntimeCall (AHDRuntime *, Node *, char *, char *);
/* Calls an AHD function within a document using parent delegation */

void AHDRuntimeBroadcastCall (AHDRuntime *, Node *, char *, char *);
/* Broadcast call of an AHD function within a document */

char * AHDRuntimeGet (AHDRuntime *, Node *, char *);
/* Returns the value of an AHD variable using parent delegation */

char * AHDRuntimePut (AHDRuntime *, Node *, char *, char *);
/* Sets the value of an AHD variable using parent delegation */

NodeList * AHDRuntimeMatch (AHDRuntime *, Element *, Element *);
/* Matches a given Element hierarchy against a pattern hierarchy, triggering onmatch events */

char * AHDRuntimeHere (AHDRuntime *);
/* Returns the hostname and port number of the given AHD runtime */

NodeList * AHDRuntimeGetLoadedDocuments (AHDRuntime *);
/* Returns a list of the currently loaded documents in the runtime */

