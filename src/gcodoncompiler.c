/******************************************************************************
** @source gcodoncompiler
**
** Calculate various kinds of amino acid and codon usage data
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




/* @prog gcodoncompiler ******************************************************
**
** Calculate various kinds of amino acid and codon usage data
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("gcodoncompiler", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;
  struct ns1__codon_USCOREcompilerInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    seqid      = NULL;
  AjPStr    id         = NULL;
  AjBool    translate  = 0;
  AjBool    startcodon = 0;
  AjBool    stopcodon  = 0;
  AjPStr    delkey     = NULL;
  AjPStr    data       = NULL;
  AjBool    accid      = ajFalse;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  id         = ajAcdGetString("id");
  translate  = ajAcdGetBoolean("translate");
  startcodon = ajAcdGetBoolean("startcodon");
  stopcodon  = ajAcdGetBoolean("stopcodon");
  delkey     = ajAcdGetString("delkey");
  data       = ajAcdGetListSingle("data");
  accid      = ajAcdGetBoolean("accid");

  outf = ajAcdGetOutfile("outfile");

  params.translate  = 0;
  params.startcodon = 0;
  params.stopcodon  = 0;
  params.del_USCOREkey = ajCharNewS(delkey);
  params.data   = ajCharNewS(data);
  params.output = "f";

  if(translate)
    params.translate = 1;
  if(startcodon)
    params.startcodon = 1;
  if(stopcodon)
    params.stopcodon = 1;

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

      if(soap_call_ns1__codon_USCOREcompiler(
	                                    &soap,
					     NULL,
					     NULL,
					     in0,
					    &params,
					    &result
                                            ) == SOAP_OK)
	{
	  ajFmtPrintF(outf, "Sequence: %S\n", seqid);
	  if(!gFileOutURLC(result, &outf))
	    {
              ajDie("File downloading error from:\n%s\n", result);
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

  AJFREE(params.del_USCOREkey);
  AJFREE(params.data);

  ajStrDel(&delkey);
  ajStrDel(&data);

  embExit();

  return 0;
}
