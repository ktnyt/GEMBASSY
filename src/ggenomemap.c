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
  embInitPV("ggenomemap",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__genome_USCOREmapInputParams params;

  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  ajint     window = 0;
  double    amp    = 0;
  AjBool    name   = 0;
  AjPStr    filename   = NULL;
  char*     _result; 
  char*     jobid;

  seq    = ajAcdGetSeq("sequence");
  window = ajAcdGetInt("window");
  amp    = ajAcdGetFloat("amp");
  name   = ajAcdGetBoolean("name");

  params.window = window;
  params.amp    = amp;
  if(name){
    params.name = 1;
  }else{
    params.name = 0;
  }

    
  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendC(&inseq,">");
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  ajStrAppendC(&inseq,"\n");
  ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__genome_USCOREmap(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
