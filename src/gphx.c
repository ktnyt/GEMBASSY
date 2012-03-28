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
  embInitPV("gphx",argc,argv,"GEMBASSY","0.0.1");

  struct soap soap;
  struct ns1__phxInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjBool    translate = 0;
  AjPStr    usage     = NULL;
  AjPStr    delkey    = NULL;
  AjPStr    filename   = NULL;
  char*     _result; 
  char*     jobid;
  int       i=0;
  
  seq        = ajAcdGetSeq("sequence");
  translate  = ajAcdGetBoolean("translate");
  usage      = ajAcdGetString("usage");
  delkey     = ajAcdGetString("delkey");
  
  if(translate){
    params.translate   = 1;
  }else{
    params.translate   = 0;
  }
  params.usage         = ajCharNewS(usage);
  params.del_USCOREkey = ajCharNewS(delkey);
  
    
  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__phx(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
    printf("Retrieving file from:\n%s\n",jobid);
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
