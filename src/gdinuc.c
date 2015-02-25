/******************************************************************************
** @source gdinuc
**
** Calculates dinucleotide usage
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




/* @prog gdinuc ***************************************************************
**
** Calculates dinucleotide usage
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gdinuc", argc, argv, "GEMBASSY", "1.0.3");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;

  AjBool translate = ajFalse;
  AjPStr position  = NULL;
  AjPStr delkey    = NULL;

  AjBool accid  = ajFalse;
  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  AjPFile tmpfile = NULL;
  AjPStr  tmpname = NULL;

  AjPFile outf = NULL;

  seqall    = ajAcdGetSeqall("sequence");
  translate = ajAcdGetBoolean("translate");
  position  = ajAcdGetListSingle("position");
  delkey    = ajAcdGetString("delkey");
  accid     = ajAcdGetBoolean("accid");
  outf      = ajAcdGetOutfile("outfile");

  if(ajStrMatchCaseC(position, "all"))
    {
      ajStrDel(&position);
      position = ajStrNew();
    }

  base = ajStrNewC("rest.g-language.org");

  gAssignUniqueName(&tmpname);

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = NULL;

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
              ajWarn("Sequence does not have features\n"
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

      ajFmtPrintS(&url, "http://%S/%S/dinuc/translate=%d/position=%S/"
                  "delkey=%S/output=f/tag=gene", base, restid, translate,
                  position, delkey);

      ajFmtPrintF(outf, "Sequence: %S\n", seqid);
      if(!gFileOutURLS(url, &outf))
        {
          ajDie("Failed to download result from:\n%S\n", url);
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

  ajStrDel(&position);
  ajStrDel(&delkey);

  embExit();

  return 0;
}
