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
  embInitPV("grep_ori_ter",argc,argv,"GEMBASSY","0.0.1");

  struct soap soap;
  struct ns1__rep_USCOREori_USCOREterInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq        = NULL;
  AjBool    oriloc       = 0;
  AjBool    gcskew       = 0;
  AjBool    dbonly       = 0;
  ajint     difthreshold = 0;
  AjBool    accid        = 0;
  AjPStr    filename     = NULL;
  char*     jobid;

  seqall       = ajAcdGetSeqall("sequence");
  difthreshold = ajAcdGetInt("difthreshold");
  oriloc       = ajAcdGetBoolean("oriloc");
  gcskew       = ajAcdGetBoolean("gcskew");
  dbonly       = ajAcdGetBoolean("dbonly");
  accid        = ajAcdGetBoolean("accid");

  params.dif_threshold   = difthreshold;
  if(oriloc){
    params.oriloc = 1;
  }else{
    params.oriloc = 0;
  }
  if(gcskew){
    params.gcskew = 1;
  }else{
    params.gcskew = 0;
  }
  if(dbonly){
    params.dbonly = 1;
  }else{
    params.dbonly = 0;
  }
    
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

    fprintf(stderr,"%s\norigin\tterminus\n",ajCharNewS(ajSeqGetAccS(seq)));

    if(!ajSeqGetFeat(seq) && !accid)
      fprintf(stderr,"Sequence does not have features\nProceeding with sequence accession ID\n");

    if(soap_call_ns1__rep_USCOREori_USCOREter(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      char* dlm = "<>";
      char* tp  = jobid;
      tp = strtok(tp,dlm);
      tp = strtok(NULL,dlm);
      printf("%s\t",tp);
      tp = strtok(NULL,dlm);
      tp = strtok(NULL,dlm);
      tp = strtok(NULL,dlm);
      printf("%s\n",tp);
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
