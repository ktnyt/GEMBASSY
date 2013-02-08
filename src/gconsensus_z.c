/******************************************************************************
** @source gconsensus_z
**
** Calculate the consensus in givin array of sequences
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.0   First release
** @modified 2012/1/20  Hidetoshi Itaya  Created!
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
#include "../include/gfile.h"
#include "../include/gplot.h"




/* @prog gconsensus_z *********************************************************
**
** Calculate the consensus in givin array of sequences
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gconsensus_z", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__consensus_USCOREzInputParams params;
  struct arrayIn  array_seq;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  ajint		  high  = 0;
  double	  low   = 0;

  char *result;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;

  ajint i;
  ajint size = 0;
  char **ptr = NULL;

  seqall = ajAcdGetSeqall("sequence");
  high   = ajAcdGetInt("high");
  low    = ajAcdGetFloat("low");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.high   = high;
  params.low    = low;
  params.output = "f";

  array_seq.__ptr = NULL;
  array_seq.__size = 0;

  ptr = (char**)malloc(sizeof(char*));

  if(!ptr)
    {
      ajFmtError("Error in allocation\n");
      embExitBad();
    }

  while(ajSeqallNext(seqall, &seq))
    {
      ptr = (char**)realloc(ptr, sizeof(char*) * (size + 1));

      if(!ptr)
	{
	  ajFmtError("Error in allocation\n");
	  embExitBad();
	}

      *(ptr + size) = ajCharNewS(ajSeqGetSeqS(seq));

      ++size;
    }

  if(size < 2)
    {
      AJFREE(*ptr);
      AJFREE(ptr);

      ajFmtError("File only has one sequence. Please input more than two.\n");
      embExitBad();
    }

  array_seq.__ptr = ptr;
  array_seq.__size = size;

  soap_init(&soap);

  if(soap_call_ns1__consensus_USCOREz(
                                     &soap,
				      NULL,
				      NULL,
				     &array_seq,
				     &params,
				     &result
                                     ) == SOAP_OK)
    {
      if(plot)
	{
	  ajStrAppendC(&title, argv[0]);

	  gpp.title = ajStrNewS(title);
	  gpp.xlab = ajStrNewC("position");
	  gpp.ylab = ajStrNewC("consensus");

          if(!gFilebuffURLC(result, &buff))
            {
              ajFmtError("File downloading error\n");
              embExitBad();
            }

          if(!gPlotFilebuff(buff, mult, &gpp))
            {
              ajFmtError("Error in plotting\n");
              embExitBad();
            }

	  ajStrDel(&title);
	  ajStrDel(&(gpp.xlab));
	  ajStrDel(&(gpp.ylab));
	}
      else
	{
	  if(!gFileOutURLC(result, &outf))
	    {
              ajFmtError("File downloading error\n");
              embExitBad();
	    }
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

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  i = 0;

  while(i < size)
    {
      AJFREE(*(ptr + i));
      ++i;
    }

  AJFREE(ptr);

  embExit();

  return 0;
}
