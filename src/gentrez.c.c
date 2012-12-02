#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int 
main(int argc, char *argv[])
{
  embInitPV("gentrez", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;

  char           *in0;
  char           *in1;
  char           *result;

  AjBool	  show = 0;
  AjPFile	  outf = NULL;

  in0 = ajCharNewS(ajAcdGetString("db"));
  in1 = ajCharNewS(ajAcdGetString("query"));

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

  soap_init(&soap);

  if (soap_call_ns1__entrez(
			    &soap, NULL, NULL,
			    in0, in1, &result
			    ) == SOAP_OK) {
    if (show)
      ajFmtPrint("%S", ajStrNewC(result));
    else
      ajFmtPrintF(outf, "%S", ajStrNewC(result));
  } else {
    soap_print_fault(&soap, stderr);
  }

  if (outf)
    ajFileClose(&outf);

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  embExit();
  return 0;
}
