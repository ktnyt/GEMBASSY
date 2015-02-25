/******************************************************************************
** @source gpalindrome
**
** Searches palindrome sequences
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.3
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
** @modified 2015/2/7   Refactor
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
#include "glibs.h"




/* @prog gpalindrome **********************************************************
**
** Searches palindrome sequences
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gpalindrome", argc, argv, "GEMBASSY", "1.0.3");

  struct soap soap;
  struct ns1__palindromeInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  AjPStr    seqid = NULL;
  ajint	    shortest = 0;
  ajint	    loop = 0;
  AjBool    gtmatch = 0;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall   = ajAcdGetSeqall("sequence");
  shortest = ajAcdGetInt("shortest");
  loop     = ajAcdGetInt("loop");
  gtmatch  = ajAcdGetBoolean("gtmatch");
  outf     = ajAcdGetOutfile("outfile");

  params.shortest = shortest;
  params.loop     = loop;
  params.gtmatch  = gtmatch;
  params.output   = "f";

  while(ajSeqallNext(seqall, &seq))
    {
      soap_init(&soap);

      inseq = NULL;

      ajStrAppendC(&inseq, ">");
      ajStrAppendS(&inseq, ajSeqGetNameS(seq));
      ajStrAppendC(&inseq, "\n");
      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__palindrome(
				  &soap,
                                   NULL,
                                   NULL,
				   in0,
                                  &params,
                                  &result
				  ) == SOAP_OK)
        {
          ajFmtPrintF(outf, "Sequence: %S\n", seqid);

          if(!gFileOutURLC(result, &outf))
            {
              ajDie("File downloading error from:\n%s\n", result);
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

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
