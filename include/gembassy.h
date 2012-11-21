#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string.h>

typedef struct{
  char* memory;
  size_t size;
} Memory;

size_t get_data(void* ptr, size_t size, size_t nmemb, void* data){
  if(size * nmemb == 0)
    return 0;

  size_t realsize = size * nmemb;
  Memory* mem = (Memory*)data;
  mem->memory = (char*)realloc(mem->memory,mem->size + realsize + 1);
  if(mem->memory){
    memcpy(&(mem->memory[mem->size]),ptr,realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }

  return realsize;
}

int valID(char* id){
  char url[100];
  CURL *curl;
  CURLcode res;
  Memory* mem = malloc(sizeof(Memory*));
  int ret;

  mem->size = 0;
  mem->memory = NULL;

  sprintf(url,"http://web.sfc.keio.ac.jp/~t11080hi/valID/valID.cgi?id=%s",id);
  curl = curl_easy_init();
  if(curl){
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)mem);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    ret = atoi(mem->memory);
    return ret;
  }

  return 0;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream){
  size_t written;
  written = fwrite(ptr, size, nmemb, stream);
  return written;
}

int get_file(char* url, char* outfilename){
  CURL *curl;
  FILE *fp;
  CURLcode res;
  curl = curl_easy_init();
  if(curl){
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
  AjPFeattable  feat      = NULL;
  AjPStr        seqline  = NULL;
  AjPStr        featline = NULL;
  AjPFile       seqfile  = NULL;
  AjPFile       featfile = NULL;
  AjPStr        inseq    = NULL;
  AjPStr        filename = NULL;

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

AjPStr getContentS(AjPStr filename){
  AjPFile file    = NULL;
  AjPStr  line    = NULL;
  AjPStr  content = NULL;

  if((file = ajFileNewInNameS(filename)) == NULL)
    embExitBad();

  while(ajReadline(file, &line))
    ajStrAppendS(&content, line);

  if(file)
    ajFileClose(&file);

  ajSysFileUnlinkS(filename);

  return content;
}
