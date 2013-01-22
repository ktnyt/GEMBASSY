#ifndef ghttp_h
#define ghttp_h

#include "emboss.h"

/*
** Prototype definitions
*/

AjBool gHttpWriteBinaryS(AjPStr url, AjPFile* outf);
AjBool gHttpWriteBinaryC(char* url, AjPFile* outf);
AjBool gHttpRedirect(AjPFile file, AjPStr* host, ajint* port, AjPStr* path);

#endif
