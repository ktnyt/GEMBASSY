/******************************************************************************
** @source gnucleotide_periodicity
**
** Checks the periodicity of certain oligonucleotides
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




/* @prog gnucleotide_periodicity *********************************************
**
** Checks the periodicity of certain oligonucleotides
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gnucleotideperiodicity", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;
  struct ns1__nucleotide_USCOREperiodicityInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     window     = 0;
  AjPStr    nucleotide = NULL;
  AjBool    accid      = ajFalse;
  AjPStr    seqid      = NULL;
  AjPFile   outf       = NULL;
  AjPStr    filename   = NULL;
  AjPStr    outfname   = NULL;
  AjPStr    format     = NULL;

  char* in0;
  char* result;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  nucleotide = ajAcdGetString("nucleotide");
  filename   = ajAcdGetString("goutfile");
  accid      = ajAcdGetBoolean("accid");

  params.window = window;
  params.nucleotide = ajCharNewS(nucleotide);

  while(ajSeqallNext(seqall, &seq))
    {
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
      ajFmtPrint("%S", inseq);
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
          outfname = ajStrNewS(ajFmtStr("%S.%ld.%S",
                                        filename,
                                        ajSeqallGetCount(seqall),
                                        format));

          outf = ajFileNewOutNameS(outfname);

          if(!outf)
            {
              ajDie("File open error\n");
            }

          if(!ajStrMatchC(format, "png"))
            {
              if(!gHttpConvertC(result, &outf, ajStrNewC("png"), format))
                {
                  ajDie("File downloading error from:\n%s\n", result);
                }
              else
                {
                  ajFmtPrint("Created %S\n", outfname);
                }
            }
          else
            {
              if(!gHttpGetBinC(result, &outf))
                {
                  ajDie("File downloading error from:\n%s\n", result);
                }
              else
                {
                  ajFmtPrint("Created %S\n", outfname);
                }
            }

          ajStrDel(&outfname);
        }
      else
        {
          soap_print_fault(&soap, stderr);
        }

      ajStrDel(&inseq);
    }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&filename);

  embExit();

  return 0;
}
