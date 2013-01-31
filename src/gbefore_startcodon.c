/******************************************************************************
** @source gbefore_startcodon
**
** Get the upstream sequence of the given CDS
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
#include "../include/ghttp.h"




/* @prog gbefore_startcodon ***************************************************
**
** Get the upstream sequence of the given CDS
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gbefore_startcodon", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  AjPStr    cds   = NULL;
  ajint     length;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  AjPFilebuff buff = NULL;

  seqall = ajAcdGetSeqall("sequence");
  cds    = ajAcdGetString("cds");
  length = ajAcdGetInt("length");
  accid  = ajAcdGetString("accid");

  while(ajSeqallNext(seqall, &seq))
    {

      inseq = NULL;

      if(!gFormatGenbank(seq, &inseq) && !ajStrGetLen(accid))
	{
	  ajFmtError("Sequence does not have features\n");
	  ajFmtError("Proceeding with sequence accession ID\n");
	  ajStrAssignS(&accid, ajSeqGetAccS(seq));
	}

      if(ajStrGetLen(accid))
	{
	  if(!gValID(accid))
	    {
	      ajFmtError("Invalid accession ID, exiting\n");
	      embExitBad();
	      ajStrAssignS(&inseq, accid);
	    }
	}

      const AjPStr filename;

      filename = ajSeqallGetFilename(seqall);

      gFilePostCS("http://localhost/~kotone/dev/upload.cgi",
                 filename, &accid);

      in0 = ajCharNewS(inseq);

    }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);

  embExit();
}
