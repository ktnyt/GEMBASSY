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
  embInitPV("gdnawalk", argc, argv, "GEMBASSY", "1.0.0");
  
  struct soap soap;
  struct ns1__dnawalkInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  AjBool    show     = 0;
  char*     result;
  
  seqall   = ajAcdGetSeqall("sequence");
  filename = ajAcdGetString("filename");
  show    = ajAcdGetToggle("show");
  accid    = ajAcdGetString("accid");

  params.gmap = 0;

  while(ajSeqallNext(seqall, &seq)){

    soap_init(&soap);

    soap.send_timeout = 0; 
    soap.recv_timeout = 0;

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

    if(soap_call_ns1__dnawalk(
			      &soap, NULL, NULL,
			      in0, &params, &result
			      ) == SOAP_OK){
      AjPStr tmp = ajStrNew();
      ajStrFromLong(&tmp, ajSeqallGetCount(seqall));
      ajStrInsertC(&tmp, 0, ".");
      ajStrAppendC(&tmp, ".png");
      if(!ajStrExchangeCS(&filename, ".png", tmp)){
        ajStrAppendC(&filename, ".");
      }

      if(get_file(result, ajCharNewS(filename))==0){
        if(show)
          if(display_png(ajCharNewS(filename), argv[0], ajCharNewS(accid)))
            fprintf(stderr, "Error in X11 displaying\n");
      }else{
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
