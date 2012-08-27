#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string.h>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written;
  written = fwrite(ptr, size, nmemb, stream);
  return written;
}


int get_file(char* url, char* outfilename) {
  CURL *curl;
  FILE *fp;
  CURLcode res;
  curl = curl_easy_init();
  if (curl) {
    fp = fopen(outfilename,"wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
  }
  return 0;
}

AjPStr getUniqueFileName(void) {
  static char ext[2] = "A";
  AjPStr filename    = NULL;

  ajFmtPrintS(&filename, "%08d%s",getpid(), ext);

  if( ++ext[0] > 'Z' ) {
    ext[0] = 'A';
  }

  return filename;
}

AjPStr getGenbank(AjPSeq seq){
  AjPSeqout     seqout   = NULL;
  AjPFeattabOut featout  = NULL;
  AjPStr        seqline  = NULL;
  AjPStr        featline = NULL;
  AjPFile       seqfile  = NULL;
  AjPFile       featfile = NULL;
  AjPStr        inseq    = NULL;
  AjPStr        filename = NULL;
  AjPFeattable  feat     = NULL;

  filename = getUniqueFileName();
  feat = ajSeqGetFeatCopy(seq);

  seqout = ajSeqoutNew();

  if(!ajSeqoutOpenFilename(seqout,filename)){
    embExitBad();
  }

  ajSeqoutSetFormatS(seqout,ajStrNewC("genbank"));
  ajSeqoutWriteSeq(seqout,seq);
  ajSeqoutClose(seqout);
  ajSeqoutDel(&seqout);

  seqfile = ajFileNewInNameS(filename);
  ajSysFileUnlinkS(filename);

  featout = ajFeattabOutNew();

  if(!ajFeattabOutOpen(featout,filename)){
    embExitBad();
  }

  ajFeattableWriteGenbank(featout,feat);
  ajFeattabOutDel(&featout);

  featfile = ajFileNewInNameS(filename);
  ajSysFileUnlinkS(filename);

  while(ajReadline(seqfile,&seqline)){
    if(!ajStrCmpC(seqline,"ORIGIN\n")){
      while(ajReadline(featfile,&featline)){
	ajStrAppendS(&inseq,featline);
      }
    }
    ajStrAppendS(&inseq,seqline);
  }
  return inseq;
}
