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
  embInitPV("gdnawalk",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  struct ns1__dnawalkInputParams params;
  
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  AjBool    gmap   = 0;
	AjPStr    filename   = NULL;
  char*     jobid;
  char*     _result;
  
  seq  = ajAcdGetSeq("sequence");
  gmap = ajAcdGetBoolean("gmap");
  
	if(gmap){
		params.gmap = 1;
	}else{
		params.gmap = 0;
	}

  soap_init(&soap);
  
  inseq = NULL;
  ajStrAppendC(&inseq,">");
  ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  ajStrAppendC(&inseq,"\n");
  ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__dnawalk(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
