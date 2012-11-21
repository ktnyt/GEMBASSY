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
  embInitPV("ggenome_map3", argc, argv, "GEMBASSY", "1.0.0");
  
  struct soap soap;
  struct ns1__genome_USCOREmap3InputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  ajint     width    = 0;
  ajint     height   = 0;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  char*     jobid;
  
  seqall   = ajAcdGetSeqall("sequence");
  width    = ajAcdGetInt("width");
  height   = ajAcdGetInt("height");
  filename = ajAcdGetString("filename");
  accid    = ajAcdGetString("accid");

  params.width  = width;
  params.height = height;
  params.gmap   = 0;
  
  while(ajSeqallNext(seqall, &seq)){

    soap_init(&soap);

    soap.send_timeout = 0; 
    soap.recv_timeout = 0;

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !ajStrGetLen(accid)){
      inseq = getGenbank(seq);
      ajStrAssignS(&accid,ajSeqGetAccS(seq));
    }else{
      if(!ajStrGetLen(accid)){
        fprintf(stderr,"Sequence does not have features\n");
        fprintf(stderr,"Proceeding with sequence accession ID\n");
        ajStrAssignS(&accid,ajSeqGetAccS(seq));
      }
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr,"Invalid accession ID, exiting");
          return 1;
      }
      ajStrAssignS(&inseq,accid);
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    if(soap_call_ns1__genome_USCOREmap3(
					&soap, NULL, NULL,
					in0, &params, &jobid
					) == SOAP_OK){
      AjPStr tmp = ajStrNew();
      ajStrFromLong(&tmp, ajSeqallGetCount(seqall));
      ajStrInsertC(&tmp, 0, ".");
      ajStrAppendC(&tmp, ".png");
      if(!ajStrExchangeCS(&filename, ".png", tmp)){
        ajStrAppendC(&filename, ".");
      }

      if(get_file(jobid,ajCharNewS(filename))){
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
