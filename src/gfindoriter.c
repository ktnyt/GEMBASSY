/******************************************************************************
** @source gfindoriter
**
** Predict the replication origin and terminus in bacterial genomes
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




/* @prog gfindoriter ********************************************************
**
** Predict the replication origin and terminus in bacterial genomes
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gfindoriter", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;

  ajint	 window  = 0;
  AjBool purine  = 0;
  AjBool keto    = 0;
  ajint	 lowpass = 0;

  AjPStr ori = NULL;
  AjPStr ter = NULL;

  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  AjPStr      tmpname = NULL;
  AjPSeqout   tmpout  = NULL;
  AjPFilebuff tmpbuff = NULL;

  AjPStr tmp  = NULL;
  AjPStr line = NULL;

  AjPRegexp regex;

  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  window = ajAcdGetInt("window");
  lowpass = ajAcdGetInt("lowpass");
  purine = ajAcdGetBoolean("purine");
  keto   = ajAcdGetBoolean("keto");
  outf   = ajAcdGetOutfile("outfile");

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

      ajFmtPrintS(&url, "http://%S/%S/find_ori_ter/window=%d/lowpass=%d/"
                  "purine=%d/keto=%d/", base, restid, window, lowpass,
                  purine, keto);

      if(!gFilebuffURLS(url, &tmpbuff))
        {
          ajDie("Failed to download result from:\n%S\n", url);
        }

      regex = ajRegCompC("([0-9]+)$");

      while(ajBuffreadLine(tmpbuff, &line))
        {
          if(ajRegExec(regex, line))
            {
              if(ajRegSubI(regex, 0, &tmp))
                {
                  if(ajStrGetLen(ori) == 0)
                    {
                      ajStrAssignS(&ori, tmp);
                    }
                  else if(ajStrGetLen(ter) == 0)
                    {
                      ajStrAssignS(&ter, tmp);
                    }
                }
            }
        }

      ajFmtPrintF(outf, "Sequence: %S Origin: %S Terminus: %S\n",
                  seqid, ori, ter);

      ajStrDel(&tmp);
      ajStrDel(&ori);
      ajStrDel(&ter);

      ajStrDel(&url);
      ajStrDel(&restid);
    }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
