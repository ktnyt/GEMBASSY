/******************************************************************************
** @source gbasezvalue
**
** Extracts conserved oligomers per position using Z-score
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.1   Revision 1
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
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




/* @prog gbasezvalue ********************************************************
**
** Extracts conserved oligomers per position using Z-score
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gbasezvalue", argc, argv, "GEMBASSY", "1.0.1");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;

  AjPStr position   = NULL;
  ajint	 limit      = 0;
  ajint  PatLen     = 0;
  ajint  upstream   = 0;
  ajint  downstream = 0;

  AjBool accid  = ajFalse;
  AjPStr restid = NULL;
  AjPStr seqid  = NULL;

  AjPStr base = NULL;
  AjPStr url  = NULL;

  AjPFile tmpfile = NULL;
  AjPStr  tmpname = NULL;

  AjPFile outf = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  position   = ajAcdGetSelectSingle("position");
  limit      = ajAcdGetInt("limit");
  PatLen     = ajAcdGetInt("patlen");
  upstream   = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  accid      = ajAcdGetBoolean("accid");
  outf       = ajAcdGetOutfile("outfile");

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
              ajDie("Sequence does not have features\n"
                    "Proceeding with sequence accession ID\n");
              accid = ajTrue;
            }
        }

      if(accid)
        {
          ajStrAssignS(&restid, ajSeqGetAccS(seq));
          if(!ajStrGetLen(restid))
            {
              ajStrAssignS(&restid, ajSeqGetNameS(seq));
            }
          if(!ajStrGetLen(restid))
            {
              ajDie("No valid header information\n");
            }
        }

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      url = ajStrNew();

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      url = ajStrNew();

      ajFmtPrintS(&url, "http://%S/%S/base_z_value/position=%S/PatLen=%d"
                  "upstream=%d/downstream=%d/limit=%d/output=f/tag=gene",
                  base, restid, position, PatLen, upstream, downstream, limit);

      ajFmtPrintF(outf, "Sequence: %S\n", seqid);
      if(!gFileOutURLS(url, &outf))
        {
          ajDie("Failed to download result from:\n%S\n", url);
        }

      ajStrDel(&url);
      ajStrDel(&inseq);
    }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  ajStrDel(&position);

  embExit();

  return 0;
}