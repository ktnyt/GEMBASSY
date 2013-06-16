#ifndef ghttp_h
#define ghttp_h

#include "emboss.h"

/*
** Prototype definitions
*/

AjBool gHttpConvertS(AjPStr url, AjPFile* outf, AjPStr informat, AjPStr outformat);
AjBool gHttpConvertC(char* res, AjPFile* outf, AjPStr informat, AjPStr outformat);
AjBool gHttpGetBinS(AjPStr url, AjPFile* outf);
AjBool gHttpGetBinC(char* url, AjPFile* outf);
AjPFilebuff gHttpPostFileSS(AjPStr url, AjPStr filename);
AjPFilebuff gHttpPostFileCS(char* url, AjPStr filename);
AjBool gHttpRedirect(AjPFile file, AjPStr* host, ajint* port, AjPStr* path);

#endif
