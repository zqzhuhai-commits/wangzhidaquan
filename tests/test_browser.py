"""Exercise actual browser routing and result handling with a mocked libnx API."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
header = r'''
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef uint32_t Result;
#define R_SUCCEEDED(r) ((r)==0)
#define R_FAILED(r) ((r)!=0)
typedef enum { WebExitReason_ExitButton=0, WebExitReason_ErrorDialog=7 } WebExitReason;
typedef struct { uint8_t bytes[16]; } Uuid;
typedef struct { int dummy; } WebCommonConfig;
typedef struct { int dummy; } WebCommonReply;
typedef struct { int dummy; } WebWifiConfig;
typedef struct { uint32_t unk_x0; Result res; } WebWifiReturnValue;
void webWifiCreate(WebWifiConfig*,const char*,const char*,Uuid,uint32_t);
Result webWifiShow(WebWifiConfig*,WebWifiReturnValue*);
Result webPageCreate(WebCommonConfig*,const char*);
Result webConfigSetWhitelist(WebCommonConfig*,const char*);
Result webConfigSetFooter(WebCommonConfig*,bool);
Result webConfigSetPointer(WebCommonConfig*,bool);
Result webConfigShow(WebCommonConfig*,WebCommonReply*);
Result webReplyGetExitReason(WebCommonReply*,WebExitReason*);
'''
test = r'''
#include "browser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int step,fail_at,wifi_calls;
static Result wifi_show_rc,wifi_reply_rc;
static WebExitReason exit_reason;
static const char *url="https://example.com/";
static Result next(void){return ++step==fail_at?0x1234:0;}
Result webPageCreate(WebCommonConfig *c,const char *u){(void)c;assert(!strcmp(u,url));return next();}
Result webConfigSetWhitelist(WebCommonConfig *c,const char *s){(void)c;assert(!strcmp(s,"^https?://.*"));return next();}
Result webConfigSetFooter(WebCommonConfig *c,bool b){(void)c;assert(b);return next();}
Result webConfigSetPointer(WebCommonConfig *c,bool b){(void)c;assert(b);return next();}
Result webConfigShow(WebCommonConfig *c,WebCommonReply *r){(void)c;(void)r;return next();}
Result webReplyGetExitReason(WebCommonReply *r,WebExitReason *e){(void)r;*e=exit_reason;return next();}
void webWifiCreate(WebWifiConfig *c,const char *t,const char *u,Uuid id,uint32_t rev){
    (void)c;assert(!t);assert(!strcmp(u,url));assert(rev==0);
    for(size_t i=0;i<sizeof(id.bytes);i++)assert(id.bytes[i]==0);
    wifi_calls++;
}
Result webWifiShow(WebWifiConfig *c,WebWifiReturnValue *r){(void)c;wifi_calls++;r->res=wifi_reply_rc;return wifi_show_rc;}
int main(void){
    const char *stages[]={"create","whitelist","footer","pointer","web-show","exit-reason"};
    for(fail_at=1;fail_at<=6;fail_at++){
        step=0;BrowserReport r=browser_open(BrowserMode_Standard,url);
        assert(r.rc==0x1234);assert(!r.has_exit_reason);assert(step==fail_at);
        assert(!strcmp(r.stage,stages[fail_at-1]));assert(wifi_calls==0);
    }
    fail_at=0;step=0;exit_reason=WebExitReason_ErrorDialog;
    BrowserReport r=browser_open(BrowserMode_Standard,url);
    assert(r.rc==0&&r.has_exit_reason&&r.exit_reason==WebExitReason_ErrorDialog);
    step=0;exit_reason=WebExitReason_ExitButton;r=browser_open(BrowserMode_Standard,url);
    assert(r.rc==0&&r.has_exit_reason&&r.exit_reason==WebExitReason_ExitButton);
    for(int i=0;i<3;i++){
        step=wifi_calls=0;wifi_show_rc=i==1?0x2345:0;wifi_reply_rc=i==2?0x3456:0;
        r=browser_open(BrowserMode_Wifi,url);
        assert(step==0&&wifi_calls==2&&!r.has_exit_reason);
        assert(r.rc==wifi_show_rc&&r.reply_rc==wifi_reply_rc);
        assert(!strcmp(r.stage,"wifi-show"));
    }
    puts("Browser routing passed: both modes, setup failures, system error exit, Wi-Fi call/reply errors");
}
'''
with tempfile.TemporaryDirectory() as d:
    p = Path(d)
    (p/'switch.h').write_text(header)
    (p/'test.c').write_text(test)
    subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror',
                    '-I'+str(p),'-I'+str(root/'source'),str(root/'source/browser.c'),
                    str(p/'test.c'),'-o',str(p/'test')],check=True)
    subprocess.run([str(p/'test')],check=True)
