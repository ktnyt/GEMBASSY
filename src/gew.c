#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"

int main(int argc, char *argv[])
{
  embInitPV("gew", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__EwInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  AjBool    translate = 0;
  AjPStr    delkey = NULL;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall    = ajAcdGetSeqall("sequence");
  translate = ajAcdGetBoolean("translate");
  delkey    = ajAcdGetString("delkey");
  accid     = ajAcdGetString("accid");
  outf      = ajAcdGetOutfile("outfile");

  params.translate = 0;
  params.del_USCOREkey = ajCharNewS(delkey);
  params.output = "f";

  if(translate)
    params.translate = 1;

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

      if (soap_call_ns1__Ew(
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

  AJFREE(params.del_USCOREkey);

  ajStrDel(&delkey);

  embExit();

  return 0;
}
