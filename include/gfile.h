#ifndef gfile_h
#define gfile_h

#include "emboss.h"

/*
** Prototype definitions
*/

AjBool gValID(AjPStr id);
AjBool gGetFileFromURL(AjPStr url, AjPStr filename);
AjPStr gGetUniqueFileName(void);
AjPStr gFormatGenbank(AjPSeq seq);
AjBool gGetFileContent(AjPStr* content, AjPStr filename);

#endif
