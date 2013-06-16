/******************************************************************************
** @source gviewcds
**
** Displays a graph of nucleotide contents around start and stop codons
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




/* @prog gviewcds ************************************************************
**
** Displays a graph of nucleotide contents around start and stop codons
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gviewcds", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;
  struct ns1__view_USCOREcdsInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  AjPStr    seqid = NULL;
  ajint	    length = 0;
  ajint	    gap = 0;
  AjBool    accid = ajFalse;

  char *in0;
  char *result;
  int   i;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;    
  AjPPStr     names = NULL;

  seqall = ajAcdGetSeqall("sequence");
  length = ajAcdGetInt("length");
  gap    = ajAcdGetInt("gap");
  accid  = ajAcdGetBoolean("accid");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.length = length;
  params.gap    = gap;
  params.output = "f";

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      if(!ajStrGetLen(seqid))
        ajStrAssignS(&seqid, ajSeqGetNameS(seq));

      if(!ajStrGetLen(seqid))
        {
          ajWarn("No valid header information\n");
        }

      if(accid || !gFormatGenbank(seq, &inseq))
        {
          if(!accid)
            ajWarn("Sequence does not have features\n"
                   "Proceeding with sequence accession ID:%S\n", seqid);

          if(!gValID(seqid))
            {
              ajDie("Invalid accession ID:%S, exiting\n", seqid);
            }

          ajStrAssignS(&inseq, seqid);
        }

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__view_USCOREcds(
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
              if((names = (AjPPStr)malloc(sizeof(AjPStr) * 5)) == NULL) {
                ajDie("Error in memory allocation, exiting\n");
              }

              names[0] = NULL;
              names[1] = ajStrNewC("A");
              names[2] = ajStrNewC("T");
              names[3] = ajStrNewC("G");
              names[4] = ajStrNewC("C");

              title = ajStrNew();

              ajStrAppendC(&title, argv[0]);
              ajStrAppendC(&title, " of ");
              ajStrAppendS(&title, seqid);

              gpp.title = ajStrNewS(title);
              gpp.xlab = ajStrNewC("position");
              gpp.ylab = ajStrNewC("percentage");
              gpp.names = names;

              if(!gFilebuffURLC(result, &buff))
                {
                  ajDie("File downloading error from:\n%s\n", result);
                }

              if(!gPlotFilebuff(buff, mult, &gpp))
                {
                  ajDie("Error in plotting\n");
                }

              i = 0;
              while(names[i])
                {
                  AJFREE(names[i]);
                  ++i;
                }

              AJFREE(names);

              AJFREE(gpp.title);
              AJFREE(gpp.xlab);
              AJFREE(gpp.ylab);
              ajStrDel(&title);
              ajFilebuffDel(&buff);
            }
          else
            {
              ajFmtPrintF(outf, "Sequence: %S\n", seqid);
              if(!gFileOutURLC(result, &outf))
                {
                  ajDie("File downloading error from:\n%s\n", result);
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
