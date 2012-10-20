#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"
#include "../include/display_png.h"

int main(int argc, char *argv[]){
  embInitPV("gconsensus_z",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  struct ns1__consensus_USCOREzInputParams params;
  struct arrayIn array_seq;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    output   = NULL;
  AjPStr    filename = NULL;
  ajint     high     = 0;
  double    low      = 0;  
  char*     jobid;
  
  seqall = ajAcdGetSeqall("sequence");
  high   = ajAcdGetInt("high");
  low    = ajAcdGetFloat("low");
  output = ajAcdGetString("output");
  
  params.high = high;
  params.low  = low;

  char** tmp  = (char**)malloc(sizeof(char));
  int    size = 0;

  while(ajSeqallNext(seqall,&seq)){
    if(size == 0) ajStrAssignS(&filename,ajSeqGetAccS(seq));
    tmp = (char**)realloc(tmp,sizeof(char)*(size+1));
    tmp[size] = ajCharNewS(ajSeqGetSeqS(seq));
    size++;
  }

  array_seq.__size = size;
  array_seq.__ptr  = tmp;

  soap_init(&soap);

  if(soap_call_ns1__consensus_USCOREz(&soap,NULL,NULL,&array_seq,&params,&jobid)==SOAP_OK){

    ajStrAppendC(&filename,".png");
    if(get_file(jobid,ajCharNewS(filename))==0){
      fprintf(stderr,"Retrieval successful\n");

      if(strcmp(ajCharNewS(output),"show") == 0)
	if(display_png(ajCharNewS(filename), argv[0], ajCharNewS(ajSeqGetAccS(seq))))
	  fprintf(stderr,"Error in X11 displaying\n");
    }else{
      fprintf(stderr,"Retrieval unsuccessful\n");
    }
  }else{
    soap_print_fault(&soap,stderr);
  }
    
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  free(tmp);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);

  embExit();
  return 0;
}
