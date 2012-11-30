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
	embInitPV("gb2", argc, argv, "GEMBASSY", "1.0.0");

	struct soap soap;

	AjPSeqall seqall;
	AjPSeq    seq;
	AjPStr    inseq    = NULL;
	AjPStr    accid    = NULL;
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

		if(ajSeqGetFeat(seq) && !ajStrGetLen(accid)){
			inseq = getGenbank(seq);
			ajStrAssignS(&accid, ajSeqGetAccS(seq));
		}else{
			if(!valID(ajCharNewS(accid))){
				fprintf(stderr, "Invalid accession ID, exiting\n");
				return 1;
			}
			if(!ajStrGetLen(accid)){
				fprintf(stderr, "Sequence does not have features\n");
				fprintf(stderr, "Proceeding with sequence accession ID\n");
				ajStrAssignS(&accid, ajSeqGetAccS(seq));
			}
			ajStrAssignS(&inseq, accid);
		}

		char* in0;
		in0 = ajCharNewS(inseq);

		if(soap_call_ns1__B2(
					&soap, NULL, NULL,
					in0, &result
				    ) == SOAP_OK){
			if(show)
				ajFmtPrint("Sequence: %S B1: %S\n",
						accid, ajStrNewC(result));
			else
				ajFmtPrintF(outf, "Sequence: %S B1: %S\n",
						accid, ajStrNewC(result));
		}else{
			soap_print_fault(&soap, stderr);
		}

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	if(outf)
		ajFileClose(&outf);

	ajSeqallDel(&seqall);
	ajSeqDel(&seq);
	ajStrDel(&inseq);

	embExit();
	return 0;
}
