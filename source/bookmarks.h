#ifndef BOOKMARKS_H
#define BOOKMARKS_H
#include <stddef.h>
#include "url.h"
#define BOOKMARK_NAME_CAP 97
typedef struct { char name[BOOKMARK_NAME_CAP],url[URL_CAP]; } Bookmark;
typedef struct { Bookmark *items; size_t count,capacity; } Bookmarks;
void bookmarks_free(Bookmarks *b);
int bookmarks_load(Bookmarks *b,const char *path);
/* Mutations are kept only after a successful recoverable file replacement. */
int bookmarks_add(Bookmarks *b,const char *path,const char *name,const char *url);
int bookmarks_edit(Bookmarks *b,const char *path,size_t index,const char *name,const char *url);
int bookmarks_remove(Bookmarks *b,const char *path,size_t index);
#endif
