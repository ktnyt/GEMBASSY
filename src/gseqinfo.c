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
  embInitPV("gseqinfo",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq    = NULL;
  AjPStr    inseq  = NULL;
  AjPStr    accid  = NULL;
  int       n;
  char*     jobid;

  seqall = ajAcdGetSeqall("sequence");
  accid  = ajAcdGetString("accid");

  while(ajSeqallNext(seqall, &seq)){

    soap_init(&soap);
    
    inseq = NULL;

    ajStrAppendC(&inseq, ">");
    ajStrAppendS(&inseq, ajSeqGetNameS(seq));
    ajStrAppendC(&inseq, "\n");
    ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

    if(!ajStrGetLen(accid))
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    
    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"A\tT\tG\tC\t\n");

    if(soap_call_ns1__seqinfo(
			      &soap, NULL, NULL,
			      in0, &jobid
			      ) == SOAP_OK){
      char* dlm = "<>";
      char* tp;
      int   list[4];
      tp = strtok(jobid, dlm);
      n=1;
      while(tp != NULL){
	tp = strtok(NULL, dlm);
	if(n % 3 == 1)
	  list[(n - 1) / 3] = atoi(tp);
	n++;
      }
      fprintf(stdout, "%d\t%d\t%d\t%d\t\n", list[0], list[1], list[2], list[3]);
    }else{
      soap_print_fault(&soap, stderr);
    }
    
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  
  embExit();
  return 0;
}
