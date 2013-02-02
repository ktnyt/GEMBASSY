/******************************************************************************
** @source grep_ori_ter
**
** Get the positions of replication origin and terminus
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




/* @prog grep_ori_ter *********************************************************
**
** Get the positions of replication origin and terminus
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("grep_ori_ter", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__rep_USCOREori_USCOREterInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  AjBool    oriloc = 0;
  AjBool    gcskew = 0;
  AjBool    dbonly = 0;
  ajint	    difthreshold = 0;
  AjPStr    accid  = NULL;
  AjPStr    tmp    = NULL;
  AjPStr    parse  = NULL;
  AjPStrTok handle = NULL;
  AjPStr    ori    = NULL;
  AjPStr    ter    = NULL;

  char *in0;
  char *result;

  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  difthreshold = ajAcdGetInt("difthreshold");
  oriloc = ajAcdGetBoolean("oriloc");
  gcskew = ajAcdGetBoolean("gcskew");
  dbonly = ajAcdGetBoolean("dbonly");
  accid  = ajAcdGetString("accid");
  outf   = ajAcdGetOutfile("outfile");

  params.dif_threshold = difthreshold;
  params.oriloc = 0;
  params.gcskew = 0;
  params.dbonly = 0;

  if(oriloc)
    params.oriloc = 1;
  if(gcskew)
    params.gcskew = 1;
  if(dbonly)
    params.dbonly = 1;

  while(ajSeqallNext(seqall, &seq))
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

      if(soap_call_ns1__rep_USCOREori_USCOREter(
					       &soap,
                                                NULL,
                                                NULL,
					        in0,
                                               &params,
                                               &result
					       ) == SOAP_OK)
        {
          tmp    = ajStrNew();
          parse  = ajStrNew();
          ori    = ajStrNew();
          ter    = ajStrNew();

          ajStrAssignC(&tmp, result);

          ajStrExchangeCC(&tmp, "<", "\n");
          ajStrExchangeCC(&tmp, ">", "\n");

          handle = ajStrTokenNewC(tmp, "\n");

          while(ajStrTokenNextParse(&handle, &parse))
            {
              if(ajStrIsInt(parse))
                if(!ajStrGetLen(ori))
                  ori = ajStrNewS(parse);
                else if(!ajStrGetLen(ter))
                  ter = ajStrNewS(parse);
            }

          ajFmtPrintF(outf, "Sequence: %S Origin: %S Terminus: %S\n",
                      accid, ori, ter);

          ajStrDel(&tmp);
          ajStrDel(&parse);
          ajStrDel(&ori);
          ajStrDel(&ter);
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

  embExit();

  return 0;
}
