#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInitPV("gentrez",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  
  char* in0;
  char* in1;
  char* _result;
  
  soap_init(&soap);

  in0 = ajCharNewS(ajAcdGetString("db"));
  in1 = ajCharNewS(ajAcdGetString("query"));

  if(soap_call_ns1__entrez(
			   &soap, NULL, NULL,
			   in0, in1, &_result
			   ) == SOAP_OK){
    fprintf(stdout, "%s\n", _result);
  }else{
    soap_print_fault(&soap,stderr);
  }
  
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  
  embExit();
  return 0;
}
