/******************************************************************************
** @source gentrez
**
** Search NCBI Entrez in G-language Shell
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.1   Revision 1
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/


#include "emboss.h"
#include "soapH.h"
#include "GLANGSoapBinding.nsmap"
#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "glibs.h"




/* @prog gentrez **************************************************************
**
** Search NCBI Entrez in G-language Shell
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gentrez", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;

  AjPStr database = NULL;
  AjPStr query    = NULL;

  char *in0;
  char *in1;
  char *result;

  AjPFile outf = NULL;

  database = ajAcdGetString("database");
  query    = ajAcdGetString("query");
  outf     = ajAcdGetOutfile("outfile");

  in0 = ajCharNewS(database);
  in1 = ajCharNewS(query);

  soap_init(&soap);

  if(soap_call_ns1__entrez(
                          &soap,
			   NULL,
			   NULL,
			   in0,
			   in1,
			  &result
			  ) == SOAP_OK)
    {
      if(result)
        {
          ajFmtPrintF(outf, "%s", result);
        }
      else
        {
          ajFmtPrintF(outf, "No results found.\n");
        }
    }
  else
    {
      soap_print_fault(&soap, stderr);
    }

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajFileClose(&outf);

  AJFREE(in0);
  AJFREE(in1);

  ajStrDel(&database);
  ajStrDel(&query);

  embExit();

  return 0;
}
