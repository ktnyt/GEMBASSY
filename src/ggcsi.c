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
  embInitPV("ggcsi", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__gcsiInputParams params;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  ajint		  window = 0;
  AjBool	  _gcsi = 0;
  AjBool	  at = 0;
  AjBool	  purine = 0;
  AjBool	  keto = 0;
  AjBool	  pval = 0;
  AjPStr	  accid = NULL;
  char           *result;

  AjBool	  show = 0;
  AjPFile	  outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  window = ajAcdGetInt("window");
  _gcsi = ajAcdGetBoolean("gcsi");
  at = ajAcdGetBoolean("at");
  purine = ajAcdGetBoolean("purine");
  keto = ajAcdGetBoolean("keto");
  pval = ajAcdGetBoolean("pval");
  accid = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

  params.window = window;
  if (_gcsi) {
    params.version = 1;
  } else {
    params.version = 2;
  }
  if (at) {
    params.at = 1;
  } else {
    params.at = 0;
  }
  if (purine) {
    params.purine = 1;
  } else {
    params.purine = 0;
  }
  if (keto) {
    params.keto = 1;
  } else {
    params.keto = 0;
  }
  if (pval) {
    params.p = 1;
  } else {
    params.p = 0;
  }

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

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
    in0 = ajCharNewS(inseq);

    if (soap_call_ns1__gcsi(
			    &soap, NULL, NULL,
			    in0, &params, &result
			    ) == SOAP_OK) {
      AjPStr	      tmp = ajStrNewC(result);
      AjPStr	      parse = ajStrNew();
      AjPStr	      gcsi = NULL;
      AjPStr	      sa = NULL;
      AjPStr	      dist = NULL;
      AjPStr	      z = NULL;
      AjPStr	      p = NULL;
      AjPStrTok	      handle = NULL;
      ajStrExchangeCC(&tmp, "<", "\n");
      ajStrExchangeCC(&tmp, ">", "\n");
      handle = ajStrTokenNewC(tmp, "\n");
      while (ajStrTokenNextParse(&handle, &parse)) {
	if (ajStrIsFloat(parse))
	  if (!gcsi)
	    gcsi = ajStrNewS(parse);
	  else if (!sa)
	    sa = ajStrNewS(parse);
	  else if (!dist)
	    dist = ajStrNewS(parse);
	  else if (!z)
	    z = ajStrNewS(parse);
	  else if (!p)
	    p = ajStrNewS(parse);
      }
      tmp = ajFmtStr("Sequence: %S GCSI: %S SA: %S DIST: %S",
		     accid, gcsi, sa, dist);
      if (pval)
	tmp = ajFmtStr("%S Z: %S P: %S", tmp, z, p);
      if (show)
	ajFmtPrint("%S\n", tmp);
      else
	ajFmtPrintF(outf, "%S\n", tmp);
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
