/******************************************************************************
** @source greporiter
**
** Get the positions of replication origin and terminus
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




/* @prog greporiter *********************************************************
**
** Get the positions of replication origin and terminus
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("greporiter", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;

  AjBool accid  = ajFalse;
  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  AjBool oriloc = 0;
  AjBool gcskew = 0;
  AjBool dbonly = 0;
  ajint	 difthreshold = 0;

  AjPFile outf = NULL;

  AjPFile     tmpfile = NULL;
  AjPStr      tmpname = NULL;
  AjPStr      fstname = NULL;
  AjPFilebuff tmp     = NULL;
  AjPStr      line    = NULL;
  AjPSeqout   tmpout  = NULL;

  AjPRegexp regex;

  AjPStr    ori    = NULL;
  AjPStr    ter    = NULL;

  seqall = ajAcdGetSeqall("sequence");
  difthreshold = ajAcdGetInt("difthreshold");
  oriloc = ajAcdGetBoolean("oriloc");
  gcskew = ajAcdGetBoolean("gcskew");
  dbonly = ajAcdGetBoolean("dbonly");
  accid  = ajAcdGetBoolean("accid");
  outf   = ajAcdGetOutfile("outfile");

  base = ajStrNewC("rest.g-language.org");

  gAssignUniqueName(&tmpname);
  gAssignUniqueName(&fstname);
  ajStrAppendC(&fstname, ".fasta");

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = ajStrNew();

      tmpout = ajSeqoutNew();

      if(!accid)
        {
          if(gFormatGenbank(seq, &inseq))
            {
              tmpfile = ajFileNewOutNameS(tmpname);
              if(!tmpfile)
                {
                  ajDie("Output file (%S) open error\n", tmpname);
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
              if(!ajSeqoutOpenFilename(tmpout, fstname))
                {
                  embExitBad();
                }

              ajSeqoutSetFormatS(tmpout,ajStrNewC("fasta"));
              ajSeqoutWriteSeq(tmpout, seq);
              ajSeqoutClose(tmpout);
              ajSeqoutDel(&tmpout);
              ajFmtPrintS(&url, "http://%S/upload/upl.pl", base);
              gFilePostSS(url, fstname, &restid);
              ajStrDel(&url);
              ajSysFileUnlinkS(fstname);
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

      ajFmtPrintS(&url, "http://%S/%S/rep_ori_ter/oriloc=%d/gcskew=%d/"
                  "difthreshold=%d/dbonly=%d/",  base, restid, oriloc, gcskew,
                  difthreshold, dbonly);

      if(!gFilebuffURLS(url, &tmp))
        {
          ajDie("Failed to download result from:\n%S\n", url);
        }

      ajBuffreadLine(tmp, &line);

      regex = ajRegCompC("([0-9]+),([0-9]+)");

      if(ajRegExec(regex, line)) {
        if(ajRegSubI(regex, 1, &ori), ajRegSubI(regex, 2, &ter)) {
          ajFmtPrint("%S Origin: %S Terminus %S\n", seqid, ori, ter);
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
