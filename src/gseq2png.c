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
  embInitPV("gseq2png",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  struct ns1__seq2pngInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  ajint     width  = 0;
  ajint     window = 0;
  AjPStr    output = NULL;
  AjPStr    mode  = NULL;
	AjPStr    filename   = NULL;
  char*     jobid;
  char*     _result;
  
  seq        = ajAcdGetSeq("sequence");
  window     = ajAcdGetInt("window");
  width      = ajAcdGetInt("width");
  output     = ajAcdGetString("output");
  mode = ajAcdGetString("mode");
  
  params.window = window;
  params.width  = width;
  params.output = ajCharNewS(output);

  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendC(&inseq,">");
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  ajStrAppendC(&inseq,"\n");
  ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__seq2png(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
