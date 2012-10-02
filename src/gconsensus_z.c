#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

char *_upper(char *s){
  char *p;
  for(p=s; *p; p++){
    *p = toupper(*p);
  }
  return(s);
}

int main(int argc, char *argv[]){
  embInitPV("gconsensus_z",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  struct ns1__consensus_USCOREzInputParams params;
  struct arrayIn array_seq;

  array_seq.__size = 0;
  
  AjPSeqall seqall;
  AjPSeqout seqout;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    filename = NULL;
  ajint     high     = 0;
  double    low      = 0;  
  char*     jobid;
  
  seqall = ajAcdGetSeqall("sequence");
  high   = ajAcdGetInt("high");
  low    = ajAcdGetFloat("low");
  seqout = ajAcdGetSeqout("outseq");
  
  params.high = high;
  params.low  = low;

  char* tmp[ajSeqallGetCount(seqall)];

  while(ajSeqallNext(seqall,&seq)){
    char* in0 = NULL;
    in0 = ajCharNewS(ajSeqGetSeqS(seq));
    tmp[array_seq.__size] = ajCharNewS(ajSeqGetSeqS(seq));
    array_seq.__size ++;
  }

  array_seq.__ptr = tmp;

  soap_init(&soap);

  if(soap_call_ns1__consensus_USCOREz(&soap,NULL,NULL,&array_seq,&params,&jobid)==SOAP_OK){
    puts(jobid);
    /*ajCharFmtUpper(jobid);
    ajSeqAssignSeqC(seq, jobid);
    ajSeqoutWriteSeq(seqout, seq);
    ajSeqoutClose(seqout);*/
  }else{
    soap_print_fault(&soap,stderr);
  }
    
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);

  embExit();
  return 0;
}
