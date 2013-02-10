#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"

int main(int argc, char *argv[])
{
  embInitPV("gsignature", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__signatureInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    seqid      = NULL;
  ajint	    wordlength = 0;
  AjBool    bothstrand = 0;
  AjBool    oe         = 0;
  AjBool    accid      = ajFalse;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  wordlength = ajAcdGetInt("wordlength");
  bothstrand = ajAcdGetBoolean("bothstrand");
  oe     = ajAcdGetBoolean("oe");
  accid  = ajAcdGetBoolean("accid");
  outf   = ajAcdGetOutfile("outfile");

  params.wordlength = wordlength;
  params.bothstrand = 0;
  params.oe = 0;

  if(bothstrand)
    params.bothstrand = 1;
  if(oe)
    params.oe = 1;

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

      if(soap_call_ns1__signature(
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

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
