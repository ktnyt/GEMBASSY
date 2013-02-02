/******************************************************************************
** @source goligomer_search
**
** Searches oligomers in given sequence
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




/* @prog goligomer_search *****************************************************
**
** Searches oligomers in given sequence
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("goligomer_search", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__oligomer_USCOREsearchInputParams params;
  struct ns1__oligomer_USCOREsearchResponse result;
  struct arrayOut *arrayOut;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    oligomer = NULL;
  AjPStr    return_  = NULL;
  AjPStr    accid    = NULL;
  AjPStr    tmp      = NULL;
  AjPStr    parse    = NULL;
  AjPStrTok handle   = NULL;

  ajint i = 0;

  char *in0;
  char *in1;

  AjPFile outf = NULL;

  seqall   = ajAcdGetSeqall("sequence");
  oligomer = ajAcdGetString("oligomer");
  return_  = ajAcdGetString("return");
  accid    = ajAcdGetString("accid");
  outf     = ajAcdGetOutfile("outfile");

  params.return_ = ajCharNewS(return_);

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      soap.send_timeout = 0;
      soap.recv_timeout = 0;

      inseq = NULL;

      ajStrAppendC(&inseq, ">");
      ajStrAppendS(&inseq, ajSeqGetNameS(seq));
      ajStrAppendC(&inseq, "\n");
      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      ajStrAssignS(&accid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);
      in1 = ajCharNewS(oligomer);

      if(soap_call_ns1__oligomer_USCOREsearch(
					     &soap,
                                              NULL,
                                              NULL,
					      in0,
                                              in1,
                                             &params,
                                             &result
					     ) == SOAP_OK)
        {
          
          ajFmtPrintF(outf, "Sequence: %S Oligomer: %S\n", accid, oligomer);

          arrayOut = result._result;

          while(i < (*arrayOut).__size)
            {
              ajFmtPrintF(outf, "%s\n", (*arrayOut).__ptr[i]);
              ++i;
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
      AJFREE(in1);

      ajStrDel(&inseq);
    }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&oligomer);

  AJFREE(params.return_);

  ajStrDel(&return_);

  embExit();

  return 0;
}
