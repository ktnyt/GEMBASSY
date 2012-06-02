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
  embInitPV("ggcskew",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__gcskewInputParams params;

  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     window     = 0;
  ajint     slide      = 0;
  AjBool    cumulative = 0;
  AjBool    at         = 0;
  AjBool    purine     = 0;
  AjBool    keto       = 0;
  AjPStr    output     = NULL;
  AjPStr    mode       = NULL;
  AjPStr    filename   = NULL;
  char*     _result; 
  char*     jobid;

  seq        = ajAcdGetSeq("sequence");
  window     = ajAcdGetInt("window");
  slide      = ajAcdGetInt("slide");
  cumulative = ajAcdGetBoolean("cumulative");
  at         = ajAcdGetBoolean("at");
  purine     = ajAcdGetBoolean("purine");
  keto       = ajAcdGetBoolean("keto");
  output     = ajAcdGetString("output");
  mode       = ajAcdGetString("mode");

  params.window       = window;
  params.slide        = slide;
  if(cumulative){
    params.cumulative = 1;
  }else{
    params.cumulative = 0;
  }
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
  params.output       = ajCharNewS(output);

    
  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendC(&inseq,">");
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  ajStrAppendC(&inseq,"\n");
  ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__gcskew(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
    if(strcmp(params.output,"g") == 0){
      filename = ajAcdGetString("filename");
      if(ajStrMatchC(filename,"ggcskew.png(csv)")){
	ajStrAssignC(&filename,"ggcskew.png");
      }
    }else{
      filename = ajAcdGetString("filename");
      if(ajStrMatchC(filename,"ggcskew.png(csv)")){
	ajStrAssignC(&filename,"ggcskew.csv");
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
