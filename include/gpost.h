#ifndef gpost_h
#define gpost_h

#include "emboss.h"
#include <curl/curl.h>
#include <curl/easy.h>

typedef struct{
  char* memory;
  size_t size;
} Memory;

/*
** Prototype definitions
*/

AjBool gFilePostCC(char* url, char* filename, AjPStr* string);
AjBool gFilePostCS(char* url, AjPStr filename, AjPStr* string);
AjBool gFilePostSS(AjPStr url, AjPStr filename, AjPStr* string);
size_t curl_write(void *ptr, size_t size, size_t nmemb, void *stream);

#endif
