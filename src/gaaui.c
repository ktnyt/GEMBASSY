/******************************************************************************
** @source gaaui
**
** Calculates various indeces of amino acid usage
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





/* @prog gaaui ****************************************************************
**
** Calculates various indeces of amino acid usage
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gaaui", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__aauiInputParams params;

  AjPSeqall  seqall;
  AjPSeq     seq;
  AjPStr     inseq = NULL;
  AjPStr     id    = NULL;
  AjPStr     accid = NULL;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  id     = ajAcdGetString("id");
  accid  = ajAcdGetString("accid");
  outf   = ajAcdGetOutfile("outfile");

  params.id = ajCharNewS(id);

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

      if(soap_call_ns1__aaui(&soap,
			      NULL,
			      NULL,
			      in0,
			      &params,
			      &result
                             ) == SOAP_OK)
	{
	  ajFmtPrintF(outf, "Sequence: %S\n", accid);
	  if(!gFileOutURLC(result, &outf))
	    {
	      ajFmtError("File downloading error\n");
	      embExitBad();
	    }
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
  ajStrDel(&accid);

  AJFREE(params.id);

  ajStrDel(&id);

  embExit();

  return 0;
}
