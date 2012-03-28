#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInitPV("gaminoinfo",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  
  AjPSeq    seq   = NULL;
  AjPStr    inseq = NULL;
  AjPStr    mode  = NULL;
  int i;
  char*     _result;
  
  
  soap_init(&soap);
  
  mode = ajAcdGetString("mode");
  
  
  inseq = NULL;
  if(ajStrMatchC(mode,"id")||ajStrMatchC(mode,"ID")){
    seq=ajAcdGetSeq("sequence");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  }else if(ajStrMatchC(mode,"seq")||ajStrMatchC(mode,"SEQ")){
    seq=ajAcdGetSeq("sequence");
    ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  }else{
    ajStrAppendS(&inseq,mode);
  }
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__amino_USCOREinfo(&soap,NULL,NULL,in0,&_result)==SOAP_OK){
    puts(_result);
  }else{
      soap_print_fault(&soap,stderr);
  }
  
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  
  embExit();
  return 0;
}
