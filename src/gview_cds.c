#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"
#include "../include/gplot.h"

int main(int argc, char *argv[])
{
  embInitPV("gview_cds", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__view_USCOREcdsInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq = NULL;
  ajint	    length = 0;
  ajint	    gap = 0;
  AjPStr    accid = NULL;

  char *in0;
  char *result;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;    

  seqall = ajAcdGetSeqall("sequence");
  length = ajAcdGetInt("length");
  gap    = ajAcdGetInt("gap");
  accid  = ajAcdGetString("accid");

  plot = ajAcdGetToggle("plot");

  if(!plot)
    outf = ajAcdGetOutfile("outfile");
  else
    mult = ajAcdGetGraphxy("graph");

  params.length = length;
  params.gap    = gap;
  params.output = "f";

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
              title = ajStrNew();

              ajStrAppendC(&title, argv[0]);
              ajStrAppendC(&title, " of ");
              ajStrAppendS(&title, accid);

              gpp.title = ajStrNewS(title);
              gpp.xlab = ajStrNewC("position");
              gpp.ylab = ajStrNewC("percentage");

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

              AJFREE(gpp.title);
              AJFREE(gpp.xlab);
              AJFREE(gpp.ylab);

              ajStrDel(&title);
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

  embExit();

  return 0;
}
