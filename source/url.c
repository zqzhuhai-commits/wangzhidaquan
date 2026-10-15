#include "url.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
int normalize_url(const char *input, char *out, size_t cap) {
    if (!input || !out || cap < 1) return 0;
    out[0] = 0;
    while (isspace((unsigned char)*input)) input++;
    size_t n = strlen(input);
    while (n && isspace((unsigned char)input[n-1])) n--;
    if (!n || n >= URL_CAP-9) return 0;
    char buf[URL_CAP]; memcpy(buf,input,n); buf[n]=0;
    for (size_t i=0;i<n;i++) if ((unsigned char)buf[i]<=32 || buf[i]==127 || buf[i]=='\\') return 0;
    const char *body=buf, *scheme="https://";
    if (!strncasecmp(buf,"https://",8)) body=buf+8;
    else if (!strncasecmp(buf,"http://",7)) { body=buf+7; scheme="http://"; }
    else if (strstr(buf,"://") || !strncmp(buf,"//",2)) return 0;
    const char *end=body+strcspn(body,"/?#");
    if (end==body) return 0;
    const char *port=NULL;
    for (const char *p=body;p<end;p++) {
        if (*p==':') { if (port) return 0; port=p; }
        else if (!(isalnum((unsigned char)*p) && (unsigned char)*p<128) && *p!='.' && *p!='-') return 0;
    }
    const char *host_end=port?port:end;
    if (host_end==body || host_end-body>253) return 0;
    const char *label=body;
    for (const char *p=body;p<=host_end;p++) {
        if (p==host_end || *p=='.') {
            if (p==label || p-label>63 || *label=='-' || p[-1]=='-') return 0;
            label=p+1;
        }
    }
    if (port) {
        if (port+1==end) return 0;
        unsigned long value=0;
        for (const char *p=port+1;p<end;p++) { if (!isdigit((unsigned char)*p)) return 0; value=value*10+(*p-'0'); if(value>65535) return 0; }
        if (!value) return 0;
    }
    size_t needed=strlen(scheme)+strlen(body)+1;
    if (needed>cap) return 0;
    snprintf(out,cap,"%s%s",scheme,body);
    return 1;
}
