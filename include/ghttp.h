#ifndef ghttp_h
#define ghttp_h

#include "emboss.h"

/*
** Prototype definitions
*/

AjBool gHttpGetBinS(AjPStr url, AjPFile* outf);
AjBool gHttpGetBinC(char* url, AjPFile* outf);
AjBool gHttpPostBinS(AjPStr url, AjPStr params, AjPFile* outf);
AjBool gHttpPostBinC(char* url, AjPStr params, AjPFile* outf);
AjBool gHttpRedirect(AjPFile file, AjPStr* host, ajint* port, AjPStr* path);

#endif
