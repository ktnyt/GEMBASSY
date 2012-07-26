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
  embInitPV("ggeneskew",argc,argv,"GEMBASSY","0.0.1");

  struct soap soap;
  struct ns1__geneskewInputParams params;

  AjPSeq sequence;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     window     = 0;
  ajint     slide      = 0;
  AjBool    cumulative = 0;
  AjBool    gc3;
  AjPStr    output     = NULL;
  AjPStr    base;
	AjPStr    filename   = NULL;
  char*     _result; 
  char*     jobid;

  seq        = ajAcdGetSeq("sequence");
  window     = ajAcdGetInt("window");
  slide      = ajAcdGetInt("slide");
  cumulative = ajAcdGetBoolean("cumulative");
  gc3        = ajAcdGetBoolean("gctri");
  output     = ajAcdGetString("output");
  base       = ajAcdGetString("base");
  
  params.window       = window;
  params.slide        = slide;
  if(cumulative){
    params.cumulative = 1;
  }else{
    params.cumulative = 0;
  }
  if(gc3){
    params.gc3         = 1;
  }else{
    params.gc3         = 0;
  }
  params.base         = ajCharNewS(base);
  params.output       = ajCharNewS(output);

  
  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__geneskew(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
    if(strcmp(params.output,"g") == 0){
      filename = ajAcdGetString("filename");
      if(ajStrMatchC(filename,"ggeneskew.png(csv)")){
        ajStrAssignC(&filename,"ggeneskew.png");
      }
    }else{
      filename = ajAcdGetString("filename");
      if(ajStrMatchC(filename,"ggeneskew.png(csv)")){
        ajStrAssignC(&filename,"ggeneskew.csv");
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
