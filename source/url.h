#ifndef URL_H
#define URL_H
#include <stddef.h>
#define URL_CAP 2048
/* Returns 1 for an HTTP(S) URL with a valid ASCII host, 0 otherwise. */
int normalize_url(const char *input, char *out, size_t cap);
#endif
