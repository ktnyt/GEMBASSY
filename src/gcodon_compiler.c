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
  embInitPV("gcodon_compiler", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__codon_USCOREcompilerInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    id         = NULL;
  AjBool    translate  = 0;
  AjBool    startcodon = 0;
  AjBool    stopcodon  = 0;
  AjPStr    delkey     = NULL;
  AjPStr    data       = NULL;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     jobid;
  
  seqall     = ajAcdGetSeqall("sequence");
  id         = ajAcdGetString("id");
  translate  = ajAcdGetBoolean("translate");
  startcodon = ajAcdGetBoolean("startcodon");
  stopcodon  = ajAcdGetBoolean("stopcodon");
  delkey     = ajAcdGetString("delkey");
  data       = ajAcdGetString("data");
  filename   = ajAcdGetString("filename");
  accid      = ajAcdGetString("accid");

  if(translate){
    params.translate   = 1;
  }else{
    params.translate   = 0;
  }
  if(startcodon){
    params.startcodon  = 1;
  }else{
    params.startcodon  = 0;
  }
  if(stopcodon){
    params.stopcodon   = 1;
  }else{
    params.stopcodon   = 0;
  }
  params.del_USCOREkey = ajCharNewS(delkey);
  params.data          = ajCharNewS(data);
  params.output        = "f";

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

    if(soap_call_ns1__codon_USCOREcompiler(
					   &soap, NULL, NULL,
					   in0, &params, &jobid
					   ) == SOAP_OK){
      if(ajStrCmpC(filename, "gcodon_compiler.[accession].csv") == 0){
        ajStrAssignC(&filename, argv[0]);
        ajStrAppendC(&filename, ".");
        ajStrAppendS(&filename, accid);
        ajStrAppendC(&filename, ".csv");
      }else{
        ajStrInsertC(&filename, -5, ".");
        ajStrInsertS(&filename, -5, accid);
      }
      if(get_file(jobid, ajCharNewS(filename))){
        fprintf(stderr, "Retrieval unsuccessful\n");
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
