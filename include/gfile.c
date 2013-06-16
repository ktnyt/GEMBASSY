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

  ajStrAppendS(&url, id);

  gFilebuffURLS(url, &buff);

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

  gAssignUniqueName(&filename);
  feat = ajSeqGetFeatCopy(seq);

  if(!feat)
    return ajFalse;

  seqout = ajSeqoutNew();

  if(!ajSeqoutOpenFilename(seqout,filename))
    embExitBad();

  ajSeqoutSetFormatS(seqout,ajStrNewC("genbank"));
  ajSeqoutWriteSeq(seqout,seq);
  ajSeqoutClose(seqout);
  ajSeqoutDel(&seqout);

  seqfile = ajFileNewInNameS(filename);
  ajSysFileUnlinkS(filename);

  featout = ajFeattabOutNew();

  if(!ajFeattabOutOpen(featout,filename))
    return ajFalse;

  ajFeattableWriteGenbank(featout,feat);

  ajFeattableDel(&feat);
  //ajFeattabOutDel(&featout);
  ajFileClose(&(featout->Handle));

  featfile = ajFileNewInNameS(filename);
  ajSysFileUnlinkS(filename);

  while(ajReadline(seqfile,&seqline)){
    if(ajStrMatchC(seqline,"ORIGIN\n")){
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

  return ajTrue;
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
