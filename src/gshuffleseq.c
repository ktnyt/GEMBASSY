/******************************************************************************
** @source gshuffleseq
**
** Create randomized sequence with conserved k-mer composition
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




/* @prog gshuffleseq **********************************************************
**
** Create randomized sequence with conserved k-mer composition
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gshuffleseq", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__shuffleseqInputParams params;

  AjPSeqall seqall;
  AjPSeqout seqout;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  ajint	    k = 0;

  char *in0;
  char *result;

  seqall = ajAcdGetSeqall("sequence");
  k      = ajAcdGetInt("k");
  seqout = ajAcdGetSeqout("outseq");

  params.k = k;

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__shuffleseq(
				  &soap,
                                   NULL,
                                   NULL,
				   in0,
                                  &params,
                                  &result
				  ) == SOAP_OK)
        {
          ajCharFmtUpper(result);
          ajSeqAssignSeqC(seq, result);
          ajSeqoutWriteSeq(seqout, seq);
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

  ajSeqoutClose(seqout);
  ajSeqoutDel(&seqout);
  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  embExit();

  return 0;
}
