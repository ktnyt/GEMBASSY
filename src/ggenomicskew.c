#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"
#include "../include/gplot.h"

int main(int argc, char *argv[])
{
  embInitPV("ggenomicskew", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__genomicskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  ajint	    divide = 0;
  AjBool    at = 0;
  AjPStr    accid = NULL;
  char     *result;

  AjBool    plot = 0;
  AjPFile   outf = NULL;
  AjPGraph  mult = NULL;

  AjPStr filename = gGetUniqueFileName();

  gPlotParams gpp;

  seqall = ajAcdGetSeqall("sequence");
  divide = ajAcdGetInt("divide");
  at     = ajAcdGetBoolean("at");
  accid  = ajAcdGetString("accid");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.divide = divide;
  if (at)
    params.at = 1;
  else
    params.at = 0;
  params.output = "f";

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    inseq = NULL;

    if (ajSeqGetFeat(seq) && !ajStrGetLen(accid)) {
      inseq = gFormatGenbank(seq);
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    } else {
      if (!ajStrGetLen(accid)) {
	fprintf(stderr, "Sequence does not have features\n");
	fprintf(stderr, "Proceeding with sequence accession ID\n");
	ajStrAssignS(&accid, ajSeqGetAccS(seq));
      }
      if (!gValID(accid)) {
	fprintf(stderr, "Invalid accession ID, exiting");
	return 1;
      } else {
	ajStrAssignS(&inseq, accid);
      }
    }

    char           *in0;
    in0 = ajCharNewS(inseq);

    if (soap_call_ns1__genomicskew(
				   &soap, NULL, NULL,
				   in0, &params, &result
				   ) == SOAP_OK) {
      gGetFileFromURL(ajStrNewC(result), filename);
      return 0;
      if (gGetFileFromURL(ajStrNewC(result), filename) == 0) {
	if (plot) {
	  AjPStr	  title = NULL;
	  AjPPStr	  names = NULL;
	  if ((names = (AjPPStr) malloc(sizeof(AjPStr) * 5)) == NULL) {
	    fprintf(stderr, "Error in memory allocation, exiting\n");
	    return 1;
	  }
	  names[0] = NULL;
	  names[1] = ajStrNewC("whole genome");
	  names[2] = ajStrNewC("coding region");
	  names[3] = ajStrNewC("intergenic region");
	  names[4] = ajStrNewC("codon third position");
	  ajStrAppendC(&title, argv[0]);
	  ajStrAppendC(&title, " of ");
	  ajStrAppendS(&title, accid);
	  gpp.title = ajStrNewS(title);
	  gpp.xlab = ajStrNewC("location");
	  gpp.ylab = ajStrNewC("GC skew");
	  gpp.names = names;
	  ajStrDel(&title);
	  if (gPlotFile(filename, mult, &gpp) == 1)
	    fprintf(stderr, "Error in plotting\n");
	} else {
	  AjPStr content;
	  gGetFileContent(&content, filename);
	  ajFmtPrintF(outf, "Sequence: %S\n%S\n",
		      accid, content);
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
