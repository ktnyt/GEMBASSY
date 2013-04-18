/******************************************************************************
** @source gkmer_table
**
** Create an image showing all k-mer abundance within a sequence
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




int main(int argc, char *argv[])
{
  embInitPV("gkmer_table", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__kmer_USCOREtableInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    seqid    = NULL;
  ajint     k        = 0;
  AjBool    accid    = ajFalse;
  AjPFile   outf     = NULL;
  AjPStr    filename = NULL;
  AjPStr    outfname = NULL;
  AjPStr    tempname = NULL;

  char *in0;
  char *result;

  seqall   = ajAcdGetSeqall("sequence");
  k        = ajAcdGetInt("k");
  filename = ajAcdGetString("goutfile");

  params.k = k;

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      ajStrAppendC(&inseq, ">");
      ajStrAppendS(&inseq, ajSeqGetAccS(seq));
      ajStrAppendC(&inseq, "\n");
      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__kmer_USCOREtable(
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

          ajStrAssignS(&outfname, filename);

	  ajStrFromLong(&tempname, ajSeqallGetCount(seqall));
	  ajStrInsertC(&tempname, 0, ".");
	  ajStrAppendC(&tempname, ".png");

	  if(!ajStrExchangeCS(&outfname, ".png", tempname))
	    {
	      ajStrAppendS(&outfname, tempname);
	    }

	  outf = ajFileNewOutNameS(outfname);

	  ajStrDel(&outfname);
	  ajStrDel(&tempname);

	  if(!gHttpGetBinC(result, &outf))
	    {
              ajFmtError("File downloading error from:\n%s\n", result);
	      embExitBad();
	    }
          else
            {
              ajFmtError("Created %S\n", outfname);
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
  ajStrDel(&seqid);

  ajStrDel(&filename);

  embExit();

  return 0;
}
