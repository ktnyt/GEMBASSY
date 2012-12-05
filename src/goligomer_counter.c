#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int
main(int argc, char *argv[])
{
  embInitPV("goligomer_counter", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__oligomer_USCOREcounterInputParams params;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  AjPStr	  oligomer = NULL;
  ajint		  window = 0;
  AjPStr	  accid = NULL;
  char           *result;

  AjBool	  show = 0;
  AjPFile	  outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  oligomer = ajAcdGetString("oligomer");
  window = ajAcdGetInt("window");
  accid = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

  params.window = window;

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    soap.send_timeout = 0;
    soap.recv_timeout = 0;

    inseq = NULL;

    ajStrAppendC(&inseq, ">");
    ajStrAppendS(&inseq, ajSeqGetNameS(seq));
    ajStrAppendC(&inseq, "\n");
    ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

    if (!ajStrGetLen(accid))
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    else
      ajStrAssignS(&inseq, accid);

    char           *in0;
    char           *in1;
    in0 = ajCharNewS(inseq);
    in1 = ajCharNewS(oligomer);

    if (soap_call_ns1__oligomer_USCOREcounter(
					      &soap, NULL, NULL,
					      in0, in1, &params, &result
					      ) == SOAP_OK) {
      if (show)
	ajFmtPrint("Sequence: %S Oligomer: %S Count: %S\n",
		   accid, oligomer, ajStrNewC(result));
      else
	ajFmtPrintF(outf, "Sequence: %S Oligomer: %S Count: %S\n",
		    accid, oligomer, ajStrNewC(result));
    } else {
      soap_print_fault(&soap, stderr);
    }

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  if (outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);

  embExit();
  return 0;
}
