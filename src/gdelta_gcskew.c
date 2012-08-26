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
  embInitPV("gdelta_gcskew",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__delta_USCOREgcskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjBool    at         = 0;
  AjBool    purine     = 0;
  AjBool    keto       = 0;
  AjBool    accid    = 0;
  AjPStr    filename   = NULL;
  char*     jobid;

  seqall     = ajAcdGetSeqall("sequence");
  at         = ajAcdGetBoolean("at");
  purine     = ajAcdGetBoolean("purine");
  keto       = ajAcdGetBoolean("keto");
  accid      = ajAcdGetBoolean("accid");

  if(at){
    params.at         = 1;
  }else{
    params.at         = 0;
  }
  if(purine){
    params.purine     = 1;
  }else{
    params.purine     = 0;
  }
  if(keto){
    params.keto       = 1;
  }else{
    params.keto       = 0;
  }
  
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
    if(soap_call_ns1__delta_USCOREgcskew(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
