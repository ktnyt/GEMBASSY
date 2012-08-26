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
  embInitPV("gdinuc",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__dinucInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjPStr    id        = NULL;
  AjPStr    position  = NULL;
  AjBool    translate = 0;
  AjPStr    delkey    = NULL;
  AjBool    accid    = 0;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seqall    = ajAcdGetSeqall("sequence");
  id        = ajAcdGetString("id");
  position  = ajAcdGetString("position");
  translate = ajAcdGetBoolean("translate");
  delkey    = ajAcdGetString("delkey");
  accid     = ajAcdGetBoolean("accid");

  if(translate){
    params.translate   = 1;
  }else{
    params.translate   = 0;
  }
  params.del_USCOREkey = ajCharNewS(delkey);
  params.position      = ajCharNewS(position);
  params.output        = "f";

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
    if(soap_call_ns1__dinuc(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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

  ajSeqallDel(seqall);
  ajSeqDel(seq);
  ajStrDel(inseq);
  ajStrDel(filename);
  
  embExit();
  return 0;
}
