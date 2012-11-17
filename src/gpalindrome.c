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
  embInitPV("gpalindrome",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__palindromeInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  ajint     shortest = 0;
  ajint     loop     = 0;
  AjBool    gtmatch  = 0;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  char*     jobid;

  seqall   = ajAcdGetSeqall("sequence");
  shortest = ajAcdGetInt("shortest");
  loop     = ajAcdGetInt("loop");
  gtmatch  = ajAcdGetBoolean("gtmatch");
  filename = ajAcdGetString("filename");
  accid    = ajAcdGetString("accid");
  
  params.shortest = shortest;
  params.loop     = loop;
  params.gtmatch  = gtmatch;
  params.output   = "f";

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
          fprintf(stderr,"Invalid accession ID, exiting");
          return 1;
      }
      ajStrAssignS(&inseq, accid);
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    if(soap_call_ns1__palindrome(
				 &soap, NULL, NULL,
				 in0, &params, &jobid
				 ) == SOAP_OK){
      if(ajStrCmpC(filename, "gpalindrome.[accession].csv") == 0){
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
