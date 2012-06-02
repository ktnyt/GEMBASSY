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
  embInitPV("gwvalue",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  struct ns1__w_USCOREvalueInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjPStr    include   = NULL;
  AjPStr    exclude   = NULL;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seq     = ajAcdGetSeq("sequence");
  include = ajAcdGetString("include");
  exclude = ajAcdGetString("exclude");
  
  params.include = ajCharNewS(include);
  params.exclude = ajCharNewS(exclude);
  params.output = "f";
  
  soap_init(&soap);
  
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__w_USCOREvalue(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
