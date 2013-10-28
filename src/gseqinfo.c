/******************************************************************************
** @source gseqinfo
**
** Prints out basic nucleotide sequence statistics
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.1   Revision 1
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
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




/* @prog gseqinfo *************************************************************
**
** Prints out basic nucleotide sequence statistics
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gseqinfo", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  AjPStr    seqid  = NULL;
  AjPStr    tmp    = NULL;
  AjPStr    parse  = NULL;
  AjPStr    numA   = NULL;
  AjPStr    numT   = NULL;
  AjPStr    numG   = NULL;
  AjPStr    numC   = NULL;
  AjPStrTok handle = NULL;

  ajint n;

  char *in0;
  char *result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");

  outf = ajAcdGetOutfile("outfile");

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

      if(soap_call_ns1__seqinfo(
			       &soap,
                                NULL,
                                NULL,
			        in0,
                               &result
			       ) == SOAP_OK)
        {
          tmp = ajStrNewC(result);

          ajStrExchangeCC(&tmp, "<", "\n");
          ajStrExchangeCC(&tmp, ">", "\n");

          handle = ajStrTokenNewC(tmp, "\n");

          while(ajStrTokenNextParse(handle, &parse))
            {
              if(ajStrIsInt(parse))
                if(!numA)
                  numA = ajStrNewS(parse);
                else if(!numT)
                  numT = ajStrNewS(parse);
                else if(!numG)
                  numG = ajStrNewS(parse);
                else if(!numC)
                  numC = ajStrNewS(parse);
            }
          if(show)
            ajFmtPrint("Sequence: %S A: %S T: %S G: %S C: %S\n",
                       seqid, numA, numT, numG, numC);
          else
            ajFmtPrintF(outf, "Sequence: %S A: %S T: %S G: %S C: %S\n",
                        seqid, numA, numT, numG, numC);
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

  embExit();

  return 0;
}
