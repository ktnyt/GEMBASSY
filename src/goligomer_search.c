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
	embInitPV("goligomer_search", argc, argv, "GEMBASSY", "1.0.0");

	struct soap soap;
	struct ns1__oligomer_USCOREsearchInputParams params;

	AjPSeqall seqall;
	AjPSeq    seq;
	AjPStr    inseq    = NULL;
	AjPStr    oligomer = NULL;
	AjPStr    return_  = NULL;
	AjPStr    accid    = NULL;
	char*     result;

	AjBool  show = 0;
	AjPFile outf = NULL;

	seqall   = ajAcdGetSeqall("sequence");
	oligomer = ajAcdGetString("oligomer");
	return_  = ajAcdGetString("return");
	accid    = ajAcdGetString("accid");

	show = ajAcdGetToggle("show");
	outf = ajAcdGetOutfile("outfile");

	params.return_ = ajCharNewS(return_);

	while(ajSeqallNext(seqall, &seq)){  

		soap_init(&soap);

		soap.send_timeout = 0;
		soap.recv_timeout = 0;

		inseq = NULL;

		ajStrAppendC(&inseq, ">");
		ajStrAppendS(&inseq, ajSeqGetNameS(seq));
		ajStrAppendC(&inseq, "\n");
		ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

		if(!ajStrGetLen(accid))
			ajStrAssignS(&accid, ajSeqGetAccS(seq));

		char* in0;
		char* in1;
		in0 = ajCharNewS(inseq);
		in1 = ajCharNewS(oligomer);

		if(soap_call_ns1__oligomer_USCOREsearch(
					&soap, NULL, NULL,
					in0, in1, &params, &result
					) == SOAP_OK){
			AjPStr tmp = ajStrNewC(result);
			AjPStr parse = ajStrNew();
			AjPStrTok handle = NULL;
			ajStrExchangeCC(&tmp, "<", "\n");
			ajStrExchangeCC(&tmp, ">", "\n");
			handle = ajStrTokenNewC(tmp, "\n");
			if(show)
				ajFmtPrint("Sequence: %S Oligomer: %S\n",
						accid, oligomer);
			else
				ajFmtPrintF(outf,"Sequence: %S Oligomer: %S\n",
						accid, oligomer);
			while(ajStrTokenNextParse(&handle, &parse)){
				if(ajStrIsInt(parse))
					if(show)
						ajFmtPrint("%S\n", parse);
					else
						ajFmtPrintF(outf, "%S\n", parse);
			}
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
