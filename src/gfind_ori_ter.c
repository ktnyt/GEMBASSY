#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"

int main(int argc, char *argv[])
{
  embInitPV("gfind_ori_ter", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__find_USCOREori_USCOREterInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  ajint	    window = 0;
  AjBool    purine = 0;
  AjBool    keto   = 0;
  ajint	    filter = 0;
  AjPStr    accid  = NULL;
  AjPStr    tmp    = NULL;
  AjPStr    parse  = NULL;
  AjPStrTok handle = NULL;
  AjPStr    ori    = NULL;
  AjPStr    ter    = NULL;

  char *in0;
  char *result;

  AjBool  show = 0;
  AjPFile outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  window = ajAcdGetInt("window");
  filter = ajAcdGetInt("filt");
  purine = ajAcdGetBoolean("purine");
  keto   = ajAcdGetBoolean("keto");
  accid  = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");

  if(!show)
    outf = ajAcdGetOutfile("outfile");

  params.window = window;
  params.filter = filter;
  params.purine = 0;
  params.keto = 0;

  if(purine)
    params.purine = 1;
  if(keto)
    params.keto = 1;

  while(ajSeqallNext(seqall, &seq))
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

      if(soap_call_ns1__find_USCOREori_USCOREter(
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
	  handle = ajStrNew();
	  ori    = ajStrNew();
	  ter    = ajStrNew();

	  ajStrAssignC(&tmp, result);

	  ajStrExchangeCC(&tmp, "<", "\n");
	  ajStrExchangeCC(&tmp, ">", "\n");

	  handle = ajStrTokenNewC(tmp, "\n");

	  while(ajStrTokenNextParse(&handle, &parse))
	    {
	      if(ajStrIsInt(parse))
		if(!ori)
		  ori = ajStrNewS(parse);
		else if(!ter)
		  ter = ajStrNewS(parse);
	    }

	  if(show)
	    ajFmtPrint("Sequence: %S Origin: %S Terminus: %S\n",
		       accid, ori, ter);
	  else
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

  if(outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);

  embExit();

  return 0;
}
