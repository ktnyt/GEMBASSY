/******************************************************************************
** @source ggcsi
**
** GC Skew Index: an index for strand-specific mutational bias
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.3
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
** @modified 2015/2/7   RESTify
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




/* @prog ggcsi ****************************************************************
**
** GC Skew Index: an index for strand-specific mutational bias
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("ggcsi", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq   = NULL;

  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  ajint	 window  = 0;
  AjBool at      = 0;
  AjBool purine  = 0;
  AjBool keto    = 0;
  AjBool pval    = 0;
  AjPStr version = NULL;

  AjPStr      tmpname = NULL;
  AjPSeqout   tmpout  = NULL;
  AjPFilebuff tmpbuff = NULL;

  AjPStr tmp   = NULL;
  AjPStr parse = NULL;
  AjPStr gcsi  = NULL;
  AjPStr sa    = NULL;
  AjPStr dist  = NULL;
  AjPStr z     = NULL;
  AjPStr p     = NULL;

  AjPStrTok handle  = NULL;

  AjPFile outf = NULL;

  seqall  = ajAcdGetSeqall("sequence");
  window  = ajAcdGetInt("window");
  at      = ajAcdGetBoolean("at");
  purine  = ajAcdGetBoolean("purine");
  keto    = ajAcdGetBoolean("keto");
  pval    = ajAcdGetBoolean("pval");
  version = ajAcdGetSelectSingle("gcsi");
  outf    = ajAcdGetOutfile("outfile");

  base = ajStrNewC("rest.g-language.org");

  gAssignUniqueName(&tmpname);
  ajStrAppendC(&tmpname, ".fasta");

  while(ajSeqallNext(seqall, &seq))
    {
      tmpout = ajSeqoutNew();

      if(!ajSeqoutOpenFilename(tmpout, tmpname))
        {
          embExitBad();
        }

      ajSeqoutSetFormatS(tmpout,ajStrNewC("fasta"));
      ajSeqoutWriteSeq(tmpout, seq);
      ajSeqoutClose(tmpout);
      ajSeqoutDel(&tmpout);

      ajFmtPrintS(&url, "http://%S/upload/upl.pl", base);
      gFilePostSS(url, tmpname, &restid);
      ajStrDel(&url);
      ajSysFileUnlinkS(tmpname);

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      if(ajStrGetLen(seqid) == 0)
        {
          ajStrAssignS(&seqid, ajSeqGetNameS(seq));
        }

      if(ajStrGetLen(seqid) == 0)
        {
          ajWarn("No valid header information\n");
        }

      url = ajStrNew();

      ajFmtPrintS(&url, "http://%S/%S/gcsi/window=%d/at=%d/purine=%d/"
                  "keto=%d/p=%d/version=%d/", base, restid, window, at, purine,
                  keto, pval, version);

      if(!gFilebuffURLS(url, &tmpbuff))
        {
          ajDie("Failed to download result from:\n%S\n", url);
        }

      ajBuffreadLine(tmpbuff, &tmp);

      parse = ajStrNew();
      gcsi  = ajStrNew();
      sa    = ajStrNew();
      dist  = ajStrNew();
      z     = ajStrNew();
      p     = ajStrNew();

      ajStrRemoveLastNewline(&tmp);

      handle = ajStrTokenNewC(tmp, ",");

      while (ajStrTokenNextParse(handle, &parse))
        {
          if (ajStrIsFloat(parse))
            {
              if(!ajStrGetLen(gcsi))
                ajStrAssignS(&gcsi, parse);
              else if(!ajStrGetLen(sa))
                ajStrAssignS(&sa, parse);
              else if(!ajStrGetLen(dist))
                ajStrAssignS(&dist, parse);
              else if(!ajStrGetLen(z))
                ajStrAssignS(&z, parse);
              else if(!ajStrGetLen(p))
                ajStrAssignS(&p, parse);
            }
        }

      tmp = ajFmtStr("Sequence: %S GCSI: %S SA: %S DIST: %S",
                     seqid, gcsi, sa, dist);

      if(pval)
        tmp = ajFmtStr("%S Z: %S P: %S", tmp, z, p);

      ajFmtPrintF(outf, "%S\n", tmp);

      ajStrDel(&tmp);
      ajStrDel(&parse);
      ajStrDel(&gcsi);
      ajStrDel(&sa);
      ajStrDel(&dist);
      ajStrDel(&z);
      ajStrDel(&p);

      ajStrDel(&inseq);
    }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
