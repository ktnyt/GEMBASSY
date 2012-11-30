#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

char *_upper(char *s){
	char *p;
	for(p = s; *p; p++){
		*p = toupper(*p);
	}
	return(s);
}

int main(int argc, char *argv[]){
	embInitPV("gshuffleseq", argc, argv, "GEMBASSY", "1.0.0");

	struct soap soap;
	struct ns1__shuffleseqInputParams params;

	AjPSeqall seqall;
	AjPSeqout seqout;
	AjPSeq    seq;
	AjPStr    inseq = NULL;
	ajint     k     = 0;
	char*     result;

	seqall = ajAcdGetSeqall("sequence");
	k      = ajAcdGetInt("k");
	seqout = ajAcdGetSeqout("outseq");

	params.k = k;

	while(ajSeqallNext(seqall, &seq)){

		soap_init(&soap);

		inseq = NULL;

		ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

		char* in0;
		in0 = ajCharNewS(inseq);

		if(soap_call_ns1__shuffleseq(
					&soap, NULL, NULL,
					in0, &params, &result
					) == SOAP_OK){
			ajCharFmtUpper(result);
			ajSeqAssignSeqC(seq, result);
			ajSeqoutWriteSeq(seqout, seq);
		}else{
			soap_print_fault(&soap, stderr);
		}

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	ajSeqoutClose(seqout);
	ajSeqoutDel(&seqout);
	ajSeqallDel(&seqall);
	ajSeqDel(&seq);
	ajStrDel(&inseq);

	embExit();
	return 0;
}
