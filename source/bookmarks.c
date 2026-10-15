#include "bookmarks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
void bookmarks_free(Bookmarks *b){free(b->items);memset(b,0,sizeof(*b));}
static int prepare(Bookmark *item,const char *name,const char *url) {
    if(!name || !url)return 0;
    while(isspace((unsigned char)*name))name++;
    size_t n=strlen(name);while(n&&isspace((unsigned char)name[n-1]))n--;
    if(!n || n>=BOOKMARK_NAME_CAP)return 0;
    for(size_t i=0;i<n;i++)if((unsigned char)name[i]<32 || name[i]==127)return 0;
    memcpy(item->name,name,n);item->name[n]=0;
    return normalize_url(url,item->url,sizeof(item->url));
}
static int push(Bookmarks *b,const Bookmark *item) {
    if(b->count==b->capacity){
        size_t cap=b->capacity?b->capacity*2:8;
        if(cap<b->capacity || cap>SIZE_MAX/sizeof(Bookmark))return 0;
        void *p=realloc(b->items,cap*sizeof(Bookmark));if(!p)return 0;
        b->items=p;b->capacity=cap;
    }
    b->items[b->count++]=*item;return 1;
}
static int duplicate(const Bookmarks *b,const char *url,size_t except) {
    for(size_t i=0;i<b->count;i++)if(i!=except&&!strcmp(b->items[i].url,url))return 1;
    return 0;
}
int bookmarks_load(Bookmarks *b,const char *path) {
    FILE *f=fopen(path,"r");
    if(!f && errno==ENOENT) {
        size_t n=strlen(path);char *backup=malloc(n+5);if(!backup)return 0;
        snprintf(backup,n+5,"%s.bak",path);f=fopen(backup,"r");int error=errno;free(backup);
        if(!f)return error==ENOENT;
    }
    if(!f)return 0;
    Bookmarks loaded={0};char line[BOOKMARK_NAME_CAP+URL_CAP+8];int ok=1;
    while(fgets(line,sizeof(line),f)) {
        size_t n=strlen(line);
        if(n && line[n-1]!='\n' && !feof(f)){int c;while((c=fgetc(f))!='\n'&&c!=EOF){}continue;}
        line[strcspn(line,"\r\n")]=0;char *sep=strchr(line,'\t');if(!sep)continue;
        *sep++=0;Bookmark item;
        if(prepare(&item,line,sep)&&!duplicate(&loaded,item.url,SIZE_MAX)&&!push(&loaded,&item)){ok=0;break;}
    }
    if(ferror(f))ok=0;
    if(fclose(f))ok=0;
    if(ok){bookmarks_free(b);*b=loaded;}else bookmarks_free(&loaded);
    return ok;
}
static int save(const Bookmarks *b,const char *path,size_t skip) {
    size_t n=strlen(path);char *tmp=malloc(n+5);if(!tmp)return 0;
    memcpy(tmp,path,n);memcpy(tmp+n,".tmp",5);
    FILE *f=fopen(tmp,"w");if(!f){free(tmp);return 0;}
    int ok=1;
    for(size_t i=0;i<b->count;i++)if(i!=skip&&fprintf(f,"%s\t%s\n",b->items[i].name,b->items[i].url)<0){ok=0;break;}
    if(fflush(f))ok=0;
    if(fclose(f))ok=0;
    /* libnx SD rename does not replace an existing destination. Rotate the old
       file to a backup; load can recover it if interrupted before replacement. */
    char *backup=malloc(n+5);int rotated=0;
    if(!backup)ok=0;
    if(backup)snprintf(backup,n+5,"%s.bak",path);
    if(ok) {
        struct stat st;
        if(stat(path,&st)==0) {
            if(remove(backup)&&errno!=ENOENT)ok=0;
            if(ok){if(rename(path,backup))ok=0;else rotated=1;}
        } else if(errno!=ENOENT)ok=0;
    }
    if(ok&&rename(tmp,path)){ok=0;if(rotated)rename(backup,path);}
    if(ok)remove(backup);else remove(tmp);
    free(backup);free(tmp);return ok;
}
int bookmarks_add(Bookmarks *b,const char *path,const char *name,const char *url) {
    Bookmark item;
    if(!prepare(&item,name,url)||duplicate(b,item.url,SIZE_MAX)||!push(b,&item))return 0;
    if(save(b,path,SIZE_MAX))return 1;
    b->count--;return 0;
}
int bookmarks_edit(Bookmarks *b,const char *path,size_t index,const char *name,const char *url) {
    Bookmark item;
    if(index>=b->count||!prepare(&item,name,url)||duplicate(b,item.url,index))return 0;
    Bookmark previous=b->items[index];b->items[index]=item;
    if(save(b,path,SIZE_MAX))return 1;
    b->items[index]=previous;return 0;
}
int bookmarks_remove(Bookmarks *b,const char *path,size_t index) {
    if(index>=b->count||!save(b,path,index))return 0;
    memmove(b->items+index,b->items+index+1,(b->count-index-1)*sizeof(Bookmark));b->count--;return 1;
}
