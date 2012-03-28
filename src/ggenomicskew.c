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
  embInit("ggenomicskew",argc,argv);

  struct soap soap;
  struct ns1__genomicskewInputParams params;

  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     divide     = 0;
  AjBool    at         = 0;
  AjPStr    output     = NULL;
	AjPStr    filename   = NULL;
  char*     jobid;
	char*     _result;

  seq        = ajAcdGetSeq("sequence");
  divide     = ajAcdGetInt("divide");
  at         = ajAcdGetBoolean("at");
  output     = ajAcdGetString("output");

  params.divide       = divide;
  if(at){
    params.at         = 1;
  }else{
    params.at         = 0;
  }
  params.output       = ajCharNewS(output);

  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__genomicskew(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
    if(strcmp(params.output,"g") == 0){
      filename = ajAcdGetString("filename");
      if(ajStrMatchC(filename,"ggenomicskew.png(csv)")){
        ajStrAssignC(&filename,"ggenomicskew.png");
      }
    }else{
      filename = ajAcdGetString("filename");
      if(ajStrMatchC(filename,"ggenomicskew.png(csv)")){
        ajStrAssignC(&filename,"ggenomicskew.csv");
      }
    }
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
