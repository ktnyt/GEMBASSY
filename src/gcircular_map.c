/******************************************************************************
** @source gcircular_map
**
** Draws circular map of the genome
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
#include "../include/ghttp.h"
#include "../include/display_png.h"




/* @prog gcircular_map ********************************************************
**
** Draws circular map of the genome
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gcircular_map", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__circular_USCOREmapInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    accid    = NULL;
  AjPFile   outf     = NULL;
  AjPStr    filename = NULL;
  AjPStr    outfname = NULL;
  AjPStr    tempname = NULL;

  char *in0;
  char *result;

  seqall   = ajAcdGetSeqall("sequence");
  filename = ajAcdGetString("filename");
  accid    = ajAcdGetString("accid");

  params.gmap = 0;

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      soap.send_timeout = 0;
      soap.recv_timeout = 0;

      inseq = NULL;

      if(!gFormatGenbank(seq, &inseq) && !ajStrGetLen(accid))
	{
	  ajFmtError("Sequence does not have features\n");
	  ajFmtError("Proceeding with sequence accession ID\n");
	  ajStrAssignS(&accid, ajSeqGetAccS(seq));

          if(!ajStrGetLen(accid))
            {
              ajStrAssignS(&accid, ajSeqGetNameS(seq));

              if(!ajStrGetLen(accid))
                {
                  ajFmtError("No header information\n");
                  embExitBad();
                }
            }
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

      if(soap_call_ns1__circular_USCOREmap(
	                                  &soap,
					   NULL,
					   NULL,
					   in0,
					  &params,
					  &result
                                          ) == SOAP_OK)
	{
	  outfname = ajStrNew();
	  tempname = ajStrNew();

	  ajStrFromLong(&tempname, ajSeqallGetCount(seqall));
	  ajStrInsertC(&tempname, 0, ".");
	  ajStrAppendC(&tempname, ".svg");

	  if(!ajStrExchangeCS(&outfname, ".svg", tempname))
	    {
	      ajStrAppendS(&outfname, tempname);
	    }

	  outf = ajFileNewOutNameS(outfname);

	  ajStrDel(&outfname);
	  ajStrDel(&tempname);

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

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  ajStrDel(&filename);

  embExit();

  return 0;
}
