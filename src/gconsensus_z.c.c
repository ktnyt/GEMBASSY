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
  embInitPV("gconsensus_z", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__consensus_USCOREzInputParams params;
  struct arrayIn  array_seq;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  ajint		  high = 0;
  double	  low = 0;
  char           *result;

  AjBool	  plot = 0;
  AjPFile	  outf = NULL;
  AjPGraph	  mult = NULL;

  AjPStr	  filename = getUniqueFileName();

  gPlotParams	  gpp;

  seqall = ajAcdGetSeqall("sequence");
  high = ajAcdGetInt("high");
  low = ajAcdGetFloat("low");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.high = high;
  params.low = low;
  params.output = "f";

  char          **tmp = (char **)malloc(sizeof(char));
  int		  size = 0;

  while (ajSeqallNext(seqall, &seq)) {
    tmp = (char **)realloc(tmp, sizeof(char) * (size + 1));
    tmp[size] = ajCharNewS(ajSeqGetSeqS(seq));
    size++;
  }

  array_seq.__size = size;
  array_seq.__ptr = tmp;

  if (size < 2) {
    fprintf(stderr, "File only has one sequence. Please input more than two.\n");
    return 0;
  }
  soap_init(&soap);

  if (soap_call_ns1__consensus_USCOREz(
				       &soap, NULL, NULL,
				       &array_seq, &params, &result
				       ) == SOAP_OK) {
    if (get_file(result, ajCharNewS(filename)) == 0) {
      if (plot) {
	AjPStr		title = NULL;
	ajStrAppendC(&title, argv[0]);
	gpp.title = ajStrNewS(title);
	gpp.xlab = ajStrNewC("position");
	gpp.ylab = ajStrNewC("consensus");
	ajStrDel(&title);
	if (gPlotFile(filename, mult, &gpp) == 1)
	  fprintf(stderr, "Error allocating\n");
      } else {
	ajFmtPrintF(outf, "Sequence: %S\n%S\n",
		    ajSeqGetAccS(seq), getContentS(filename));
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

  if (outf)
    ajFileClose(&outf);

  free(tmp);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);

  embExit();
  return 0;
}
