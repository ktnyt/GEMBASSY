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
  embInitPV("grep_ori_ter", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__rep_USCOREori_USCOREterInputParams params;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  AjBool	  oriloc = 0;
  AjBool	  gcskew = 0;
  AjBool	  dbonly = 0;
  ajint		  difthreshold = 0;
  AjPStr	  accid = NULL;
  char           *result;

  AjBool	  show = 0;
  AjPFile	  outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  difthreshold = ajAcdGetInt("difthreshold");
  oriloc = ajAcdGetBoolean("oriloc");
  gcskew = ajAcdGetBoolean("gcskew");
  dbonly = ajAcdGetBoolean("dbonly");
  accid = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

  params.dif_threshold = difthreshold;
  if (oriloc) {
    params.oriloc = 1;
  } else {
    params.oriloc = 0;
  }
  if (gcskew) {
    params.gcskew = 1;
  } else {
    params.gcskew = 0;
  }
  if (dbonly) {
    params.dbonly = 1;
  } else {
    params.dbonly = 0;
  }

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    inseq = NULL;

    if (ajSeqGetFeat(seq) && !ajStrGetLen(accid)) {
      inseq = getGenbank(seq);
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    } else {
      if (!ajStrGetLen(accid)) {
	fprintf(stderr, "Sequence does not have features\n");
	fprintf(stderr, "Proceeding with sequence accession ID\n");
	ajStrAssignS(&accid, ajSeqGetAccS(seq));
      }
      if (!valID(ajCharNewS(accid))) {
	fprintf(stderr, "Invalid accession ID, exiting");
	return 1;
      }
      ajStrAssignS(&inseq, accid);
    }

    char           *in0;
    in0 = ajCharNewS(inseq);

    if (soap_call_ns1__rep_USCOREori_USCOREter(
					       &soap, NULL, NULL,
					       in0, &params, &result
					       ) == SOAP_OK) {
      AjPStr	      tmp = ajStrNewC(result);
      AjPStr	      parse = ajStrNew();
      AjPStr	      ori = NULL;
      AjPStr	      ter = NULL;
      AjPStrTok	      handle = NULL;
      ajStrExchangeCC(&tmp, "<", "\n");
      ajStrExchangeCC(&tmp, ">", "\n");
      handle = ajStrTokenNewC(tmp, "\n");
      while (ajStrTokenNextParse(&handle, &parse)) {
	if (ajStrIsInt(parse))
	  if (!ori)
	    ori = ajStrNewS(parse);
	  else if (!ter)
	    ter = ajStrNewS(parse);
      }
      if (show)
	ajFmtPrint("Sequence: %S Origin: %S Terminus: %S\n",
		   accid, ori, ter);
      else
	ajFmtPrintF(outf, "Sequence: %S Origin: %S Terminus: %S\n",
		    accid, ori, ter);
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
