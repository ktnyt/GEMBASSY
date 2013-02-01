#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gfile.h"
#include "../include/display_png.h"

int main(int argc, char *argv[])
{
  embInitPV("gseq2png", argc, argv, "GEMBASSY", "1.0.0");

  struct soap soap;
  struct ns1__seq2pngInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  ajint	    width    = 0;
  ajint	    window   = 0;
  AjBool    show     = 0;
  AjPStr    accid    = NULL;
  AjPFile   outf     = NULL;
  AjPStr    filename = NULL;
  AjPStr    outfname = NULL;
  AjPStr    tempname = NULL;

  char *in0;
  char *result;

  seqall   = ajAcdGetSeqall("sequence");
  window   = ajAcdGetInt("window");
  width    = ajAcdGetInt("width");
  filename = ajAcdGetString("filename");
  show     = ajAcdGetToggle("show");
  accid    = ajAcdGetString("accid");

  params.window = window;
  params.width  = width;
  params.output = "g";

  while(ajSeqallNext(seqall, &seq))
    {

      soap_init(&soap);

      inseq = NULL;

      ajStrAppendC(&inseq, ">");
      ajStrAppendS(&inseq, ajSeqGetNameS(seq));
      ajStrAppendC(&inseq, "\n");
      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      ajStrAssignS(&accid, ajSeqGetAccS(seq));

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
          outfname = ajStrNew();
          tempname = ajStrNew();

          ajStrFromLong(&tempname, ajSeqallGetCount(seqall));
          ajStrInsertC(&tempname, 0, ".");
          ajStrAppendC(&tempname, ".png");

          if(!ajStrExchangeCS(&outfname, ".png", tempname))
            {
              ajStrAppendS(&outfname, tempname);
            }

          outf = ajFileNewOutNameS(outfname);

          ajStrDel(&outfname);
          ajStrDel(&tempname);

          if(gHttpGetBinC(result, &outf))
            {
              if(show)
                {
                  if(display_png(ajCharNewS(outfname), argv[0], ajCharNewS(accid)))
                    {
                      ajFmtError("Error in X11 displaying\n");
                      embExitBad();
                    }
                }
            }
          else
            {
              ajFmtError("File downloading error\n");
              embExitBad();
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

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&filename);

  embExit();

  return 0;
}
