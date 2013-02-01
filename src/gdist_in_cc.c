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

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"




/* @prog gdist_in_cc **********************************************************
**
** Calculates distance betwwen two loci in cirular chromosomes
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gdist_in_cc", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  ajint	    position1 = 0;
  ajint	    position2 = 0;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall    = ajAcdGetSeqall("sequence");
  position1 = ajAcdGetInt("position");
  position2 = ajAcdGetInt("secondposition");
  accid     = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  if(!show)
    outf = ajAcdGetOutfile("outfile");

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

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
            }
          ajStrAssignS(&inseq, accid);
        }

      if(!ajStrGetLen(accid))
        ajStrAssignS(&accid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__dist_USCOREin_USCOREcc(
					      &soap,
                                               NULL,
                                               NULL,
                                               in0,
                                               position1,
                                               position2,
                                              &result
					      ) == SOAP_OK)
        {
          if(show)
            ajFmtPrint("Sequence: %S Distance: %s\n", accid, result);
          else
            ajFmtPrintF(outf, "Sequence: %S Distance: %s\n", accid, result);
        }
      else
        {
          soap_print_fault(&soap, stderr);
        }

      soap_destroy(&soap);
      soap_end(&soap);
      soap_done(&soap);

      AJFREE(in0);

      ajStrDel(&inseq);
    }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  embExit();
  return 0;
}
