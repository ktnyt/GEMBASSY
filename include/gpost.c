/******************************************************************************
** @source The last resort for POSTing
**
** @version 1.0
** @modified December 27 2012 Hidetoshi Itaya Created this file
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/

#include "gpost.h"




/* @func gFilePostCC **********************************************************
**
** Posts the file to given url
**
** @param [r] [char*] URL to lookup
** @return [AjPFile]
** @@
******************************************************************************/

AjBool gFilePostCC(char* url, char* filename, AjPStr* string)
{
  CURL *curl;
  CURLcode res;

  struct curl_httppost *post = NULL;
  struct curl_httppost *last = NULL;

  Memory *mem = malloc(sizeof(Memory*));

  mem->size = 0;
  mem->memory = NULL;

  curl = curl_easy_init();

  if(curl)
    {
      curl_formadd(&post, &last,
                   CURLFORM_COPYNAME, "file",
                   CURLFORM_FILE, filename,
                   CURLFORM_END);

      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, mem);

      res = curl_easy_perform(curl);

      if(res)
        {
          return 0;
        }

      curl_formfree(post);
    }
  else
    {
      return 0;
    }

  curl_easy_cleanup(curl);

  ajStrAssignC(string, mem->memory);

  return 1;
}

size_t curl_write(void* ptr, size_t size, size_t nmemb, void* data)
{
  if(size * nmemb == 0)
    return 0;

  size_t realsize = size * nmemb;
  Memory* mem = (Memory*)data;
  mem->memory = (char*)realloc(mem->memory,mem->size + realsize + 1);
  if(mem->memory){
    memcpy(&(mem->memory[mem->size]),ptr,realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }

  return realsize;
}



/* @func gFilePostCS **********************************************************
**
** Posts the file to given url
**
** @param [r] [char*] URL to lookup
** @return [AjPFile]
** @@
******************************************************************************/

AjBool gFilePostCS(char* url, AjPStr filename, AjPStr* string)
{
  if(!gFilePostCC(url, ajCharNewS(filename), string))
    return ajFalse;

  return ajTrue;
}



/* @func gFilePostSS **********************************************************
**
** Posts the file to given url
**
** @param [r] [char*] URL to lookup
** @return [AjPFile]
** @@
******************************************************************************/

AjBool gFilePostSS(AjPStr url, AjPStr filename, AjPStr* string)
{
  if(!gFilePostCC(ajCharNewS(url), ajCharNewS(filename), string))
    return ajFalse;

  return ajTrue;
}
