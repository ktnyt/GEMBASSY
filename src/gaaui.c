#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int main(int argc, char *argv[]){
  embInitPV("gaaui", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__aauiInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjPStr    id        = NULL;
  AjPStr    accid     = NULL;
  char*     result;

  AjPFile outf = NULL;

  AjPStr filename = getUniqueFileName();

  seqall = ajAcdGetSeqall("sequence");
  id     = ajAcdGetString("id");
  accid  = ajAcdGetString("accid");

  outf = ajAcdGetOutfile("outfile");

  params.id = ajCharNewS(id);

  while(ajSeqallNext(seqall, &seq)){

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !ajStrGetLen(accid)){
      inseq = getGenbank(seq);
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    }else{
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr, "Invalid accession ID, exiting\n");
          return 0;
      }
      if(!ajStrGetLen(accid)){
	fprintf(stderr, "Sequence does not have features\n");
	fprintf(stderr, "Proceeding with sequence accession ID\n");
	ajStrAssignS(&accid, ajSeqGetAccS(seq));
      }
      ajStrAssignS(&inseq, accid);
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    if(soap_call_ns1__aaui(
			   &soap, NULL, NULL,
			   in0, &params, &result
			   ) == SOAP_OK){
      if(get_file(result, ajCharNewS(filename))){
	fprintf(stderr, "Retrieval unsuccessful\n");
      }
      ajFmtPrintF(outf, "Sequence: %S %S\n",
		  ajSeqGetNameS(seq), getContentS(filename));
    }else{
      soap_print_fault(&soap, stderr);
    }

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&accid);
  ajStrDel(&id);
    
  embExit();
  return 0;
}
