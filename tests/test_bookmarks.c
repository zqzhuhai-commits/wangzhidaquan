#define _POSIX_C_SOURCE 200809L
#include "../source/bookmarks.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#ifdef rename
#undef rename
#endif
int rename(const char *,const char *);
/* Emulate libnx: renaming over an existing destination must fail. */
int sd_rename(const char *a,const char *b){struct stat st;if(!stat(b,&st)){errno=EEXIST;return -1;}return rename(a,b);}
int main(void) {
 char dir[]="/tmp/webnav-bookmarks-XXXXXX";assert(mkdtemp(dir));char path[512],bad[512];snprintf(path,sizeof(path),"%s/sites.tsv",dir);snprintf(bad,sizeof(bad),"%s/missing/sites.tsv",dir);
 Bookmarks b={0},loaded={0};assert(bookmarks_load(&b,path));assert(!b.count);
 assert(bookmarks_add(&b,path,"测试站点","example.com"));assert(b.count==1);assert(!strcmp(b.items[0].url,"https://example.com"));
 assert(!bookmarks_add(&b,path,"重复","example.com"));assert(!bookmarks_add(&b,path,"非法\t名称","a.com"));assert(!bookmarks_add(&b,path," ","a.com"));assert(!bookmarks_add(&b,path,"脚本","javascript:alert(1)"));
 assert(bookmarks_load(&loaded,path));assert(loaded.count==1);assert(!strcmp(loaded.items[0].name,"测试站点"));
 assert(!bookmarks_add(&b,bad,"失败","fail.com"));assert(b.count==1);
 assert(!bookmarks_edit(&b,bad,0,"错误改名","changed.com"));assert(!strcmp(b.items[0].name,"测试站点"));
 assert(!bookmarks_remove(&b,bad,0));assert(b.count==1);
 assert(bookmarks_edit(&b,path,0,"修改后","https://new.example.com/中文"));assert(bookmarks_load(&loaded,path));assert(!strcmp(loaded.items[0].name,"修改后"));
 for(int i=0;i<128;i++){char url[100],name[100];snprintf(url,sizeof(url),"https://site%d.example.com",i);snprintf(name,sizeof(name),"网址%d",i);assert(bookmarks_add(&b,path,name,url));}
 assert(b.count==129);assert(bookmarks_load(&loaded,path));assert(loaded.count==129);
 assert(bookmarks_remove(&b,path,0));assert(b.count==128);assert(!strcmp(b.items[0].name,"网址0"));
 assert(bookmarks_load(&loaded,path));assert(loaded.count==128);
 FILE *f=fopen(path,"a");assert(f);fputs("malformed\nscript\tjavascript:alert(1)\n",f);fclose(f);assert(bookmarks_load(&loaded,path));assert(loaded.count==128);
 char backup[520];snprintf(backup,sizeof(backup),"%s.bak",path);assert(!rename(path,backup));assert(bookmarks_load(&loaded,path));assert(loaded.count==128);assert(bookmarks_add(&loaded,path,"中断恢复","recover.example.com"));assert(loaded.count==129);assert(access(backup,F_OK)!=0);
 bookmarks_free(&b);bookmarks_free(&loaded);remove(path);rmdir(dir);puts("Bookmarks passed: persistence, edit/delete, duplicates, validation, 129 entries and failed-write rollback");
}
