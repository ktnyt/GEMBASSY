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
  embInitPV("gseq2png", argc, argv, "GEMBASSY", "1.0.0");
  
  struct soap soap;
  struct ns1__seq2pngInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  ajint     width    = 0;
  ajint     window   = 0;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  AjBool    output   = 0;
  char*     jobid;
  char*     _result;
  
  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  width      = ajAcdGetInt("width");
  filename   = ajAcdGetString("filename");
  output     = ajAcdGetBoolean("show");
  accid      = ajAcdGetString("accid");
  
  params.window = window;
  params.width  = width;
  params.output = "g";

  while(ajSeqallNext(seqall, &seq)){

    soap_init(&soap);

    inseq = NULL;

    ajStrAppendC(&inseq, ">");
    ajStrAppendS(&inseq, ajSeqGetNameS(seq));
    ajStrAppendC(&inseq, "\n");
    ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

    if(!ajStrGetLen(accid))
      ajStrAssignS(&accid, ajSeqGetAccS(seq));

    char* in0;
    in0 = ajCharNewS(inseq);

    if(soap_call_ns1__seq2png(
			      &soap, NULL, NULL,
			      in0, &params, &jobid
			      ) == SOAP_OK){
      if(ajStrCmpC(filename, "gseq2png.[accession].png") == 0){
        ajStrAssignC(&filename, argv[0]);
        ajStrAppendC(&filename, ".");
        ajStrAppendS(&filename, accid);
        ajStrAppendC(&filename, ".png");
      }else{
        ajStrInsertC(&filename, -5, ".");
        ajStrInsertS(&filename, -5, accid);
      }
      if(get_file(jobid, ajCharNewS(filename))==0){
        if(output)
          if(display_png(ajCharNewS(filename), argv[0], ajCharNewS(accid)))
            fprintf(stderr, "Error in X11 displaying\n");
      }else{
        fprintf(stderr, "Retrieval unsuccessful\n");
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
