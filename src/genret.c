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
#include <sys/ioctl.h>




/* @prog genret ***************************************************************
**
** Retrieves various gene related infomration from genome flatfile
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("genret", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;

  AjPSeq seq;
  AjPStr inseq  = NULL;
  AjPStr method = NULL;
  AjPStr option = 0;
  AjBool trans  = 0;
  AjPStr accid  = NULL;
  AjPStr restid = NULL;

  ajuint count  = 0;
  AjBool multi  = ajFalse;
  AjBool isflat = ajFalse;

  AjPStr select = NULL;
  AjPStr parse  = NULL;
  ajuint total  = 0;
  ajuint iter   = 0;
  float percent = 0;
  AjPStrTok tok = NULL;

  AjPStr content = NULL;

  AjPStr  tmpfname = NULL;
  AjPFile outfile  = NULL;
  AjPFile tmpfile  = NULL;
  AjPFilebuff buff = NULL;

  AjPStr url  = NULL;
  AjPStr line = NULL;

  struct winsize w;
  struct AJTIMEOUT timo;

  seqall  = ajAcdGetSeqall("genome");
  method  = ajAcdGetString("method");
  select  = ajAcdGetString("selector");
  option  = ajAcdGetString("option");
  trans   = ajAcdGetBoolean("translate");
  accid   = ajAcdGetString("accid");
  outfile = ajAcdGetOutfile("outfile");

  tmpfname = ajStrNew();
  gAssignUniqueName(&tmpfname);

  ajStrRemoveWhite(&select);
  tok = ajStrTokenNewC(select, ",");
  total = ajStrCalcCountK(select, ',');

  if(ajStrMatchCaseC(method, "feature") ||
     ajStrMatchCaseC(method, "cds") ||
     ajStrMatchCaseC(method, "gene") ||
     ajStrMatchCaseC(method, "intergenic"))
    {
      isflat = ajTrue;
    }

  while(ajSeqallNext(seqall, &seq))
    {
      inseq  = ajStrNew();
      restid = ajStrNew();

      if(!gFormatGenbank(seq, &inseq) || ajStrGetLen(accid))
	{
          if(ajStrMatchC(method, "load"))
            {
              ajFmtError("Cannot load sequence without features\n");
              embExitBad();
            }

          if(!ajStrGetLen(accid))
            {
              ajStrAssignS(&accid, ajSeqGetNameS(seq));
            }

          ajStrAssignS(&restid, ajSeqGetAccS(seq));

          if(!ajStrGetLen(restid))
            ajStrAssignS(&restid, accid);
        }
      else
        {
          tmpfile = ajFileNewOutNameS(tmpfname);
          ajFmtPrintF(tmpfile, "%S", inseq);
          ajFileClose(&tmpfile);

          gFilePostCS("http://rest.g-language.org/upload/upl.pl",
                      tmpfname, &restid);

          ajSysFileUnlinkS(tmpfname);

          if(!ajStrGetLen(accid))
            ajStrAssignS(&accid, ajSeqGetAccS(seq));

          ajStrAssignS(&inseq, ajSeqGetSeqS(seq));

          if(ajStrMatchC(method, "load"))
            {
              sleep(2);

              ajStrFmtWrap(&inseq, 60);

              ajFmtPrintF(outfile, ">%S|%S\n%S\n", restid, accid, inseq);

              continue;
            }
        }

      multi = ajStrTokenNextParse(&tok, &parse);

      count = 0;

      do
        {
          ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

          url     = ajStrNew();
          line    = ajStrNew();
          content = ajStrNew();

          if(!multi)
            ajStrAssignS(&parse, select);

          if(isflat)
            ajStrAssignC(&parse, "");

          if(total)
            {
              ajFmtError("\r[");
              percent = (float) count / (float) total;
              for(iter = 0; (float)iter < percent * (w.ws_col - 10); ++iter)
                {
                  ajFmtError("=");
                }
              ajFmtError(">");
              for(; iter < (w.ws_col - 10); ++iter)
                {
                  ajFmtError(" ");
                }
              ajFmtError("]");
              ajFmtError(" %3.0f %%", percent * 100);
            }

          timo.seconds = 180;
          ajSysTimeoutSet(&timo);
          ajFmtPrintS(&url, "http://rest.g-language.org/%S/%S/%S/%S",
                      restid, method, parse, option);
          ajSysTimeoutUnset(&timo);

          gFilebuffURLS(url, &buff);

          while(ajBuffreadLine(buff, &line))
            {
              ajStrAppendS(&content, line);
            }

          ajStrFmtWrap(&content, 60);

          if(!ajStrMatchC(parse, "*") && !isflat)
            ajFmtPrintF(outfile, ">%S\n", parse);

          ajFmtPrintF(outfile, "%S", content);

          ajStrDel(&url);
          ajStrDel(&line);
          ajStrDel(&content);

          if(count)
            sleep(2);

          if(isflat)
            continue;

          ++count;
        }
      while(ajStrTokenNextParse(&tok, &parse));

      ajStrDel(&inseq);
    }

  ajFileClose(&outfile);

  if(total)
    {
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
      ajFmtError("\r");
      for(iter = 0; (float)iter < w.ws_col; ++iter)
        {
          ajFmtError(" ");
        }
      ajFmtError("\r[");
      percent = (float) count / (float) total;
      for(iter = 0; (float)iter < percent * (w.ws_col - 10); ++iter)
        {
          ajFmtError("=");
        }
      ajFmtError("=");
      for(; iter < (w.ws_col - 10); ++iter)
        {
          ajFmtError(" ");
        }
      ajFmtError("]");
      ajFmtError(" %3.0f %%", percent * 100);
      ajFmtError("\n");
    }

  if(ajStrMatchC(method, "load"))
    {
      ajFmtError("Use the output sequence to query genret\n");
    }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  embExit();
}
