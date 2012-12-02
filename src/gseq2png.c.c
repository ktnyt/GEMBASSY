#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"
#include "../include/display_png.h"

int 
main(int argc, char *argv[])
{
  embInitPV("gseq2png", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__seq2pngInputParams params;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  ajint		  width = 0;
  ajint		  window = 0;
  AjPStr	  accid = NULL;
  AjPStr	  filename = NULL;
  AjBool	  show = 0;
  char           *result;

  seqall = ajAcdGetSeqall("sequence");
  window = ajAcdGetInt("window");
  width = ajAcdGetInt("width");
  filename = ajAcdGetString("filename");
  show = ajAcdGetToggle("show");
  accid = ajAcdGetString("accid");

  params.window = window;
  params.width = width;
  params.output = "g";

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    inseq = NULL;

    ajStrAppendC(&inseq, ">");
    ajStrAppendS(&inseq, ajSeqGetNameS(seq));
    ajStrAppendC(&inseq, "\n");
    ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

    if (!ajStrGetLen(accid))
      ajStrAssignS(&accid, ajSeqGetAccS(seq));

    char           *in0;
    in0 = ajCharNewS(inseq);

    if (soap_call_ns1__seq2png(
			       &soap, NULL, NULL,
			       in0, &params, &result
			       ) == SOAP_OK) {
      AjPStr	      tmp = ajStrNew();
      ajStrFromLong(&tmp, ajSeqallGetCount(seqall));
      ajStrInsertC(&tmp, 0, ".");
      ajStrAppendC(&tmp, ".png");
      if (!ajStrExchangeCS(&filename, ".png", tmp)) {
	ajStrAppendC(&filename, ".");
      }
      if (get_file(result, ajCharNewS(filename)) == 0) {
	if (show)
	  if (display_png(ajCharNewS(filename), argv[0], ajCharNewS(accid)))
	    fprintf(stderr, "Error in X11 displaying\n");
      } else {
	fprintf(stderr, "Retrieval unsuccessful\n");
      }
    } else {
      soap_print_fault(&soap, stderr);
    }

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);

  embExit();
  return 0;
}
