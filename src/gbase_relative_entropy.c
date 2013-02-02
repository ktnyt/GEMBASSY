/******************************************************************************
** @source gbase_relative_entropy
**
** Calculates and graphs the sequence conservation using Kullback-Leibler
** divergence (relative entropy)
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




/* @prog gbase_relative_entropy **********************************************
**
** Calculates and graphs the sequence conservation using Kullback-Leibler
** divergence (relative entropy)
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gbase_relative_entropy", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__base_USCORErelative_USCOREentropyInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  AjPStr    position = NULL;
  ajint	    PatLen = 0;
  ajint	    upstream = 0;
  ajint	    downstream = 0;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  position   = ajAcdGetString("position");
  PatLen     = ajAcdGetInt("patlen");
  upstream   = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  accid      = ajAcdGetString("accid");

  plot = ajAcdGetToggle("plot");

  if(!plot)
    outf = ajAcdGetOutfile("outfile");
  else
    mult = ajAcdGetGraphxy("graph");

  params.position   = ajCharNewS(position);
  params.PatLength  = PatLen;
  params.upstream   = upstream;
  params.downstream = downstream;
  params.output     = "f";

  while (ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      if(!gFormatGenbank(seq, &inseq) && !ajStrGetLen(accid))
	{
	  ajFmtError("Sequence does not have features\n");
	  ajFmtError("Proceeding with sequence accession ID\n");
	  ajStrAssignS(&accid, ajSeqGetAccS(seq));

          if(!ajStrGetLen(accid))
            {
              ajStrAssignS(&accid, ajSeqGetNameS(seq));

              if(!ajStrGetLen(accid))
                {
                  ajFmtError("No header information\n");
                  embExitBad();
                }
            }
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

      if(soap_call_ns1__base_USCORErelative_USCOREentropy(
	                                                 &soap,
							  NULL,
							  NULL,
							  in0,
							 &params,
							 &result
                                                         ) == SOAP_OK)
	{
	  if(plot)
	    {
	      ajStrAppendC(&title, argv[0]);
	      ajStrAppendC(&title, " of ");
	      ajStrAppendS(&title, accid);

	      gpp.title = ajStrNewS(title);
	      gpp.xlab = ajStrNewC("position");
	      gpp.ylab = ajStrNewC("relative entropy");

	      if(!gFilebuffURLC(result, &buff))
		{
		  ajFmtError("File downloading error\n");
		  embExitBad();
		}
	      
	      if(gPlotFilebuff(buff, mult, &gpp))
		{
		  ajFmtError( "Error in plotting\n");
		  embExitBad();
		}

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
	}
      else
	{
	  soap_print_fault(&soap, stderr);
	}

      soap_destroy(&soap);
      soap_end(&soap);
      soap_done(&soap);

      AJFREE(in0);

      ajStrDel(&inseq);
    }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&accid);

  AJFREE(params.position);

  ajStrDel(&position);

  embExit();

  return 0;
}
