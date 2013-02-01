#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"

int main(int argc, char *argv[])
{
  embInitPV("gpalindrome", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__palindromeInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  ajint	    shortest = 0;
  ajint	    loop = 0;
  AjBool    gtmatch = 0;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall   = ajAcdGetSeqall("sequence");
  shortest = ajAcdGetInt("shortest");
  loop     = ajAcdGetInt("loop");
  gtmatch  = ajAcdGetBoolean("gtmatch");
  accid    = ajAcdGetString("accid");
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

      ajStrAssignS(&accid, ajSeqGetAccS(seq));

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

  embExit();

  return 0;
}
