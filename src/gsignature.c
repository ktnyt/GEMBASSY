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
  embInitPV("gsignature",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__signatureInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     wordlength = 0;
  AjBool    bothstrand = 0;
  AjBool    oe         = 0;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     jobid;

  seqall     = ajAcdGetSeqall("sequence");
  wordlength = ajAcdGetInt("wordlength");
  bothstrand = ajAcdGetBoolean("bothstrand");
  oe         = ajAcdGetBoolean("oe");
  accid      = ajAcdGetString("accid");
  
  params.wordlength = wordlength;
  if(bothstrand){
    params.bothstrand = 1;
  }else{
    params.bothstrand = 0;
  }
  if(oe){
    params.oe = 1;
  }else{
    params.oe = 0;
  }

  while(ajSeqallNext(seqall,&seq)){  

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !strlen(ajCharNewS(accid))){
      inseq = getGenbank(seq,ajSeqGetFeat(seq));
    }else{
      if(!strlen(ajCharNewS(accid))){
        fprintf(stderr,"Sequence does not have features\n");
        fprintf(stderr,"Proceeding with sequence accession ID\n");
        ajStrAssignS(&inseq,ajSeqGetAccS(seq));
      }
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr,"Invalid accession ID, exiting");
          return 1;
      }else{
        ajStrAssignS(&inseq,accid);
      }
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));


    if(soap_call_ns1__signature(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetAccS(seq));
      ajStrAppendC(&filename,".csv");
      if(get_file(jobid,ajCharNewS(filename)) == 0){
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

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);
    
  embExit();
  return 0;
}
