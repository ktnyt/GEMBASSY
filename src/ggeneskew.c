#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"
#include "../include/gplot.h"

int main(int argc, char *argv[]){
  embInitPV("ggeneskew",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__geneskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     window     = 0;
  ajint     slide      = 0;
  AjBool    cumulative = 0;
  AjBool    gc3;
  AjPStr    output     = NULL;
  AjPStr    base;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     _result; 
  char*     jobid;

  AjPGraph    mult;
  gPlotParams gpp;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  slide      = ajAcdGetInt("slide");
  cumulative = ajAcdGetBoolean("cumulative");
  gc3        = ajAcdGetBoolean("gctri");
  base       = ajAcdGetString("base");
  accid      = ajAcdGetString("accid");
  mult       = ajAcdGetGraphxy("graph");
  
  params.window       = window;
  params.slide        = slide;
  if(cumulative){
    params.cumulative = 1;
  }else{
    params.cumulative = 0;
  }
  if(gc3){
    params.gc3        = 1;
  }else{
    params.gc3        = 0;
  }
  params.base         = ajCharNewS(base);
  params.output       = "f";

  while(ajSeqallNext(seqall,&seq)){

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !strlen(ajCharNewS(accid))){
      inseq = getGenbank(seq,ajSeqGetFeat(seq));
      ajStrAssignS(&accid,ajSeqGetAccS(seq));
    }else{
      if(!strlen(ajCharNewS(accid))){
        fprintf(stderr,"Sequence does not have features\n");
        fprintf(stderr,"Proceeding with sequence accession ID\n");
        ajStrAssignS(&inseq,ajSeqGetAccS(seq));
        ajStrAssignS(&accid,ajSeqGetAccS(seq));
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


    if(soap_call_ns1__geneskew(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      ajStrAppendC(&filename,".csv");
      if(get_file(jobid,ajCharNewS(filename))==0){
        AjPStr title = NULL;
        ajStrAppendC(&title, argv[0]);
        ajStrAppendC(&title, " of ");
        ajStrAppendS(&title, accid);
        gpp.title = ajStrNewS(title);
        gpp.xlab = ajStrNewC("location");
        gpp.ylab = ajStrNewC("GC skew");
        ajStrDel(&title);
        if(gPlotFile(filename, mult, &gpp) == 1)
          fprintf(stderr,"Error allocating\n");
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
