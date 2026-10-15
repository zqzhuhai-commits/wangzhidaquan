#include "../source/url.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    char out[URL_CAP];
    const char *valid[][2]={
      {"baidu.com","https://baidu.com"},{"  HTTPS://www.baidu.com/  ","https://www.baidu.com/"},
      {"http://192.168.1.8:8080/a?q=1#x","http://192.168.1.8:8080/a?q=1#x"},
      {"https://example.com/中文","https://example.com/中文"},{"localhost:8000","https://localhost:8000"}
    };
    for(size_t i=0;i<sizeof(valid)/sizeof(*valid);i++){assert(normalize_url(valid[i][0],out,sizeof(out)));assert(!strcmp(out,valid[i][1]));}
    const char *invalid[]={"","  ","https://","javascript:alert(1)","file:///etc/passwd","ftp://x.com","https://a b.com","https://x.com:0","https://x.com:65536","https://user@x.com","https://a..com","https://-a.com","https://a-.com","https://a.com:abc","https://x.com\\@evil.com","//example.com","https://中文.com"};
    for(size_t i=0;i<sizeof(invalid)/sizeof(*invalid);i++)assert(!normalize_url(invalid[i],out,sizeof(out)));
    char long_url[URL_CAP+80];memset(long_url,'a',sizeof(long_url)-1);long_url[sizeof(long_url)-1]=0;
    assert(!normalize_url(long_url,out,sizeof(out)));assert(!normalize_url("baidu.com",out,6));
    puts("URL normalization checks passed");return 0;
}
