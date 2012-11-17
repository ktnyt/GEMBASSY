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
  embInitPV("gsignature", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__signatureInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     wordlength = 0;
  AjBool    bothstrand = 0;
  AjBool    oe         = 0;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     jobid;

  seqall     = ajAcdGetSeqall("sequence");
  wordlength = ajAcdGetInt("wordlength");
  bothstrand = ajAcdGetBoolean("bothstrand");
  oe         = ajAcdGetBoolean("oe");
  filename   = ajAcdGetString("filename");
  accid      = ajAcdGetString("accid");
  
  params.wordlength = wordlength;
  if(bothstrand){
    params.bothstrand = 1;
  }else{
    params.bothstrand = 0;
  }
  if(oe){
    params.oe = 1;
  }else{
    params.oe = 0;
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
        ajStrAssignS(&accid, ajSeqGetAccS(seq));
      }
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr, "Invalid accession ID, exiting");
          return 1;
      }
      ajStrAssignS(&inseq, accid);
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    if(soap_call_ns1__signature(
				&soap, NULL, NULL,
				in0, &params, &jobid
				) == SOAP_OK){
      if(ajStrCmpC(filename, "gsignature.[accession].csv") == 0){
        ajStrAssignC(&filename, argv[0]);
        ajStrAppendC(&filename, ".");
        ajStrAppendS(&filename, accid);
        ajStrAppendC(&filename, ".csv");
      }else{
        ajStrInsertC(&filename, -5, ".");
        ajStrInsertS(&filename, -5, accid);
      }
      if(get_file(jobid, ajCharNewS(filename))){
	fprintf(stderr,"Retrieval unsuccessful\n");
      }
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
