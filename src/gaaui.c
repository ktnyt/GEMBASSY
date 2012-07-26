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
  embInitPV("gaaui",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  struct ns1__aauiInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    id       = NULL;
  AjPStr    filename = NULL;
  char*     jobid;
  
  seq = ajAcdGetSeq("sequence");
  id   = ajAcdGetString("id");
  
  params.id = ajCharNewS(id);
  
  soap_init(&soap);
  
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__aaui(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
