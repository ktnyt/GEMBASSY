#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInitPV("gicdi",argc,argv,"GEMBASSY","0.0.1");

  struct soap soap;
  struct ns1__icdiInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjBool    translate = 0;
  AjPStr    id        = NULL;
  AjPStr    delkey   = NULL;
  char*     _result; 
  char*     jobid;
  
  seq        = ajAcdGetSeq("sequence");
  translate  = ajAcdGetBoolean("translate");
  id         = ajAcdGetString("id");
  delkey     = ajAcdGetString("delkey");
  
  if(translate){
    params.translate   = 1;
  }else{
    params.translate   = 0;
  }
  params.id            = ajCharNewS(id);
  params.del_USCOREkey = ajCharNewS(delkey);
  
  
  soap_init(&soap);
  
  inseq = NULL;
    seq=ajAcdGetSeq("sequence");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
    
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__icdi(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
