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
  embInitPV("glda_bias",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__lda_USCOREbiasInputParams params;

  AjPSeqall     seqall;
  AjPSeq        seq;
  AjPStr        inseq        = NULL;
  ajint         coefficients = 0;
  AjPStr        variable     = NULL;
  AjBool        accid        = 0;
  AjPStr        filename     = NULL;
  char*         jobid;

  seqall       = ajAcdGetSeqall("sequence");
  coefficients = ajAcdGetInt("coefficients");
  variable     = ajAcdGetString("variable");
  accid        = ajAcdGetBoolean("accid");
  
  params.coefficients = coefficients;
  params.variable     = ajCharNewS(variable);

  while(ajSeqallNext(seqall,&seq)){  

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq,ajSeqGetFeat(seq));
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    if(!ajSeqGetFeat(seq) && !accid)
      fprintf(stderr,"Sequence does not have features\nProceeding with sequence accession ID\n");

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));

    if(soap_call_ns1__lda_USCOREbias(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      puts(jobid);
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