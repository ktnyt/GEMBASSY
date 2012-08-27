#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int main(int argc, char *argv[]){
  embInitPV("gaaui",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__aauiInputParams params;

  AjPSeqall     seqall;
  AjPSeq        seq;
  AjPStr        inseq     = NULL;
  AjPStr        id        = NULL;
  AjBool        accid     = 0;
  AjPStr        filename  = NULL;
  ajint         i         = 0;
  ajint         j         = 0;
  char*         jobid;
  
  seqall = ajAcdGetSeqall("sequence");
  id     = ajAcdGetString("id");
  accid  = ajAcdGetBoolean("accid");
  
  params.id     = ajCharNewS(id);
  params.output = "f";

  while(ajSeqallNext(seqall,&seq)){  
    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq);
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }

    soap_init(&soap);

    char* in0;
    in0 = ajCharNewS(inseq);
    if(soap_call_ns1__aaui(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetAccS(seq));
      ajStrAppendC(&filename,".csv");
      fprintf(stderr,"Retrieving file:%s\n",ajCharNewS(filename));
      if(get_file(jobid,ajCharNewS(filename))==0){
	fprintf(stderr,"Retrieval successful\n");
      }else{
	fprintf(stderr,"Retrieval unsuccessful\n");
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
