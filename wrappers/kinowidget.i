
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
#include <HTMLFormTags.h>

%}

Element * HTMLInputElementNew (Document *);
/* Creates a new HTML FORM Input element */

Element * HTMLTextareaElementNew (Document *);
/* Creates a new HTML FORM Textarea element */

Element * HTMLButtonElementNew (Document *);
/* Creates a new HTML FORM Button element */

void ElementSetCSSProperties (Element *);
/* Sets the value of a CSS property */

void ElementSetCSSProperty (Element *, char *, char *);
/* Sets the value of a CSS property */

char * ElementGetCSSProperty (char *);
/* Returns the value of a CSS property */
