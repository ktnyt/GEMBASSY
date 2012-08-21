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
  embInitPV("gbase_relative_entropy",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__base_USCORErelative_USCOREentropyInputParams params;

  AjPSeq sequence;
  AjPSeq seq;
  AjPStr inseq      = NULL;
  AjPStr position   = NULL;
  ajint  PatLen     = 0;
  ajint  upstream   = 0;
  ajint  downstream = 0;
  char*  jobid; 

  seq        = ajAcdGetSeq("sequence");
  position   = ajAcdGetString("position");
  PatLen     = ajAcdGetInt("patlen");
  upstream   = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  
  params.position   = ajCharNewS(position);
  params.PatLength  = PatLen;
  params.upstream   = upstream;
  params.downstream = downstream;
  params.output     = "g";
  
  soap_init(&soap);
  
  //ajFeattableNewSeq(seq);

  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__base_USCORErelative_USCOREentropy(&soap,NULL,NULL,"ecoli",&params,&jobid)==SOAP_OK){
    puts(jobid);
  }else{
    soap_print_fault(&soap,stderr);
  }
      
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  embExit();
  return 0;
}
