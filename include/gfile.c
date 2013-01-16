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

AjBool gValID(AjPStr id){
  AjPStr url      = NULL;
  AjPStr filename = NULL;
  AjPStr content  = NULL;
  char   first[1];

  url = ajStrNewC("http://web.sfc.keio.ac.jp/~t11080hi/valID/valID.cgi?id=");

  filename = gGetUniqueFileName();

  url = ajFmtStr("%S%S", url, id);
  gGetFileFromURL(url, filename);
  gGetFileContent(&content, filename);
  first[0] = ajStrGetCharFirst(content);
  if(atoi(first))
    return ajTrue;
  else
    return ajFalse;
}




/* @func gGetFileFromURL******************************************************
**
** Downloads file from a specified URL
**
** @param [r] url [AjPStr] URL to download file from
** @param [r] filename [AjPStr] Filename to output to
** @return [AjBool] 
** @@
******************************************************************************/

AjBool gGetFileFromURL(AjPStr url, AjPStr filename){
  AjPFilebuff buff = NULL;
  AjPFile     outf = NULL;
  AjPStr      line = NULL;
  AjPStr      cont = NULL;
  AjPStr      host = NULL;
  AjPStr      path = NULL;
  ajint       port = 80;

  ajHttpUrlDeconstruct(url, &port, &host, &path);
  buff = ajHttpRead(NULL, NULL, NULL, host, port, path);
  outf = ajFileNewInNameS(filename);
  ajFilebuffHtmlNoheader(buff);
  while(ajBuffreadLine(buff, &line)){
    ajStrAppendS(&cont, line);
  }
  ajFmtPrint("%S", cont);
  
  ajFileClose(&outf);

  return 0;
}




/* @func gGetUniqueFileName ***************************************************
**
** Creates a unique filename
**
** @return [AjPStr] the unique filename
** @@
******************************************************************************/

AjPStr gGetUniqueFileName(void) {
  static char ext[2] = "A";
  AjPStr filename    = NULL;

  ajFmtPrintS(&filename, "%08d%s", getpid(), ext);

  if( ++ext[0] > 'Z' ) {
    ext[0] = 'A';
  }

  return filename;
}




/* @func gFormatGenbank *******************************************************
**
** Creates a genbank format string with sequence and features
**
** @param [r] seq [AjPSeq] Sequence object to write
** @return [AjPStr] Sequence output
** @@
******************************************************************************/

AjPStr gFormatGenbank(AjPSeq seq){
  AjPSeqout     seqout   = NULL;
  AjPFeattabOut featout  = NULL;
  AjPFeattable  feat      = NULL;
  AjPStr        seqline  = NULL;
  AjPStr        featline = NULL;
  AjPFile       seqfile  = NULL;
  AjPFile       featfile = NULL;
  AjPStr        inseq    = NULL;
  AjPStr        filename = NULL;

  filename = gGetUniqueFileName();
  feat = ajSeqGetFeatCopy(seq);

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
    embExitBad();

  ajFeattableWriteGenbank(featout,feat);
  ajFeattabOutDel(&featout);

  featfile = ajFileNewInNameS(filename);
  ajSysFileUnlinkS(filename);

  while(ajReadline(seqfile,&seqline)){
    if(ajStrCmpC(seqline,"ORIGIN\n") == 0){
      while(ajReadline(featfile,&featline)){
	ajStrAppendS(&inseq,featline);
      }
    }
    ajStrAppendS(&inseq,seqline);
  }

  ajStrDel(&seqline);
  ajStrDel(&featline);
  ajStrDel(&filename);
  ajFileClose(&seqfile);
  ajFileClose(&featfile);

  return inseq;
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
