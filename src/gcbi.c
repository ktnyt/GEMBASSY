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
  embInitPV("gcbi",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  struct ns1__cbiInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjBool    translate = 0;
  AjPStr    id        = NULL;
  AjPStr    delkey    = NULL;
  AjBool    accid    = 0;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seqall     = ajAcdGetSeqall("sequence");
  translate  = ajAcdGetBoolean("translate");
  id         = ajAcdGetString("id");
  delkey     = ajAcdGetString("delkey");
  accid      = ajAcdGetBoolean("accid");
  
  if(translate){
    params.translate   = 1;
  }else{
    params.translate   = 0;
  }
  params.id            = ajCharNewS(id);
  params.del_USCOREkey = ajCharNewS(delkey);
  
  while(ajSeqallNext(seqall,&seq)){
    soap_init(&soap);

    inseq = NULL;
    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq);
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }
    
    char* in0;
    in0 = ajCharNewS(inseq);
    fprintf(stderr,"%s:\n",ajSeqGetAccS(seq));
    if(soap_call_ns1__cbi(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      puts(jobid);
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
