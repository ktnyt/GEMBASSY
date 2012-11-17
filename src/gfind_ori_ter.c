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
  embInitPV("gfind_ori_ter", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__find_USCOREori_USCOREterInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  ajint     window   = 0;
  AjBool    purine   = 0;
  AjBool    keto     = 0;
  ajint     filter   = 0;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  char*     jobid;

  seqall = ajAcdGetSeqall("sequence");
  window = ajAcdGetInt("window");
  filter = ajAcdGetInt("filt");
  purine = ajAcdGetBoolean("purine");
  keto   = ajAcdGetBoolean("keto");
  accid  = ajAcdGetString("accid");

  params.window   = window;
  params.filter   = filter;
  if(purine){
    params.purine = 1;
  }else{
    params.purine = 0;
  }
  if(keto){
    params.keto   = 1;
  }else{
    params.keto   = 0;
  }

  while(ajSeqallNext(seqall, &seq)){

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !ajStrGetLen(accid)){
      inseq = getGenbank(seq);
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    }else{
      if(!ajStrGetLen(accid)){
        fprintf(stderr, "Sequence does not have features\n");
        fprintf(stderr, "Proceeding with sequence accession ID\n");
        ajStrAssignS(&accid,ajSeqGetAccS(seq));
      }
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr, "Invalid accession ID, exiting");
          return 1;
      }
      ajStrAssignS(&inseq, accid);
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    if(soap_call_ns1__find_USCOREori_USCOREter(
					       &soap, NULL, NULL,
					       in0, &params, &jobid
					       ) == SOAP_OK){
      char* dlm = "<>";
      char* tp  = jobid;
      tp = strtok(tp, dlm);
      tp = strtok(NULL, dlm);
      fprintf(stdout, "%s\t", tp);
      tp = strtok(NULL, dlm);
      tp = strtok(NULL, dlm);
      tp = strtok(NULL, dlm);
      fprintf(stdout, "%s\n", tp);
    }else{
      soap_print_fault(&soap, stderr);
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
