#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"
#include "../include/gplot.h"

int
main(int argc, char *argv[])
{
  embInitPV("gbase_relative_entropy", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__base_USCORErelative_USCOREentropyInputParams params;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  AjPStr	  position = NULL;
  ajint		  PatLen = 0;
  ajint		  upstream = 0;
  ajint		  downstream = 0;
  AjPStr	  accid = NULL;
  char           *result;

  AjBool	  plot = 0;
  AjPFile	  outf = NULL;
  AjPGraph	  mult = NULL;

  gPlotParams	  gpp;

  AjPStr	  filename = getUniqueFileName();

  seqall = ajAcdGetSeqall("sequence");
  position = ajAcdGetString("position");
  PatLen = ajAcdGetInt("patlen");
  upstream = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  accid = ajAcdGetString("accid");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.position = ajCharNewS(position);
  params.PatLength = PatLen;
  params.upstream = upstream;
  params.downstream = downstream;
  params.output = "f";

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
    if (soap_call_ns1__base_USCORErelative_USCOREentropy(
							 &soap, NULL, NULL,
						       in0, &params, &result
							 ) == SOAP_OK) {
      if (get_file(result, ajCharNewS(filename)) == 0) {
	if (plot) {
	  AjPStr	  title = NULL;
	  ajStrAppendC(&title, argv[0]);
	  ajStrAppendC(&title, " of ");
	  ajStrAppendS(&title, accid);
	  gpp.title = ajStrNewS(title);
	  gpp.xlab = ajStrNewC("position");
	  gpp.ylab = ajStrNewC("entropy");
	  ajStrDel(&title);
	  if (gPlotFile(filename, mult, &gpp) == 1)
	    fprintf(stderr, "Error in plotting\n");
	} else {
	  ajFmtPrintF(outf, "Sequence: %S\n%S\n",
		      accid, getContentS(filename));
	}
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

  if (outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);

  embExit();
  return 0;
}
