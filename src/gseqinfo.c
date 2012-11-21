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
  embInitPV("gseqinfo", argc, argv, "GEMBASSY", "0.0.1");
  
  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq    = NULL;
  AjPStr    inseq  = NULL;
  AjPStr    accid  = NULL;
  int       n;
  char*     result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  accid  = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

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

    if(soap_call_ns1__seqinfo(
			      &soap, NULL, NULL,
			      in0, &result
			      ) == SOAP_OK){
      AjPStr tmp = ajStrNewC(result);
      AjPStr parse = ajStrNew();
      AjPStr numA = NULL;
      AjPStr numT = NULL;
      AjPStr numG = NULL;
      AjPStr numC = NULL;
      AjPStrTok handle = NULL;
      ajStrExchangeCC(&tmp, "<", "\n");
      ajStrExchangeCC(&tmp, ">", "\n");
      handle = ajStrTokenNewC(tmp, "\n");
      while(ajStrTokenNextParse(&handle, &parse)){
        if(ajStrIsInt(parse))
	  if(!numA)
	    numA = ajStrNewS(parse);
	  else if(!numT)
	    numT = ajStrNewS(parse);
	  else if(!numG)
	    numG = ajStrNewS(parse);
	  else if(!numC)
	    numC = ajStrNewS(parse);
      }
      if(show)
        ajFmtPrint("Sequence: %S A: %S T: %S G: %S C: %S\n",
                   ajSeqGetAccS(seq), numA, numT, numG, numC);
      else
	ajFmtPrintF(outf,"Sequence: %S A: %S T: %S G: %S C: %S\n",
		    ajSeqGetAccS(seq), numA, numT, numG, numC);
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
  
  embExit();
  return 0;
}
