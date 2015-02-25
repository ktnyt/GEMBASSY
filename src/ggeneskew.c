/******************************************************************************
** @source ggeneskew
**
** Calculate the gene strand bias of the given genome
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.3
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
** @modified 2015/2/7   Refactor
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
#include "glibs.h"




/* @prog ggeneskew ************************************************************
**
** Calculate the gene strand bias of the given genome
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("ggeneskew", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;

  ajint	 window     = 0;
  ajint	 slide      = 0;
  AjBool cumulative = ajFalse;
  AjBool gc3        = ajFalse;
  AjPStr basetype   = NULL;

  AjBool accid  = ajFalse;
  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  AjPFile tmpfile = NULL;
  AjPStr  tmpname = NULL;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  slide      = ajAcdGetInt("slide");
  cumulative = ajAcdGetBoolean("cumulative");
  gc3        = ajAcdGetBoolean("gctri");
  basetype   = ajAcdGetSelectSingle("base");
  accid      = ajAcdGetBoolean("accid");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  if(ajStrMatchC(base, "none"))
    basetype = ajStrNewC("");

  base = ajStrNewC("rest.g-language.org");

  gAssignUniqueName(&tmpname);

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = NULL;

      if(!accid)
        {
          if(gFormatGenbank(seq, &inseq))
            {
              gAssignUniqueName(&tmpname);

              tmpfile = ajFileNewOutNameS(tmpname);

              if(!tmpfile)
                {
                  ajFmtError("Output file (%S) open error\n", tmpname);
                  embExitBad();
                }

              ajFmtPrintF(tmpfile, "%S", inseq);
              ajFileClose(&tmpfile);
              ajFmtPrintS(&url, "http://%S/upload/upl.pl", base);
              gFilePostSS(url, tmpname, &restid);
              ajStrDel(&url);
              ajSysFileUnlinkS(tmpname);
            }
          else
            {
              ajFmtError("Sequence does not have features\n"
                         "Proceeding with sequence accession ID\n");
              accid = ajTrue;
            }
        }

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      if(ajStrGetLen(seqid) == 0)
        {
          ajStrAssignS(&seqid, ajSeqGetNameS(seq));
        }

      if(ajStrGetLen(seqid) == 0)
        {
          ajWarn("No valid header information\n");
        }

      if(accid)
        {
          ajStrAssignS(&restid, seqid);
          if(ajStrGetLen(seqid) == 0)
            {
              ajDie("Cannot proceed without header with -accid\n");
            }

          if(!gValID(seqid))
            {
              ajDie("Invalid accession ID:%S, exiting\n", seqid);
            }
        }

      url = ajStrNew();

      ajFmtPrintS(&url, "http://%S/%S/geneskew/window=%d/slide=%d/"
                  "cumulative=%d/gc3=%d/base=%S/output=f/tag=gene",
                  base, restid, window, slide, cumulative, gc3, basetype);

      if(plot)
        {
          title = ajStrNew();

          ajStrAppendC(&title, argv[0]);
          ajStrAppendC(&title, " of ");
          ajStrAppendS(&title, seqid);

          gpp.title = ajStrNewS(title);
          gpp.xlab = ajStrNewC("gene skew");
          gpp.ylab = ajStrNewC("bp");

          if(!gFilebuffURLS(url, &buff))
            {
              ajDie("File downloading error from:\n%S\n", url);
            }

          if(!gPlotFilebuff(buff, mult, &gpp))
            {
              ajDie("Error in plotting\n");
            }

          AJFREE(gpp.title);
          AJFREE(gpp.xlab);
          AJFREE(gpp.ylab);
          ajStrDel(&title);
          ajFilebuffDel(&buff);
        }
      else
        {
          ajFmtPrintF(outf, "Sequence: %S\n", seqid);
          if(!gFileOutURLS(url, &outf))
            {
              ajDie("File downloading error from:\n%S\n", url);
            }
        }

      ajStrDel(&url);
      ajStrDel(&restid);
      ajStrDel(&seqid);
      ajStrDel(&inseq);
    }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&base);

  embExit();

  return 0;
}
