/******************************************************************************
** @source gdistincc
**
** Calculates distance betwwen two loci in cirular chromosomes
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.3
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
** @modified 2015/2/7   Support sequences without features
** @modified 2015/2/7   Remove negative distance bug
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





/* @prog gdistincc **********************************************************
**
** Calculates distance betwwen two loci in cirular chromosomes
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gdistincc", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq;

  AjBool accid = ajFalse;
  AjPStr seqid  = NULL;
  AjPStr restid = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  ajint first;
  ajint second;

  AjPFile     outfile = NULL;
  AjPFile     tmpfile = NULL;
  AjPStr      tmpname = NULL;
  AjPStr      fstname = NULL;
  AjPFilebuff tmp     = NULL;
  AjPStr      line    = NULL;
  AjPSeqout   tmpout  = NULL;

  seqall  = ajAcdGetSeqall("sequence");
  first   = ajAcdGetInt("first");
  second  = ajAcdGetInt("second");
  accid   = ajAcdGetBoolean("accid");
  outfile = ajAcdGetOutfile("outfile");

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

      if(second >= 0)
        {
          ajFmtPrintS(&url, "http://%S/%S/dist_in_cc/%d/%d",
                      base, restid, first, second);
        }
      else
        {
          ajFmtPrintS(&url, "http://%S/%S/dist_in_cc/%d",
                      base, restid, first);
        }

      if(!gFilebuffURLS(url, &tmp))
        {
          ajDie("Failed to download result from:\n%S\n", url);
        }

      ajBuffreadLine(tmp, &line);

      ajStrRemoveSetC(&line, "\n");

      if(second >= 0)
        {
          ajFmtPrintF(outfile, "Sequence: %S Position1: %d Position2: %d "
                      "Distance %S\n", seqid, first, line);
        }
      else
        {
          ajFmtPrintF(outfile, "Sequence: %S Position1: %d Distance %S\n",
                      seqid, first, line);
        }

      ajStrDel(&url);
      ajStrDel(&restid);
      ajStrDel(&seqid);
    }

  ajFileClose(&outfile);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&base);

  embExit();

  return 0;
}
