/******************************************************************************
** @source genret
**
** Retrieves various gene related infomration from genome flatfile
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
#include "../include/gfile.h"
#include "../include/gpost.h"




/* @prog genret ***************************************************************
**
** Retrieves various gene related infomration from genome flatfile
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("genret", argc, argv, "GEMBASSY", "1.0.0");

  AjPSeqall seqall;
  AjPSeq seq      = NULL;
  AjPStr inseq    = NULL;
  AjPStr gene     = NULL;
  AjPStr access   = NULL;
  AjBool accid    = ajTrue;
  AjPStr argument = NULL;
  AjPFile outfile = NULL;

  AjPStr seqid = NULL;
  AjBool valid = ajFalse;
  AjBool isseq = ajFalse;

  AjPFilebuff buff = NULL;
  AjPFile tmp_file = NULL;
  AjPStr  tmp_name = NULL;

  AjPStr regexstr = NULL;
  AjPStrTok token = NULL;
  AjPRegexp regex = NULL;

  AjPStr url  = NULL;
  AjPStr base = NULL;
  AjPStr head = NULL;
  AjPStr line = NULL;

  seqall   = ajAcdGetSeqall("sequence");
  access   = ajAcdGetString("access");
  gene     = ajAcdGetString("gene");
  argument = ajAcdGetString("argument");
  accid    = ajAcdGetBoolean("accid");
  outfile  = ajAcdGetOutfile("outfile");

  if(
     ajStrMatchC(access, "translation") ||
     ajStrMatchC(access, "get_exon") ||
     ajStrMatchC(access, "get_exons") ||
     ajStrMatchC(access, "get_cdsseq") ||
     ajStrMatchC(access, "get_gbkseq") ||
     ajStrMatchC(access, "get_geneseq") ||
     ajStrMatchC(access, "get_intron") ||
     ajStrMatchC(access, "getseq") ||
     ajStrMatchC(access, "seq") ||
     ajStrMatchC(access, "around_startcodon") ||
     ajStrMatchC(access, "around_stopcodon") ||
     ajStrMatchC(access, "before_startcodon") ||
     ajStrMatchC(access, "before_stopcodon") ||
     ajStrMatchC(access, "after_startcodon") ||
     ajStrMatchC(access, "after_stopcodon")
     )
    {
      isseq = ajTrue;
    }
  else
    {
      ajFmtPrintF(outfile, "gene,%S\n", access);
    }

  base = ajStrNewC("rest.g-language.org");

  ajStrExchangeCC(&argument, " ", "/");
  ajStrExchangeCC(&argument, ",", "/");
  ajStrExchangeCC(&argument, "\t", "/");
  ajStrExchangeCC(&argument, "\r", "/");
  ajStrExchangeCC(&argument, "\n", "/");

  if(ajStrMatchC(gene, "*"))
    {
      ajStrInsertK(&gene, 0, '.');
    }

  if(ajStrPrefixC(gene, "@") || ajStrPrefixC(gene, "list::"))
    {
      ajStrExchangeCC(&gene, "@", "");
      ajStrExchangeCC(&gene, "list::", "");
      ajStrAssignS(&tmp_name, gene);

      tmp_file = ajFileNewInNameS(tmp_name);

      if(!tmp_file)
        {
          ajFmtError("List file (%S) open error\n", tmp_name);
          embExitBad();
        }

      gene = ajStrNew();

      while(ajReadline(tmp_file, &line))
        {
          ajStrAppendS(&gene, line);
        }

      ajFileClose(&tmp_file);
      ajStrDel(&tmp_name);
      ajStrDel(&line);
    }

  tmp_name = ajStrNew();
  gAssignUniqueName(&tmp_name);

  while(ajSeqallNext(seqall, &seq))
    {
      inseq = ajStrNew();

      if(!accid)
        {
          if(gFormatGenbank(seq, &inseq))
            {
              tmp_file = ajFileNewOutNameS(tmp_name);

              if(!tmp_file)
                {
                  ajFmtError("Output file (%S) open error\n", tmp_name);
                  embExitBad();
                }

              ajFmtPrintF(tmp_file, "%S", inseq);

              ajFileClose(&tmp_file);

              ajFmtPrintS(&url, "http://%S/upload/upl.pl", base);

              gFilePostSS(url, tmp_name, &seqid);

              ajStrDel(&url);

              ajSysFileUnlinkS(tmp_name);
            }
          else
            {
              ajFmtError("Sequence does not have features\n"
                         "Proceeding with sequence accession ID\n");
              accid = ajTrue;
            }
        }

      if(accid)
        {
          ajStrAssignS(&seqid, ajSeqGetAccS(seq));

          if(!ajStrGetLen(seqid))
            {
              ajStrAssignS(&seqid, ajSeqGetNameS(seq));
            }

          if(!ajStrGetLen(seqid))
            {
              ajFmtError("No valid header information\n");
              embExitBad();
            }
        }


      url = ajStrNew();

      ajFmtPrintS(&url, "http://%S/%S/*/%S/%S", base, seqid, access, argument);

      if(!gFilebuffURLS(url, &buff))
        {
          ajFmtError("GET error from %S\n", url);
          embExitBad();
        }

      while(ajBuffreadLine(buff, &line))
        {
          ajStrRemoveLastNewline(&line);

          regex = ajRegCompC("^>");

          if(ajRegExec(regex, line))
            {
              head = ajStrNew();

              ajStrAssignS(&head, line);
              ajStrTrimStartC(&head, ">");

              valid = ajFalse;

              token = ajStrTokenNewC(ajStrNewS(gene), " ,\t\r\n");

              while(ajStrTokenNextParse(&token, &regexstr))
                {
                  if(ajStrGetLen(regexstr))
                    {
                      regex = ajRegComp(regexstr);

                      if(ajRegExec(regex, line))
                        {
                          valid = ajTrue;
                          if(!ajStrIsWild(regexstr))
                            {
                              ajStrExchangeSC(&gene, regexstr, "");
                            }
                        }

                      ajRegFree(&regex);
                    }
                }
            }
          else
            {
              if(!ajStrIsAlnum(line))
                continue;

              if(valid)
                {
                  if(isseq)
                    {
                      ajStrFmtWrap(&line, 60);
                      ajFmtPrintF(outfile, ">%S\n%S\n", head, line);
                    }
                  else
                    {
                      ajFmtPrintF(outfile, "%S,%S\n", head, line);
                    }

                  valid = ajFalse;
                }
            }
        }

      ajFileClose(&outfile);

      ajStrDel(&inseq);
    }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&access);
  ajStrDel(&gene);

  embExit();
}
