#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInitPV("gbaseinfocontent",argc,argv,"GEMBASSY","0.0.1");

  struct soap soap;
  struct ns1__base_USCOREinformation_USCOREcontentInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     upstream   = 0;
  ajint     downstream = 0;
  ajint     PatLen     = 0;
  AjPStr    position   = NULL;
  char*     _result; 
  char*     jobid;

  seq=ajAcdGetSeq("sequence");
  upstream   = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  PatLen     = ajAcdGetInt("patlen");
  position   = ajAcdGetString("position");
  
  params.upstream   = upstream;
  params.downstream = downstream;
  params.PatLength  = PatLen;
  params.position   = ajCharNewS(position);
  params.output     = "g";
  
  
  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__base_USCOREinformation_USCOREcontent(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
    puts(jobid);
  }else{
    soap_print_fault(&soap,stderr);
  }
  
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  
  embExit();
  return 0;
}
