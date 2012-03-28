#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/getfile.h":

int main(int argc, char *argv[]){
  embInitPV("gbui",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  struct ns1__buiInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjBool    translate = 0;
  AjPStr    position  = NULL;
  AjPStr    id        = NULL;
  AjPStr    delkey    = NULL;
  AjPStr    mode  = NULL;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seq        = ajAcdGetSeq("sequence");
  translate  = ajAcdGetBoolean("translate");
  position   = ajAcdGetString("position");
  id         = ajAcdGetString("id");
  delkey     = ajAcdGetString("delkey");
  mode       = ajAcdGetString("mode");
  
  if(translate){
    params.translate   = 1;
  }else{
    params.translate   = 0;
  }
  params.position      = ajCharNewS(position);
  params.id            = ajCharNewS(id);
  params.del_USCOREkey = ajCharNewS(delkey);
  
  
  soap_init(&soap);
  
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__bui(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
