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
  embInitPV("gbase_entropy",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__base_USCOREentropyInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    position   = NULL;
  ajint     PatLen     = 0;
  ajint     upstream   = 0;
  ajint     downstream = 0;
  AjPStr    output     = NULL;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     jobid; 

  seqall     = ajAcdGetSeqall("sequence");
  position   = ajAcdGetString("position");
  PatLen     = ajAcdGetInt("patlen");
  upstream   = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  output     = ajAcdGetString("output");
  accid      = ajAcdGetString("accid");
  
  params.position   = ajCharNewS(position);
  params.PatLength  = PatLen;
  params.upstream   = upstream;
  params.downstream = downstream;
  params.output     = ajCharNewS(output);
  
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


    if(soap_call_ns1__base_USCOREentropy(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      if(strcmp(params.output,"g") == 0){
	ajStrAppendC(&filename,".png");
      }else{
	ajStrAppendC(&filename,".csv");
      }
      if(get_file(jobid,ajCharNewS(filename))==0){
        fprintf(stderr,"Retrieval successful\n");

  if(strcmp(params.output,"show") == 0)
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
  }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);

  embExit();
  return 0;
}
