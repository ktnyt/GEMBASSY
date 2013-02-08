/******************************************************************************
** @source gdelta_gcskew
**
** Calculates strand bias of bacterial genome using delta GC skew index
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




/* @prog gdelta_gcskew ********************************************************
**
** Calculates strand bias of bacterial genome using delta GC skew index
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gdelta_gcskew", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__delta_USCOREgcskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  AjPStr    seqid  = NULL;
  AjBool    at     = 0;
  AjBool    purine = 0;
  AjBool    keto   = 0;
  AjBool    accid  = ajFalse;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  at     = ajAcdGetBoolean("at");
  purine = ajAcdGetBoolean("purine");
  keto   = ajAcdGetBoolean("keto");
  accid  = ajAcdGetBoolean("accid");
  outf   = ajAcdGetOutfile("outfile");

  params.at     = 0;
  params.purine = 0;
  params.keto   = 0;

  if(at)
    params.at     = 1;
  if(purine)
    params.purine = 1;
  if (keto)
    params.keto   = 1;

  while(ajSeqallNext(seqall, &seq))
    {
      soap_init(&soap);

      inseq = NULL;

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      if(!ajStrGetLen(seqid))
        ajStrAssignS(&seqid, ajSeqGetNameS(seq));

      if(!ajStrGetLen(seqid))
        {
          ajFmtError("No header information\n");
          embExitBad();
        }

      if(accid || !gFormatGenbank(seq, &inseq))
        {
          if(!accid)
            ajFmtError("Sequence does not have features\n"
                       "Proceeding with sequence accession ID\n");

          if(!gValID(seqid))
            {
              ajFmtError("Invalid accession ID, exiting\n");
              embExitBad();
            }

          ajStrAssignS(&inseq, seqid);
        }

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__delta_USCOREgcskew(
	                                  &soap,
					   NULL,
					   NULL,
					   in0,
					  &params,
					  &result
                                          ) == SOAP_OK)
	{
          ajFmtPrintF(outf, "Sequence: %S delta GC skew: %s\n", seqid, result);
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

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
