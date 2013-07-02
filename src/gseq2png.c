/******************************************************************************
** @source gseq2png
**
** Converts a sequence to PNG image
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




/* @prog gseq2png *************************************************************
**
** Converts a sequence to PNG image
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gseq2png", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;
  struct ns1__seq2pngInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    seqid    = NULL;
  ajint	    width    = 0;
  ajint	    window   = 0;
  AjPFile   outf     = NULL;
  AjPStr    filename = NULL;
  AjPStr    outfname = NULL;
  AjPStr    tempname = NULL;
  AjPStr    appname  = NULL;
  AjPStr    convert  = NULL;
  AjPStr    format   = NULL;

  ajint i;

  char *in0;
  char *result;

  seqall   = ajAcdGetSeqall("sequence");
  window   = ajAcdGetInt("window");
  width    = ajAcdGetInt("width");
  filename = ajAcdGetString("goutfile");
  format   = ajAcdGetString("format");

  appname = ajStrNewC("convert");

  params.window = window;
  params.width  = width;
  params.output = "g";

  i = 0;

  while(ajSeqallNext(seqall, &seq))
    {
      soap_init(&soap);

      inseq = NULL;

      ajStrAppendC(&inseq, ">");
      ajStrAppendS(&inseq, ajSeqGetNameS(seq));
      ajStrAppendC(&inseq, "\n");
      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__seq2png(
			       &soap,
                                NULL,
                                NULL,
			        in0,
                               &params,
                               &result
			       ) == SOAP_OK)
        {
          ++i;

          outfname = ajStrNewS(ajFmtStr("%S.%d.%S",
                                        filename,
                                        i,
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

      soap_destroy(&soap);
      soap_end(&soap);
      soap_done(&soap);

      AJFREE(in0);

      ajStrDel(&inseq);
    }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  ajStrDel(&filename);

  embExit();

  return 0;
}
