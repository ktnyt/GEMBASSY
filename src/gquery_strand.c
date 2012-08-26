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
  embInitPV("gquery_strand",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__query_USCOREstrandInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  char*     position  = NULL;
  AjPStr    direction = NULL;
  AjBool    accid     = 0;
  AjPStr    filename  = NULL;
  char*     jobid;

  seqall    = ajAcdGetSeqall("sequence");
  direction = ajAcdGetString("direction");
  position  = ajCharNewS(ajAcdGetString("position"));
  accid     = ajAcdGetBoolean("accid");

  params.direction  = ajCharNewS(direction);
    
  while(ajSeqallNext(seqall,&seq)){
    soap_init(&soap);

    inseq = NULL;
    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq);
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }
    
    char* in0;
    in0 = ajCharNewS(inseq);
    fprintf(stderr,"%s:\n",ajSeqGetAccS(seq));
    if(soap_call_ns1__query_USCOREstrand(&soap,NULL,NULL,in0,position,&params,&jobid)==SOAP_OK){
      puts(jobid);
    }else{
      soap_print_fault(&soap,stderr);
    }
  
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  ajSeqallDel(seqall);
  ajSeqDel(seq);
  ajStrDel(inseq);
  ajStrDel(filename);

  embExit();
  return 0;
}
