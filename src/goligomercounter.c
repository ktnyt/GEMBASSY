/******************************************************************************
** @source goligomercounter
**
** Counts the number of given oligomers in a sequence
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.3   Revision 1
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




/* @prog goligomercounter ****************************************************
**
** Counts the number of given oligomers in a sequence
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("goligomercounter", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    oligomer = NULL;

  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  ajint window = 0;

  AjPFile outf = NULL;

  AjPStr    tmpname = NULL;
  AjPSeqout tmpout  = NULL;

  AjPFilebuff tmp = NULL;
  AjPStr     line = NULL;

  seqall   = ajAcdGetSeqall("sequence");
  oligomer = ajAcdGetString("oligomer");
  window   = ajAcdGetInt("window");
  outf     = ajAcdGetOutfile("outfile");

  base = ajStrNewC("rest.g-language.org");

  gAssignUniqueName(&tmpname);
  ajStrAppendC(&tmpname, ".fasta");

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = NULL;

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

      ajFmtPrintS(&url, "http://%S/%S/oligomer_counter/%S",
                  base, restid, oligomer);

      if(!gFilebuffURLS(url, &tmp))
        {
          ajDie("Failed to download result from:\n%S\n", url);
        }

      ajBuffreadLine(tmp, &line);

      ajStrRemoveSetC(&line, "\n");

      ajFmtPrintF(outf, "Sequence: %S Oligomer: %S Number: %S\n",
                  seqid, oligomer, line);

      ajStrDel(&url);
      ajStrDel(&restid);
      ajStrDel(&seqid);
      ajStrDel(&inseq);
    }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&base);

  ajStrDel(&oligomer);

  embExit();

  return 0;
}
