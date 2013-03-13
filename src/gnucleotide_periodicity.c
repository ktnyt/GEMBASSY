/******************************************************************************
** @source gnucleotide_periodicity
**
** Checks the periodicity of certain oligonucleotides
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




/* @prog gnucleotide_periodicity *********************************************
**
** Checks the periodicity of certain oligonucleotides
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gnucleotide_periodicity", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__nucleotide_USCOREperiodicityInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    seqid      = NULL;
  ajint	    window     = 0;
  AjPStr    nucleotide = 0;
  AjBool    accid      = ajFalse;
  AjBool    output     = 0;

  char *in0;
  char *result;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  nucleotide = ajAcdGetString("nucleotide");
  accid      = ajAcdGetBoolean("accid");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.window     = window;
  params.nucleotide = ajCharNewS(nucleotide);
  params.output     = "f";

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      if(!ajStrGetLen(seqid))
        ajStrAssignS(&seqid, ajSeqGetNameS(seq));

      if(!ajStrGetLen(seqid))
        {
          ajFmtError("No header information\n");
          embExitBad();
        }

      if(accid || !gFormatGenbank(seq, &inseq))
        {
          if(!accid)
            ajFmtError("Sequence does not have features\n"
                       "Proceeding with sequence accession ID\n");

          if(!gValID(seqid))
            {
              ajFmtError("Invalid accession ID, exiting\n");
              embExitBad();
            }

          ajStrAssignS(&inseq, seqid);
        }

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__nucleotide_USCOREperiodicity(
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
              title = ajStrNew();

              ajStrAppendC(&title, argv[0]);
              ajStrAppendC(&title, " of ");
              ajStrAppendS(&title, seqid);

              gpp.title = ajStrNewS(title);
              gpp.xlab = ajStrNewC("position");
              gpp.ylab = ajStrNewC("periodicity");

              if(!gFilebuffURLC(result, &buff))
                {
                  ajFmtError("File downloading error from:\n%s\n", result);
                  embExitBad();
                }

              if(!gPlotFilebuff(buff, mult, &gpp))
                {
                  ajFmtError("Error in plotting\n");
                  embExitBad();
                }

              AJFREE(gpp.title);
              AJFREE(gpp.xlab);
              AJFREE(gpp.ylab);

              ajStrDel(&title);
            }
          else
            {
              ajFmtPrintF(outf, "Sequence: %S\n", seqid);
              if(!gFileOutURLC(result, &outf))
                {
                  ajFmtError("File downloading error from:\n%s\n", result);
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

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
