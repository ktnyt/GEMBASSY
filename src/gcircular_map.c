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
  embInitPV("gcircular_map",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  struct ns1__circular_USCOREmapInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seqall = ajAcdGetSeqall("sequence");

  params.gmap = 0;
  
  while(ajSeqallNext(seqall,&seq)){
    soap_init(&soap);

    inseq = NULL;
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
    
    char* in0;
    in0 = ajCharNewS(inseq);
    if(soap_call_ns1__circular_USCOREmap(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      ajStrAppendC(&filename,".csv");
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
  }
  
  embExit();
  return 0;
}
