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
  embInitPV("goligomer_search",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__oligomer_USCOREsearchInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjPStr    oligomer  = NULL;
  AjPStr    return_   = NULL;
  AjBool    accid     = 0;
  AjPStr    filename  = NULL;
  char*     jobid;

  seqall   = ajAcdGetSeqall("sequence");
  oligomer = ajAcdGetString("oligomer");
  return_  = ajAcdGetString("return");
  accid    = ajAcdGetBoolean("accid");
  
  params.return_ = ajCharNewS(return_);

  while(ajSeqallNext(seqall,&seq)){  

    soap_init(&soap);

    soap.send_timeout = 0;
    soap.recv_timeout = 0;

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq,ajSeqGetFeat(seq));
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }

    char* in0;
    char* in1;
    in0 = ajCharNewS(inseq);
    in1 = ajCharNewS(oligomer);

    if(!ajSeqGetFeat(seq) && !accid)
      fprintf(stderr,"Sequence does not have features\nProceeding with sequence accession ID\n");

    fprintf(stderr,"%s %s positions\n",ajCharNewS(ajSeqGetAccS(seq)),in1);

    if(soap_call_ns1__oligomer_USCOREsearch(&soap,NULL,NULL,in0,in1,&params,&jobid)==SOAP_OK){
      char* tp = jobid;
      char* dlm = "<>";
      int   pos = 0;;
      tp = strtok(tp,dlm);
      while((tp = strtok(NULL,dlm)) != NULL){
	if(pos++ % 3 == 0){
	  puts(tp);
	}
      }
    }else{
      soap_print_fault(&soap,stderr);
    }
    
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);
    
  embExit();
  return 0;
}