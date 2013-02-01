/******************************************************************************
** @source genret
**
** Retrieves various gene related infomration from genome flatfile
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




/* @prog genret ***************************************************************
**
** Retrieves various gene related infomration from genome flatfile
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("genret", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;

  AjPSeq seq;
  AjPStr inseq  = NULL;
  AjPStr method = NULL;
  AjPStr cds    = NULL;
  AjPStr option = 0;
  AjBool trans  = 0;
  AjPStr accid  = NULL;

  AjPStr content = NULL;
  ajuint width   = 80;

  AjPStr  filename = NULL;
  AjPFile outfile  = NULL;
  AjPFile tmpfile  = NULL;
  AjPFilebuff buff = NULL;

  AjPStr url  = NULL;
  AjPStr line = NULL;

  seqall  = ajAcdGetSeqall("sequence");
  method  = ajAcdGetString("method");
  cds     = ajAcdGetString("cds");
  option  = ajAcdGetString("option");
  trans   = ajAcdGetBoolean("translate");
  accid   = ajAcdGetString("accid");
  outfile = ajAcdGetOutfile("outfile");


  filename = ajStrNew();

  gAssignUniqueName(&filename);

  while(ajSeqallNext(seqall, &seq))
    {

      inseq   = ajStrNew();
      url     = ajStrNew();
      line    = ajStrNew();
      content = ajStrNew();

      if(!gFormatGenbank(seq, &inseq) || ajStrGetLen(accid))
	{
          if(!ajStrGetLen(accid))
            {
              ajFmtError("Sequence does not have features\n");
              ajFmtError("Proceeding with sequence accession ID\n");
              ajStrAssignS(&accid, ajSeqGetAccS(seq));
            }

          if(!gValID(accid))
            {
              ajFmtError("Invalid accession ID, exiting\n");
              embExitBad();
            }
        }
      else
        {
          tmpfile = ajFileNewOutNameS(filename);

          ajFmtPrintF(tmpfile, "%S", inseq);

          ajFileClose(&tmpfile);

          gFilePostCS("http://rest.g-language.org/upload/upl.pl",
                      filename, &accid);

          ajSysFileUnlinkS(filename);
        }

      ajFmtPrintS(&url, "http://rest.g-language.org/%S/%S/%S/%S",
                  accid, method, cds, option);

      gFilebuffURLS(url, &buff);

      while(ajBuffreadLine(buff, &line))
        {
          ajStrAppendS(&content, line);
        }

      ajStrFmtWrap(&content, 60);

      ajFmtPrintF(outfile, "%S", content);

      ajStrDel(&inseq);
      ajStrDel(&url);
      ajStrDel(&line);
      ajStrDel(&content);
    }


  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  embExit();
}
