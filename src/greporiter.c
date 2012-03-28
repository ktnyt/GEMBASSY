#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInit("greporiter",argc,argv);

  struct soap soap;
  struct ns1__rep_USCOREori_USCOREterInputParams params;

  AjPSeq    seq;
  AjPStr    inseq        = NULL;
  AjBool    oriloc       = 0;
  AjBool    gcskew       = 0;
	AjBool    dbonly       = 0;
  ajint     difthreshold = 0;
  char*     _result; 
  char*     jobid;

  seq          = ajAcdGetSeq("sequence");
  difthreshold = ajAcdGetInt("difthreshold");
  oriloc       = ajAcdGetBoolean("oriloc");
  gcskew       = ajAcdGetBoolean("gcskew");
  dbonly       = ajAcdGetBoolean("dbonly");

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
    
  soap_init(&soap);
  
  inseq = NULL;
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__rep_USCOREori_USCOREter(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
    char* dlm = "<>";
    char* tp  = jobid;
    printf("origin\tterminus\n");
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
  
  embExit();
  return 0;
}
