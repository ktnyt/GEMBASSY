#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int main(int argc, char *argv[])
{
  embInitPV("goligomer_search", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__oligomer_USCOREsearchInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    oligomer = NULL;
  AjPStr    return_  = NULL;
  AjPStr    accid    = NULL;
  AjPStr    tmp      = NULL;
  AjPStr    parse    = NULL;
  AjPStrTok handle   = NULL;

  char *in0;
  char *in1;
  char *result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall   = ajAcdGetSeqall("sequence");
  oligomer = ajAcdGetString("oligomer");
  return_  = ajAcdGetString("return");
  accid    = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");

  if(!show)
    outf = ajAcdGetOutfile("outfile");

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

      if(soap_call_ns1__oligomer_USCOREcounter(
					      &soap,
                                               NULL,
                                               NULL,
					       in0,
                                               in1,
                                              &params,
                                              &result
					      ) == SOAP_OK)
        {
          tmp = ajStrNew();

          ajStrAssignC(tmp, result);

          ajStrExchangeCC(&tmp, "<", "\n");
          ajStrExchangeCC(&tmp, ">", "\n");

          handle = ajStrTokenNewC(tmp, "\n");

          if(show)
            ajFmtPrint("Sequence: %S Oligomer: %S\n", accid, oligomer);
          else
            ajFmtPrintF(outf, "Sequence: %S Oligomer: %S\n", accid, oligomer);

          while(ajStrTokenNextParse(&handle, &parse))
            {
              if(ajStrIsInt(parse))
                if(show)
                  ajFmtPrint("%s\n", parse);
                else
                  ajFmtPrintF(outf, "%s\n", parse);
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

      ajStrDel(inseq);
    }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&oligomer);

  AJFREE(params.return_);

  ajStrDel(&result);

  embExit();

  return 0;
}
