#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"

int main(int argc, char *argv[])
{
  embInitPV("glda_bias", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__lda_USCOREbiasInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  ajint	    coefficients = 0;
  AjPStr    variable = NULL;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall       = ajAcdGetSeqall("sequence");
  coefficients = ajAcdGetInt("coefficients");
  variable     = ajAcdGetString("variable");
  accid        = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

  params.coefficients = coefficients;
  params.variable = ajCharNewS(variable);

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

      if(soap_call_ns1__lda_USCOREbias(
				      &soap,
                                       NULL,
                                       NULL,
                                       in0,
                                      &params,
                                      &result
				      ) == SOAP_OK)
        {
          if(show)
            ajFmtPrint("Sequence: %S LDA bias: %s\n", accid, result);
          else
            ajFmtPrintF(outf, "Sequence: %S LDA bias: %s\n", accid, result);
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
