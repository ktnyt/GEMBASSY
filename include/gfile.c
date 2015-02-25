/******************************************************************************
** @source GEMBASSY file routines
**
** @version 1.0
** @modified December 27 2012 Hidetoshi Itaya Created this file
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/

#include "gfile.h"




/* @func gValID ***************************************************************
**
** Checks if an input string is a valid ID
**
** @param id [r]  [AjPStr] ID to check
** @return [AjBool]
** @@
******************************************************************************/

ajint gValID(AjPStr id){
  AjPFilebuff buff = NULL;
  AjPStr      url  = NULL;
  AjPStr      line = NULL;
  AjPRegexp   pval = NULL;

  url = ajStrNewC("http://web.sfc.keio.ac.jp/~t11080hi/valID/valID.cgi?id=");
  url = ajStrNew();
  ajFmtPrintS(&url, "http://rest.g-language.org/%S", id);

  //ajStrAppendS(&url, id);

  if(!gFilebuffURLS(url, &buff)) {
    return ajFalse;
  }

  return ajTrue;

  ajBuffreadLine(buff, &line);

  pval = ajRegCompC("^0");

  if(ajRegExec(pval, line))
    return ajFalse;

  return ajTrue;
}




/* @func gStrAppendURLS *******************************************************
**
** Downloads file from a specified URL and writes to given output file
**
** @param [r] url [AjPStr] URL to download file from
** @param [r] string [AjPStr] String to write into
** @return [AjBool] 
** @@
******************************************************************************/

AjBool gStrAppendURLS(AjPStr url, AjPStr* string){
  AjPFilebuff buff = NULL;
  AjPStr      file = NULL;
  AjPStr      line = NULL;

  if(!*string)
    *string = ajStrNew();

  if(!gFilebuffURLS(url, &buff))
    return ajFalse;

  while(ajBuffreadLine(buff, &line)){
    ajStrAppendS(string, line);
  }

  ajFilebuffDel(&buff);

  return ajTrue;
}




/* @func gStrAppendURLC *******************************************************
**
** Downloads file from a specified URL and writes to given output file
**
** @param [r] url [char*] URL to download file from
** @param [r] string [AjPStr] String to write into
** @return [AjBool] 
** @@
******************************************************************************/

AjBool gStrAppendURLC(char* url, AjPStr* string){
  if(!gStrAppendURLS(ajStrNewC(url), string))
    {
      return ajFalse;
    }

  return ajTrue;
}




/* @func gFileOutURLS *********************************************************
**
** Downloads file from a specified URL and writes to given output file
**
** @param [r] url [AjPStr] URL to download file from
** @param [r] outf [AjPFile] File object to write into
** @return [AjBool] 
** @@
******************************************************************************/

AjBool gFileOutURLS(AjPStr url, AjPFile* outf){
  AjPFilebuff buff = NULL;
  AjPStr      file = NULL;
  AjPStr      line = NULL;

  if(!gFilebuffURLS(url, &buff))
    return ajFalse;

  while(ajBuffreadLine(buff, &line)){
    ajWriteline(*outf, line);
  }

  ajFilebuffDel(&buff);

  return ajTrue;
}




/* @func gFileOutURLC *********************************************************
**
** Downloads file from a specified URL and writes to given output file
**
** @param [r] url [char*] URL to download file from
** @param [r] outf [AjPFile] File object to write into
** @return [AjBool] 
** @@
******************************************************************************/

AjBool gFileOutURLC(char* url, AjPFile* outf){
  if(!gFileOutURLS(ajStrNewC(url), outf))
    {
      return ajFalse;
    }

  return ajTrue;
}




/* @func gFilebuffURLS ********************************************************
**
** Downloads file from a specified URL and inputs in file buffer
**
** @param [r] url [AjPStr] URL to download file from
** @param [r] buff [AjPFilebuff] File buffer to set
** @return [AjBool]
** @@
******************************************************************************/

AjBool gFilebuffURLS(AjPStr url, AjPFilebuff* buff){
  AjPStr line = NULL;
  AjPStr host = NULL;
  AjPStr path = NULL;
  ajint  port = 80;

  ajHttpUrlDeconstruct(url, &port, &host, &path);

  *buff = ajHttpRead(NULL, NULL, NULL, host, port, path);

  if(!*buff)
    return ajFalse;

  ajFilebuffHtmlNoheader(*buff);

  return ajTrue;
}




/* @func gFilebuffURLC ********************************************************
**
** Downloads file from a specified URL and inputs in file buffer
**
** @param [r] url [char*] URL to download file from
** @param [r] buff [AjPFilebuff] File buffer to set
** @return [AjBool]
** @@
******************************************************************************/

AjBool gFilebuffURLC(char* url, AjPFilebuff* buff){
  gFilebuffURLS(ajStrNewC(url), buff);

  if(!*buff)
    return ajFalse;

  return ajTrue;
}




/* @func gAssignUniqueName ****************************************************
**
** Creates a unique filename
**
** @return [AjPStr] the unique filename
** @@
******************************************************************************/

void gAssignUniqueName(AjPStr *string) {
  static char ext[2] = "A";

  ajFmtPrintS(string, "%08d%s", getpid(), ext);

  if( ++ext[0] > 'Z' ) {
    ext[0] = 'A';
  }
}




/* @func gCreateUniqueName ****************************************************
**
** Returns a unique filename
**
** @return [AjPStr] the unique filename
** @@
******************************************************************************/

AjPStr gCreateUniqueName() {
  AjPStr string;

  gAssignUniqueName(&string);

  return string;
}




/* @func gFormatGenbank *******************************************************
**
** Creates a genbank format string with sequence and features
**
** @param [r] seq [AjPSeq] Sequence object to write
** @param [r] inseq [AjPStr] String to write to
** @return [AjBool]
** @@
******************************************************************************/

AjBool gFormatGenbank(AjPSeq seq, AjPStr *inseq){
  AjPSeqout     seqout   = NULL;
  AjPFeattabOut featout  = NULL;
  AjPFeattable  feat     = NULL;
  AjPStr        seqline  = NULL;
  AjPStr        featline = NULL;
  AjPFile       seqfile  = NULL;
  AjPFile       featfile = NULL;
  AjPStr        filename = NULL;
  AjBool        hasfeats = ajTrue;

  gAssignUniqueName(&filename);
  feat = ajSeqGetFeatCopy(seq);

  if(!feat) {
    hasfeats = ajFalse;
  }

  seqout = ajSeqoutNew();

  if(!ajSeqoutOpenFilename(seqout,filename))
    embExitBad();

  ajSeqoutSetFormatS(seqout,ajStrNewC("genbank"));
  ajSeqoutWriteSeq(seqout,seq);
  ajSeqoutClose(seqout);
  ajSeqoutDel(&seqout);

  seqfile = ajFileNewInNameS(filename);
  ajSysFileUnlinkS(filename);

  if(hasfeats) {
    featout = ajFeattabOutNew();

    if(!ajFeattabOutOpen(featout,filename))
      return ajFalse;

    ajFeattableWriteGenbank(featout,feat);

    ajFeattableDel(&feat);
    //ajFeattabOutDel(&featout);
    ajFileClose(&(featout->Handle));

    featfile = ajFileNewInNameS(filename);
    ajSysFileUnlinkS(filename);
  }

  while(ajReadline(seqfile,&seqline)){
    if(hasfeats && ajStrMatchC(seqline,"ORIGIN\n")){
      while(ajReadline(featfile,&featline)){
        ajStrAppendS(inseq, featline);
      }
    }
    ajStrAppendS(inseq, seqline);
  }

  ajStrDel(&seqline);
  ajStrDel(&featline);
  ajStrDel(&filename);
  ajFileClose(&seqfile);
  ajFileClose(&featfile);

  return hasfeats;
}




/* @func gGetFileContent ******************************************************
**
** Reads file content and sets it to string pointer
**
** @param [r] content [AjPSeq] String to write to
** @param [r] filename [AjPSeq] Filename to open
** @return [AjBool]
** @@
******************************************************************************/

AjBool gGetFileContent(AjPStr* content, AjPStr filename){
  AjPFile file    = NULL;
  AjPStr  line    = NULL;

  if((file = ajFileNewInNameS(filename)) == NULL)
    return ajFalse;

  while(ajReadline(file, &line))
    ajStrAppendS(content, line);

  if(file)
    ajFileClose(&file);

  ajSysFileUnlinkS(filename);

  return ajTrue;
}




/* @func gtaiFileOutURLS ******************************************************
**
** Downloads file from a specified URL and inputs in file buffer
**
** @param [r] url [AjPStr] URL to download file from
** @param [r] buff [AjPFilebuff] File buffer to set
** @return [AjBool]
** @@
******************************************************************************/

AjBool gtaiFileOutURLS(AjPStr url, AjPFile* outf, AjBool tai){
  if(tai)
    {
      CURL *curl;
      CURLcode curl_res;

      Memory *mem = malloc(sizeof(Memory*));

      mem->size = 0;
      mem->memory = NULL;

      curl_global_init(CURL_GLOBAL_ALL);

      curl = curl_easy_init();

      if(curl)
        {
          curl_easy_setopt(curl, CURLOPT_URL, ajCharNewS(url));
          curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, mem);
        }

      curl_res = curl_easy_perform(curl);

      if(CURLE_OK == curl_res)
        {
          char* redir;
          curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redir);

          if((CURLE_OK == curl_res) && redir) {
            ajStrAssignC(&url, redir);
            ajStrExchangeCC(&url, "cai.csv", "tai.csv");
          }
        }

      free(mem);
      curl_easy_cleanup(curl);
      curl_global_cleanup();
    }

  return gFileOutURLS(url, outf);
}
