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
  embInitPV("gleading_strand",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  
  AjPSeqout seqout;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjBool    accid      = 0;
  AjPStr    filename   = NULL;
  char*     jobid;
  
  seq    = ajAcdGetSeq("sequence");
  seqout = ajAcdGetSeqout("outseq");
  accid  = ajAcdGetBoolean("accid");
  
  soap_init(&soap);
  
  inseq = NULL;
  seq=ajAcdGetSeq("sequence");
  ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  ajStrAppendS(&filename,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__leading_USCOREstrand(&soap,NULL,NULL,in0,&jobid)==SOAP_OK){
    printf("Writing sequence out to %s\n",ajCharNewS(filename));
    ajCharFmtUpper(jobid);
    ajSeqAssignSeqC(seq, jobid);
    ajSeqoutWriteSeq(seqout, seq);
    ajSeqoutClose(seqout);
  }else{
    soap_print_fault(&soap,stderr);
  }
  
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  
  embExit();
  return 0;
}
