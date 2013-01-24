/******************************************************************************
** @source GEMBASSY file routines
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

#include "ghttp.h"




/* @func gHttpGetBinS *********************************************************
**
** Writes out remote binary file to AjPFile
**
** @param [r] [char*] URL to lookup
** @return [AjPFile]
** @@
******************************************************************************/

AjBool gHttpGetBinS(AjPStr url, AjPFile* outf)
{
  AjPFile file = NULL;
  AjPStr  line = NULL;
  AjPStr  host = NULL;
  AjPStr  path = NULL;
  AjPStr  get  = NULL;
  ajint   port = 80;
  FILE   *fp;

  AjPRegexp crlf = NULL;

  char buf[8];

  struct AJSOCKET sock;

  get = ajStrNew();

  ajHttpUrlDeconstruct(url, &port, &host, &path);

  while(file==NULL || gHttpRedirect(file, &host, &port, &path))
    {
      if(ajStrGetCharFirst(path) != '/')
	ajStrInsertK(&path, 0, '/');

      ajFmtPrintS(&get, "GET http://%S:%d%S HTTP/1.1\r\n", host, port, path);

      fp = ajHttpOpen(NULL, host, port, get, &sock);

      file = ajFileNewFromCfile(fp);

      if(!file)
	return ajFalse;
    }

  ajStrDel(&get);

  crlf = ajRegCompC("^\r?\n$");

  while(ajReadline(file, &line))
    {
      if(ajRegExec(crlf, line))
	break;
    }

  while(ajReadbinBinary(file, 1, 1, buf))
    {
      ajWritebinBinary(*outf, 1, 1, buf);
    }

  ajFileClose(outf);
  ajFileClose(&file);

  return ajTrue;
}




/* @func gHttpGetBinC *********************************************************
**
** Retrives the C file pointer from a given URL
**
** @param [r] [char*] URL to lookup
** @return [FILE*]
** @@
******************************************************************************/

AjBool gHttpGetBinC(char* url, AjPFile* outf)
{
  if(!gHttpGetBinS(ajStrNewC(url), outf))
    return ajFalse;

  return ajTrue;
}




/* @func gHttpPostBinS ********************************************************
**
** Writes out remote binary file to AjPFile
**
** @param [r] [char*] URL to lookup
** @return [AjPFile]
** @@
******************************************************************************/

AjBool gHttpPostBinS(AjPStr url, AjPStr params, AjPFile* outf)
{
  AjPFile file = NULL;
  AjPStr  line = NULL;
  AjPStr  host = NULL;
  AjPStr  path = NULL;
  AjPStr  post = NULL;
  ajint   port = 80;
  FILE   *fp;

  AjPRegexp crlf = NULL;

  char buf[8];

  struct AJSOCKET sock;

  post = ajStrNew();

  ajHttpUrlDeconstruct(url, &port, &host, &path);

  while(file==NULL || gHttpRedirect(file, &host, &port, &path))
    {
      if(ajStrGetCharFirst(path) != '/')
	ajStrInsertK(&path, 0, '/');

      ajFmtPrintS(&post,
		  "POST %S HTTP/1.1\nHost:%S\n$SContent-Length: %d\n%S\r\n",
		  path, host, ajStrGetLen(params), params);

      fp = ajHttpOpen(NULL, host, port, post, &sock);

      file = ajFileNewFromCfile(fp);

      if(!file)
	return ajFalse;
    }

  ajStrDel(&post);

  crlf = ajRegCompC("^\r?\n$");

  while(ajReadline(file, &line))
    {
      if(ajRegExec(crlf, line))
	break;
    }

  while(ajReadbinBinary(file, 1, 1, buf))
    {
      ajWritebinBinary(*outf, 1, 1, buf);
    }

  ajFileClose(outf);
  ajFileClose(&file);

  return ajTrue;
}




/* @func gHttpPostBinC *********************************************************
**
** Retrives the C file pointer from a given URL
**
** @param [r] [char*] URL to lookup
** @return [FILE*]
** @@
******************************************************************************/

AjBool gHttpPostBinC(char* url, AjPStr params, AjPFile* outf)
{
  if(!gHttpPostBinS(ajStrNewC(url), params, outf))
    return ajFalse;

  return ajTrue;
}




/* @func gHttpRedirect ********************************************************
**
** Reads the header of http response in given buffer buff,
** if it includes a redirection response updates the host, port and get
** parameters using the 'Location' header
**
** @param [u] buff [FILE*] file pointer
** @param [w] host [AjPStr*] Host name
** @param [w] port [ajint*] Port
** @param [w] path [AjPStr*] part of URL after port number
** @return [AjBool] returns true if the header includes a redirection response
** @@
******************************************************************************/

AjBool gHttpRedirect(AjPFile file, AjPStr* host, ajint* port, AjPStr* path)
{
  AjPFilebuff buff = NULL;

  AjPRegexp httpexp  = NULL;
  AjPRegexp nullexp  = NULL;
  AjPRegexp redirexp = NULL;

  AjPStr codestr  = NULL;
  AjPStr newurl   = NULL;
  AjPStr newhost  = NULL;
  AjPStr currline = NULL;

  ajuint httpcode = 0;

  AjBool isheader = ajFalse;
  AjBool ret = ajFalse;

  httpexp  = ajRegCompC("^HTTP/\\S+\\s+(\\d+)");

  ajReadline(file, &currline);

  ajDebug("gHttpRedirect: First line: '%S'\n", currline);

  if(ajRegExec(httpexp, currline))
    {
      isheader = ajTrue;
      ajRegSubI(httpexp, 1, &codestr);
      ajStrToUint(codestr, &httpcode);
      ajDebug("Header: codestr '%S' code '%u'\n", codestr, httpcode);
      ajStrDel(&codestr);
    }

  if(isheader)
    {
      if(httpcode == 301 || httpcode == 302 || httpcode==307)
        {
	  redirexp = ajRegCompC("^Location: (\\S+)");
	  nullexp  = ajRegCompC("^\r?\n?$");

	  while( ajReadline(file, &currline) &&
		 !ajRegExec(nullexp, currline))
            {
	      ajDebug("gHttpRedirect: header line: '%S'\n", currline);

	      if(ajRegExec(redirexp, currline))
                {
		  ajRegSubI(redirexp, 1, &newurl);
		  ajHttpUrlDeconstruct(newurl, port, &newhost, path);

		  if(ajStrGetLen(newhost))
		    ajStrAssignS(host, newhost);

		  ajStrDel(&newurl);
		  ajStrDel(&newhost);
		  ret = ajTrue;
		  break;
                }
            }

	  ajRegFree(&redirexp);
	  ajRegFree(&nullexp);
        }
    }

  ajRegFree(&httpexp);
  ajStrDel(&currline);

  return ret;
}


