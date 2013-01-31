/******************************************************************************
** @source ggcsi
**
** GC Skew Index: an index for strand-specific mutational bias
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




/* @prog ggcsi ****************************************************************
**
** GC Skew Index: an index for strand-specific mutational bias
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("ggcsi", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__gcsiInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq   = NULL;
  ajint	    window  = 0;
  AjBool    at      = 0;
  AjBool    purine  = 0;
  AjBool    keto    = 0;
  AjBool    pval    = 0;
  AjPStr    version = NULL;
  AjPStr    accid   = NULL;
  AjPStr    tmp     = NULL;
  AjPStr    parse   = NULL;
  AjPStr    gcsi    = NULL;
  AjPStr    sa      = NULL;
  AjPStr    dist    = NULL;
  AjPStr    z       = NULL;
  AjPStr    p       = NULL;
  AjPStrTok handle  = NULL;

  char *in0;
  char *result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall  = ajAcdGetSeqall("sequence");
  window  = ajAcdGetInt("window");
  at      = ajAcdGetBoolean("at");
  purine  = ajAcdGetBoolean("purine");
  keto    = ajAcdGetBoolean("keto");
  pval    = ajAcdGetBoolean("pval");
  version = ajAcdGetListSingle("gcsi");
  accid   = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");

  if(show)
    outf = ajAcdGetOutfile("outfile");

  params.window = window;
  params.at     = 0;
  params.purine = 0;
  params.keto   = 0;
  params.p      = 0;
  ajStrToInt(version, &(params.version));

  if(at)
    params.at = 1;
  if(purine)
    params.purine = 1;
  if(keto)
    params.keto = 1;
  if(pval)
    params.p = 1;

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
	      ajStrAssignS(&inseq, accid);
	    }
	}

      if(!ajStrGetLen(accid))
	ajStrAssignS(&accid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if (soap_call_ns1__gcsi(
	                      &soap,
                              NULL,
                              NULL,
                              in0,
                             &params,
                             &result
                            ) == SOAP_OK)
	{
	  tmp   = ajStrNew();
	  parse = ajStrNew();
	  gcsi  = ajStrNew();
	  sa    = ajStrNew();
	  dist  = ajStrNew();
	  z     = ajStrNew();
	  p     = ajStrNew();

	  ajStrAssignC(&tmp, result);

	  ajStrExchangeCC(&tmp, "<", "\n");
	  ajStrExchangeCC(&tmp, ">", "\n");

	  handle = ajStrTokenNewC(tmp, "\n");

	  while (ajStrTokenNextParse(&handle, &parse))
	    {
	      if (ajStrIsFloat(parse))
		{
		  if(!ajStrGetLen(gcsi))
		    ajStrAssignS(&gcsi, parse);
		  else if(!ajStrGetLen(sa))
		    ajStrAssignS(&sa, parse);
		  else if(!ajStrGetLen(dist))
		    ajStrAssignS(&dist, parse);
		  else if(!ajStrGetLen(z))
		    ajStrAssignS(&z, parse);
		  else if(!ajStrGetLen(p))
		    ajStrAssignS(&p, parse);
		}
	    }

	  tmp = ajFmtStr("Sequence: %S GCSI: %S SA: %S DIST: %S",
			 accid, gcsi, sa, dist);

	  if(pval)
	    tmp = ajFmtStr("%S Z: %S P: %S", tmp, z, p);

	  if(show)
	    ajFmtPrint("%S\n", tmp);
	  else
	    ajFmtPrintF(outf, "%S\n", tmp);

	  ajStrDel(&tmp);
	  ajStrDel(&parse);
	  ajStrDel(&gcsi);
	  ajStrDel(&sa);
	  ajStrDel(&dist);
	  ajStrDel(&z);
	  ajStrDel(&p);
	}
      else
	{
	  soap_print_fault(&soap, stderr);
	}

      soap_destroy(&soap);
      soap_end(&soap);
      soap_done(&soap);
    }

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);

  embExit();
}
