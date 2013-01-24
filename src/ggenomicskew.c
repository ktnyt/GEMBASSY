/******************************************************************************
** @source ggenomicskew
**
** Calculates the GC skew in different regions of the given genome
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




/* @prog ggenomicskew *********************************************************
**
** Calculates the GC skew in different regions of the given genome
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("ggenomicskew", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__genomicskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  ajint	    divide = 0;
  AjBool    at     = 0;
  AjPStr    accid  = NULL;
  AjPStr    string  = NULL;

  char *in0;
  char *result;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;
  AjPPStr     names = NULL;

  ajint i;

  seqall = ajAcdGetSeqall("sequence");
  divide = ajAcdGetInt("divide");
  at     = ajAcdGetBoolean("at");
  accid  = ajAcdGetString("accid");

  plot = ajAcdGetToggle("plot");

  if(!plot)
    outf = ajAcdGetOutfile("outfile");
  else
    mult = ajAcdGetGraphxy("graph");

  params.divide = divide;
  params.at     = 0;
  params.output = "f";

  if(at)
    params.at = 1;

  while (ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      if(!gFormatGenbank(seq, &inseq) && !ajStrGetLen(accid))
	{
	  ajFmtError("Sequence does not have features\n");
	  ajFmtError("Proceeding with sequence accession ID\n");
	  ajStrAssignS(&accid, ajSeqGetAccS(seq));
	}

      if(ajStrGetLen(accid))
	{
	  if(!gValID(accid))
	    {
	      ajFmtError("Invalid accession ID, exiting\n");
	      embExitBad();
	    }
	  ajStrAssignS(&inseq, accid);
	}

      if(!ajStrGetLen(accid))
	ajStrAssignS(&accid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if (soap_call_ns1__genomicskew(
	                            &soap,
				     NULL,
				     NULL,
				     in0,
				    &params,
				    &result
                                    ) == SOAP_OK) {
      if(plot)
	{
	  if((names = (AjPPStr)malloc(sizeof(AjPStr) * 5)) == NULL) {
	    ajFmtError("Error in memory allocation, exiting\n");
	    embExitBad();
	  }
	
	  names[0] = NULL;
	  names[1] = ajStrNewC("whole genome");
	  names[2] = ajStrNewC("coding region");
	  names[3] = ajStrNewC("intergenic region");
	  names[4] = ajStrNewC("codon third position");

	  ajStrAppendC(&title, argv[0]);
	  ajStrAppendC(&title, " of ");
	  ajStrAppendS(&title, accid);

	  gpp.title = ajStrNewS(title);
	  gpp.xlab = ajStrNewC("location");
	  gpp.ylab = ajStrNewC("GC skew");
	  gpp.names = names;

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

	  i = 0;
	  while(names[i])
	    {
	      AJFREE(names[i]);
	      ++i;
	    }

	  AJFREE(names);

	  ajStrDel(&title);
	  ajStrDel(&(gpp.title));
	  ajStrDel(&(gpp.xlab));
	  ajStrDel(&(gpp.ylab));
	  ajFilebuffDel(&buff);
	}
      else
	{
	  ajFmtPrintF(outf, "Sequence: %S\n", accid);
	  if(!gFileOutURLC(result, &outf))
	    {
	      ajFmtError("File downloading error\n");
	      embExitBad();
	    }
	}
    } else {
      soap_print_fault(&soap, stderr);
    }

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);

    ajStrDel(&inseq);
  }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  embExit();
  return 0;
}
