#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInit("gquerystrand",argc,argv);

  struct soap soap;
  struct ns1__query_USCOREstrandInputParams params;

  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  char*     position;
  AjPStr    direction  = NULL;
  AjPStr    mode  = NULL;
  char*     jobid;

  seq        = ajAcdGetSeq("sequence");
  direction  = ajAcdGetString("direction");
  position   = ajCharNewS(ajAcdGetString("position"));
  mode = ajAcdGetString("mode");

  params.direction  = ajCharNewS(direction);
    
  soap_init(&soap);
  
  inseq = NULL;
    seq=ajAcdGetSeq("sequence");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__query_USCOREstrand(&soap,NULL,NULL,in0,position,&params,&jobid)==SOAP_OK){
    printf("%s\n",jobid);
  }else{
    soap_print_fault(&soap,stderr);
  }
      
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  embExit();
  return 0;
}
