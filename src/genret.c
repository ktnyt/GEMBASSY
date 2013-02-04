/******************************************************************************
** @source genret
**
** Retrieves various gene related infomration from genome flatfile
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.0   First release
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "emboss.h"
#include "../include/gpost.h"




/* @prog genret ***************************************************************
**
** Retrieves various gene related infomration from genome flatfile
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("genret", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;

  AjPSeq seq    = NULL;
  AjPStr inseq  = NULL;
  AjPStr method = NULL;

  AjBool accid  = ajTrue;
  AjPStr restid = NULL;

  AjPFilebuff buff = NULL;
  AjPFile outfile  = NULL;
  AjPFile tmpfile  = NULL;
  AjPStr  tmpname  = NULL;

  AjPStr argument = NULL;
  AjPStr selector = NULL;
  AjPStr regexstr = NULL;
  AjPStrTok token = NULL;
  AjPRegexp regex = NULL;

  AjPStr url  = NULL;
  AjPStr base = NULL;

  AjPStr head = NULL;
  AjPStr line = NULL;

  AjBool valid = ajFalse;
  AjBool isseq = ajFalse;

  seqall   = ajAcdGetSeqall("sequence");
  method   = ajAcdGetString("method");
  selector = ajAcdGetString("selector");
  argument = ajAcdGetString("argument");
  accid    = ajAcdGetBoolean("accid");
  outfile  = ajAcdGetOutfile("outfile");

  tmpname = ajStrNew();
  gAssignUniqueName(&tmpname);

  ajStrExchangeCC(&selector, "*", ".*");

  base = ajStrNewC("rest.g-language.org");

  if(ajStrMatchC(method, "method_list") || ajStrMatchC(method, "organism_list"))
    {
      ajFmtPrintS(&url, "http://%S/%S/%S", base, method, argument);

      if(!gFileOutURLS(url, &outfile))
        {
          ajFmtError("GET error from %S\n", url);
          embExitBad();
        }

      ajFileClose(&outfile);

      embExit();
    }
    

  if(ajStrMatchC(method, "feature") ||
     ajStrMatchC(method, "cds") ||
     ajStrMatchC(method, "intergenic") ||
     ajStrMatchC(method, "load"))
    {
      isseq = ajTrue;
    }



  /*
  ** Validate the input method
  */

  url = ajStrNew();

  ajFmtPrintS(&url, "http://%S/method_list", base);

  gFilebuffURLS(url, &buff);

  if(!gFilebuffURLS(url, &buff))
    {
      ajFmtError("GET error from %S\n", url);
      embExitBad();
    }

  while(ajBuffreadLine(buff, &line))
    {
      ajStrRemoveLastNewline(&line);

      if(ajStrMatchS(method, line))
        valid = ajTrue;
    }

  ajStrAppendC(&url, "/gb");

  gFilebuffURLS(url, &buff);

  if(!gFilebuffURLS(url, &buff))
    {
      ajFmtError("GET error from %S\n", url);
      embExitBad();
    }

  while(ajBuffreadLine(buff, &line))
    {
      ajStrRemoveLastNewline(&line);

      if(ajStrMatchS(method, line))
        valid = ajTrue;
    }

  if(!valid)
    {
      ajFmtError("%S is not a valid method\n", method);
      embExitBad();
    }

  ajStrDel(&url);
  ajFilebuffDel(&buff);




  /*
  ** Read each sequence
  */

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = ajStrNew();

      if(!accid)
        {
          if(gFormatGenbank(seq, &inseq))
            {
              tmpfile = ajFileNewOutNameS(tmpname);

              if(!tmpfile)
                {
                  ajFmtError("Output file (%S) open error\n", tmpname);
                  embExitBad();
                }

              ajFmtPrintF(tmpfile, "%S", inseq);

              ajFileClose(&tmpfile);

              gFilePostCS("http://rest.g-language.org/upload/upl",
                          tmpname, &restid);

              ajSysFileUnlinkS(tmpname);
            }
          else
            {
              ajFmtError("Sequence does not have features\n"
                         "Proceeding with sequence accession ID\n");
              accid = ajTrue;
            }
        }

      if(accid)
        {
          ajStrAssignS(&restid, ajSeqGetAccS(seq));

          if(!ajStrGetLen(restid))
            ajStrAssignS(&restid, ajSeqGetNameS(seq));

          if(!ajStrGetLen(restid))
            {
              ajFmtError("No valid header information\n");
              embExitBad();
            }
        }

      url = ajStrNew();

      ajFmtPrintS(&url, "http://%S/%S/*/%S/%S", base, restid, method, argument);

      if(!isseq)
        {
          if(!gFileOutURLS(url, &outfile))
            {
              ajFmtError("GET error from %S\n", url);
              embExitBad();
            }

          continue;
        }

      if(!gFilebuffURLS(url, &buff))
        {
          ajFmtError("GET error from %S\n", url);
          embExitBad();
        }

      while(ajBuffreadLine(buff, &line))
        {
          ajStrRemoveLastNewline(&line);

          regex = ajRegCompC("^>");

          if(ajRegExec(regex, line))
            {
              head = ajStrNew();

              ajStrAssignS(&head, line);

              valid = ajFalse;

              token = ajStrTokenNewC(selector, " ,\t\r\n");

              while(ajStrTokenNextParse(&token, &regexstr))
                {
                  regex = ajRegComp(regexstr);
                  valid = ajRegExec(regex, line);
                  ajRegFree(&regex);
                }
            }
          else
            {
              if(!ajStrIsAlnum(line))
                continue;

              if(valid)
                {
                  ajStrFmtWrap(&line, 60);
                  ajFmtPrintF(outfile, "%S\n%S\n", head, line);
                }
            }
        }

      ajFileClose(&outfile);

      ajStrDel(&inseq);
    }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&method);
  ajStrDel(&selector);

  embExit();
}
