#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/getfile.h"

int main(int argc, char *argv[]){
  embInitPV("gdeltaenc",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  AjPStr    filename   = NULL;
  char*     jobid;
  char*     _result;
  
  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__delta_USCOREenc(&soap,NULL,NULL,in0,&jobid)==SOAP_OK){
    filename = ajAcdGetString("filename");
    if(get_file(jobid,ajCharNewS(filename))==0){
      printf("Retrieval successful\n");
    }else{
      printf("Retrieval unsuccessful\n");
    }
  }else{
    soap_print_fault(&soap,stderr);
  }
  
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  
  embExit();
  return 0;
}