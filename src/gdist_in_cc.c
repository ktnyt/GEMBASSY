/******************************************************************************
** @source gdist_in_cc
**
** Calculates distance betwwen two loci in cirular chromosomes
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
#include "../include/gpost.h"




/* @prog gdist_in_cc **********************************************************
**
** Calculates distance betwwen two loci in cirular chromosomes
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gdist_in_cc", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;

  AjBool accid = ajFalse;
  AjPStr seqid  = NULL;
  AjPStr restid = NULL;

  ajint first;
  ajint second;

  char *in0;
  char *result;

  AjPFile outfile = NULL;
  AjPFile tmpfile = NULL;
  AjPStr  tmpname = NULL;
  AjPFilebuff tmp = NULL;
  AjPStr     line = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  seqall  = ajAcdGetSeqall("sequence");
  first   = ajAcdGetInt("first");
  second  = ajAcdGetInt("second");
  accid   = ajAcdGetBoolean("accid");
  outfile = ajAcdGetOutfile("outfile");

  base = ajStrNewC("rest.g-language.org");

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = NULL;

      if(!accid)
        {
          gAssignUniqueName(&tmpname);

          tmpfile = ajFileNewOutNameS(tmpname);

          ajStrAppendS(&inseq, ajSeqGetSeqS(seq));
          ajStrAssignS(&restid, ajSeqGetAccS(seq));

          ajFmtPrintF(tmpfile, ">%S\n%S\n", restid, inseq);

          ajFileClose(&tmpfile);

          gFilePostCS("http://rest.g-language.org/upload/upl.pl",
                      tmpname, &restid);

          ajSysFileUnlinkS(tmpname);
        }
      else
        {
          ajStrAssignS(&restid, ajSeqGetAccS(seq));
        }

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

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
          ajFmtError("Failed to download result from:\n%S\n", url);
          embExitBad();
        }

      ajBuffreadLine(tmp, &line);

      ajStrRemoveSetC(&line, "\n");

      if(second >= 0)
        {
          ajFmtPrintF(outfile, "Sequence: %S Position1: %d Position2: %d "
                      "Distance %d\n", seqid, first, line);
        }
      else
        {
          ajFmtPrintF(outfile, "Sequence: %S Position1: %d Distance %d\n",
                      seqid, first, line);
        }

      ajStrDel(&url);
      ajStrDel(&inseq);
    }

  ajFileClose(&outfile);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
