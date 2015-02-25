#ifndef gfile_h
#define gfile_h

#include "emboss.h"
#include "curl/curl.h"
#include "curl/easy.h"

#include "gpost.h"

/*
** Prototype definitions
*/

AjBool gValID(AjPStr id);
AjBool gStrAppendURLS(AjPStr url, AjPStr* string);
AjBool gStrAppendURLC(char* url, AjPStr* string);
AjBool gFileOutURLS(AjPStr url, AjPFile* outf);
AjBool gFileOutURLC(char* url, AjPFile* outf);
AjBool gFilebuffURLS(AjPStr url, AjPFilebuff* buff);
AjBool gFilebuffURLC(char* url, AjPFilebuff* buff);
AjBool gFormatGenbank(AjPSeq seq, AjPStr *str);
void gAssignUniqueName(AjPStr *string);
AjPStr gCreateUniqueName();

#endif
