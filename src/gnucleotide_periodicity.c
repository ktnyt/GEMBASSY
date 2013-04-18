/******************************************************************************
** @source gnucleotide_periodicity
**
** Checks the periodicity of certain oligonucleotides
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
#include "../include/gfile.h"
#include "../include/gplot.h"
#include "../include/gpost.h"




/* @prog gnucleotide_periodicity *********************************************
**
** Checks the periodicity of certain oligonucleotides
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gnucleotide_periodicity", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;

  ajint  window     = 0;
  AjPStr nucleotide = NULL;

  AjBool accid  = ajFalse;
  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  AjPFile outf     = NULL;
  AjPFile tempfile = NULL;
  AjPStr  filename = NULL;
  AjPStr  outfname = NULL;
  AjPStr  tempname = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  nucleotide = ajAcdGetString("nucleotide");
  filename   = ajAcdGetString("goutfile");

  base = ajStrNewC("rest.g-language.org");

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = NULL;

      if(!accid)
        {
          if(gFormatGenbank(seq, &inseq))
            {
              gAssignUniqueName(&tempname);

              tempfile = ajFileNewOutNameS(tempname);

              if(!tempfile)
                {
                  ajFmtError("Output file (%S) open error\n", tempname);
                  embExitBad();
                }

              ajFmtPrintF(tempfile, "%S", inseq);
              ajFileClose(&tempfile);
              ajFmtPrintS(&url, "http://%S/upload/upl.pl", base);
              gFilePostSS(url, tempname, &restid);
              ajStrDel(&url);
              ajSysFileUnlinkS(tempname);
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
          ajStrAssignS(&seqid, ajSeqGetAccS(seq));

          if(!ajStrGetLen(seqid))
            {
              ajStrAssignS(&seqid, ajSeqGetNameS(seq));
            }

          if(!ajStrGetLen(seqid))
            {
              ajFmtError("No valid header information\n");
              embExitBad();
            }

          ajStrAssignS(&restid, seqid);
        }

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      outfname = ajStrNew();
      tempname = ajStrNew();

      ajStrAssignS(&outfname, filename);

      ajStrFromLong(&tempname, ajSeqallGetCount(seqall));
      ajStrInsertC(&tempname, 0, ".");
      ajStrAppendC(&tempname, ".png");

      if(!ajStrExchangeCS(&outfname, ".png", tempname))
        {
          ajStrAppendS(&outfname, tempname);
        }

      outf = ajFileNewOutNameS(outfname);

      if(!outf)
        {
          ajFmtError("File open error\n");
          embExitBad();
        }

      url = ajStrNew();

      ajFmtPrintS(&url, "http://%S/%S/nucleotide_periodicity/output=g/gmap=0/"
                  "nucleotide=%S/window=%d", base, restid, nucleotide, window);

      if(!gHttpGetBinS(url, &outf))
        {
          ajFmtError("File downloading error from:\n%S\n", url);
          embExitBad();
        }
      else
        {
          ajFmtError("Created %S\n", outfname);
        }

      ajStrDel(&outfname);
      ajStrDel(&tempname);
      ajStrDel(&url);
      ajStrDel(&inseq);
    }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&filename);

  embExit();

  return 0;
}
