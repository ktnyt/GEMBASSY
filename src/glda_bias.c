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
  AjPStr    seqid = NULL;
  ajint	    coefficients = 0;
  AjPStr    variable = NULL;
  AjBool    accid = ajFalse;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall       = ajAcdGetSeqall("sequence");
  coefficients = ajAcdGetInt("coefficients");
  variable     = ajAcdGetString("variable");
  accid        = ajAcdGetBoolean("accid");
  outf         = ajAcdGetOutfile("outfile");

  params.coefficients = coefficients;
  params.variable = ajCharNewS(variable);

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      if(!ajStrGetLen(seqid))
        ajStrAssignS(&seqid, ajSeqGetNameS(seq));

      if(!ajStrGetLen(seqid))
        {
          ajFmtError("No header information\n");
          embExitBad();
        }

      if(accid || !gFormatGenbank(seq, &inseq))
        {
          if(!accid)
            ajFmtError("Sequence does not have features\n"
                       "Proceeding with sequence accession ID\n");

          if(!gValID(seqid))
            {
              ajFmtError("Invalid accession ID, exiting\n");
              embExitBad();
            }

          ajStrAssignS(&inseq, seqid);
        }

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
          ajFmtPrintF(outf, "Sequence: %S LDA bias: %s\n", seqid, result);
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
