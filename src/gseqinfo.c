#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"

int main(int argc, char *argv[])
{
  embInitPV("gseqinfo", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  AjPStr    accid  = NULL;
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
  accid  = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");

  if(!show)
    outf = ajAcdGetOutfile("outfile");

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

          while(ajStrTokenNextParse(&handle, &parse))
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
                       accid, numA, numT, numG, numC);
          else
            ajFmtPrintF(outf, "Sequence: %S A: %S T: %S G: %S C: %S\n",
                        accid, numA, numT, numG, numC);
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

  embExit();

  return 0;
}
