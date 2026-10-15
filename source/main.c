#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"
#include "sites.h"
#define WIDTH 1280
#define HEIGHT 720
static SDL_Renderer *renderer;
static TTF_Font *latin[3], *chinese[3];
static const int sizes[]={19,26,38};
static char address[URL_CAP]="", status[256]="输入网址，或选择下方站点";
static int selected=0;
static int scroll_y=0;
static const SDL_Rect list_view={48,258,1184,340};
static int grid_pos(int site) { return site<WECHAT_SITE_INDEX?site:site-1; }
static int site_at(int pos) { return pos<WECHAT_SITE_INDEX?pos:pos+1; }
static int scroll_max(void) { int rows=(SITE_GRID_COUNT+3)/4; int bottom=262+(rows-1)*98+28+82;return bottom>594?bottom-594:0; }
static void scroll_by(int delta) { scroll_y+=delta;if(scroll_y<0)scroll_y=0;if(scroll_y>scroll_max())scroll_y=scroll_max(); }
static SDL_Color color(unsigned c) { return (SDL_Color){c>>16,(c>>8)&255,c&255,255}; }
static void fill(SDL_Rect r,unsigned c) { SDL_Color a=color(c); SDL_SetRenderDrawColor(renderer,a.r,a.g,a.b,255); SDL_RenderFillRect(renderer,&r); }
static void panel(SDL_Rect r,unsigned c) {
    const int rad=14; fill((SDL_Rect){r.x+rad,r.y,r.w-2*rad,r.h},c);
    fill((SDL_Rect){r.x,r.y+rad,r.w,r.h-2*rad},c);
    for(int y=0;y<rad;y++) for(int x=0;x<rad;x++) if ((x-rad)*(x-rad)+(y-rad)*(y-rad)<=rad*rad) {
        fill((SDL_Rect){r.x+x,r.y+y,1,1},c); fill((SDL_Rect){r.x+r.w-1-x,r.y+y,1,1},c);
        fill((SDL_Rect){r.x+x,r.y+r.h-1-y,1,1},c); fill((SDL_Rect){r.x+r.w-1-x,r.y+r.h-1-y,1,1},c);
    }
}
static void text(const char *s,int x,int y,int size,unsigned c,int maxw) {
    if(!s || !*s) return;
    int cn=0; for(const unsigned char *p=(const unsigned char *)s;*p;p++) if(*p>=128)cn=1;
    TTF_Font *f=cn?chinese[size]:latin[size];
    SDL_Surface *surface=TTF_RenderUTF8_Blended(f,s,color(c)); if(!surface)return;
    SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer,surface);
    int w=surface->w, h=surface->h; if(w>maxw)w=maxw;
    SDL_Rect src={0,0,w,h}, dst={x,y,w,h};
    if(texture)SDL_RenderCopy(renderer,texture,&src,&dst);
    SDL_DestroyTexture(texture); SDL_FreeSurface(surface);
}
static SDL_Rect rect_for(int index) {
    if(index==WECHAT_SITE_INDEX+2)return (SDL_Rect){922,99,302,42};
    if(index==0)return (SDL_Rect){56,157,958,70};
    if(index==1)return (SDL_Rect){1030,157,194,70};
    int n=grid_pos(index-2); return (SDL_Rect){56+(n%4)*296,262+(n/4)*98+(n>=12?28:0)-scroll_y,280,82};
}
static void render(void) {
    fill((SDL_Rect){0,0,WIDTH,HEIGHT},0xf2f5fa);
    panel((SDL_Rect){56,40,50,50},0x2474ef); text("N",69,44,2,0xffffff,40);
    text("网址导航",122,38,2,0x172b4d,500);
    text("SWITCH WEB NAV",124,88,0,0x78869a,400);
    panel((SDL_Rect){1050,52,174,35},0xe1eaf7);char count_label[64];snprintf(count_label,sizeof(count_label),"站点入口 · %d",SITE_COUNT);text(count_label,1068,58,0,0x39638f,160);
    for(int i=0;i<2;i++) {
        SDL_Rect r=rect_for(i); if(selected==i)panel((SDL_Rect){r.x-4,r.y-4,r.w+8,r.h+8},0x2e83f5);
        panel(r,i==1?0x2474ef:0xffffff);
    }
    text(address[0]?address:"输入网址，例如 baidu.com",78,177,1,address[0]?0x172b4d:0x8794a7,912);
    text("打开网页",1072,177,1,0xffffff,150);
    text("常用站点",56,233,1,0x243a59,400);text("上下滚动 · 选择后打开",920,236,0,0x78869a,320);
    SDL_Rect utility=rect_for(WECHAT_SITE_INDEX+2);
    if(selected==WECHAT_SITE_INDEX+2)panel((SDL_Rect){utility.x-4,utility.y-4,utility.w+8,utility.h+8},0x2e83f5);
    panel(utility,0xe0f3e7);text("微信文件传输助手",utility.x+24,utility.y+10,0,0x18884c,260);
    SDL_RenderSetClipRect(renderer,&list_view);
    text("在线小游戏",56,554-scroll_y,0,0x243a59,400);
    for(int i=0;i<SITE_COUNT;i++) {
        if(i==WECHAT_SITE_INDEX)continue;
        SDL_Rect r=rect_for(i+2);
        if(r.y+r.h<list_view.y || r.y>list_view.y+list_view.h)continue;
        if(selected==i+2)panel((SDL_Rect){r.x-4,r.y-4,r.w+8,r.h+8},0x2e83f5);
        panel(r,0xffffff); panel((SDL_Rect){r.x+15,r.y+17,48,48},sites[i].color);
        char first[5]={0}; unsigned char c=(unsigned char)sites[i].name[0]; int bytes=c<128?1:(c<224?2:3); memcpy(first,sites[i].name,bytes);
        text(first,r.x+26,r.y+26,1,0xffffff,38);text(sites[i].name,r.x+77,r.y+13,0,0x243a59,192);
        text(sites[i].tag,r.x+78,r.y+49,0,0x8897ac,160);
    }
    SDL_RenderSetClipRect(renderer,NULL);
    if(scroll_max()>0) {
        int track=332,thumb=track*340/(340+scroll_max());
        fill((SDL_Rect){1234,262,4,track},0xe0e6ef);fill((SDL_Rect){1234,262+(track-thumb)*scroll_y/scroll_max(),4,thumb},0x89aee0);
    }
    text(status,56,613,0,0x576c89,1168);
    fill((SDL_Rect){56,652,1168,1},0xdce3ee);
    text("方向键 / 左摇杆 选择     A 确认     X 输入网址     B 返回输入框   ZL/ZR 翻页",56,673,0,0x60728b,1000);
    text("+ 退出",1140,673,0,0x60728b,100);
    SDL_RenderPresent(renderer);
}
static void open_url(const char *raw) {
    char url[URL_CAP];
    if(!normalize_url(raw,url,sizeof(url))) { snprintf(status,sizeof(status),"请输入有效的 HTTP / HTTPS 网址（域名或 IPv4，可带端口）");return; }
    if(appletGetAppletType()!=AppletType_Application && appletGetAppletType()!=AppletType_SystemApplication) {
        snprintf(status,sizeof(status),"请按住 R 启动一个游戏进入 hbmenu，以完整应用模式运行本程序");return;
    }
    snprintf(status,sizeof(status),"正在打开系统网页组件…");render();
    WebCommonConfig config; WebCommonReply reply;
    Result rc=webPageCreate(&config,url);
    if(R_SUCCEEDED(rc))rc=webConfigSetWhitelist(&config,"^https?://.*");
    if(R_SUCCEEDED(rc))rc=webConfigSetFooter(&config,true);
    if(R_SUCCEEDED(rc))rc=webConfigSetPointer(&config,true);
    if(R_SUCCEEDED(rc))rc=webConfigShow(&config,&reply);
    if(R_FAILED(rc))snprintf(status,sizeof(status),"网页组件启动失败：0x%08X，请检查运行模式和系统环境",(unsigned)rc);
    else snprintf(status,sizeof(status),"已返回网址导航");
}
static void edit_url(void) {
    SwkbdConfig kbd; Result rc=swkbdCreate(&kbd,0);
    if(R_FAILED(rc)) { snprintf(status,sizeof(status),"键盘启动失败：0x%08X",(unsigned)rc);return; }
    char entered[URL_CAP]={0};
    swkbdConfigMakePresetDefault(&kbd);
    swkbdConfigSetInitialText(&kbd,address);
    swkbdConfigSetHeaderText(&kbd,"输入网址");
    swkbdConfigSetGuideText(&kbd,"例如 https://www.baidu.com");
    swkbdConfigSetOkButtonText(&kbd,"打开");
    swkbdConfigSetStringLenMax(&kbd,500);
    rc=swkbdShow(&kbd,entered,sizeof(entered)); swkbdClose(&kbd);
    if(R_SUCCEEDED(rc)) { snprintf(address,sizeof(address),"%s",entered);open_url(address); }
}
static void activate(void) { if(selected==0)edit_url();else if(selected==1) { if(address[0])open_url(address);else edit_url(); } else open_url(sites[selected-2].url); }
static void ensure_visible(void) {
    if(selected<2 || selected==WECHAT_SITE_INDEX+2)return;
    SDL_Rect r=rect_for(selected);
    if(r.y<list_view.y+4)scroll_by(r.y-list_view.y-4);
    else if(r.y+r.h>list_view.y+list_view.h-4)scroll_by(r.y+r.h-list_view.y-list_view.h+4);
}
static void move(int dx,int dy) {
    if(selected==WECHAT_SITE_INDEX+2) { if(dy>0)selected=1;else if(dx)selected=dx<0?0:1;return; }
    if(selected<2) { if(dy>0)selected=selected==0?2:5;else if(dy<0)selected=WECHAT_SITE_INDEX+2;else if(dx)selected=1-selected; }
    else { int n=grid_pos(selected-2),row=n/4,col=n%4;
        if(dy<0 && row==0)selected=col==3?1:0;
        else { if(dx)col=(col+dx+4)%4; if(dy && row+dy>=0 && row+dy<(SITE_GRID_COUNT+3)/4)row+=dy;int pos=row*4+col;if(pos>=SITE_GRID_COUNT)pos=SITE_GRID_COUNT-1;selected=2+site_at(pos); }
    }
    ensure_visible();
}
static int hit_test(int x,int y) {
    for(int i=0;i<SITE_COUNT+2;i++) {
        if(i>=2 && i!=WECHAT_SITE_INDEX+2 && (y<list_view.y || y>=list_view.y+list_view.h))continue;
        SDL_Rect r=rect_for(i);if(x>=r.x&&x<r.x+r.w&&y>=r.y&&y<r.y+r.h)return i;
    }
    return -1;
}
static int load_fonts(void) {
    PlFontData a,b; if(R_FAILED(plGetSharedFontByType(&a,PlSharedFontType_Standard)))return 0;
    if(R_FAILED(plGetSharedFontByType(&b,PlSharedFontType_ChineseSimplified)))return 0;
    for(int i=0;i<3;i++) { latin[i]=TTF_OpenFontRW(SDL_RWFromConstMem(a.address,(int)a.size),1,sizes[i]); chinese[i]=TTF_OpenFontRW(SDL_RWFromConstMem(b.address,(int)b.size),1,sizes[i]); if(!latin[i]||!chinese[i])return 0; }
    return 1;
}
int main(int argc,char **argv) {
    (void)argc;(void)argv;
    Result rc=plInitialize(PlServiceType_User);
    SDL_Window *window=NULL; int sdl=0,ttf=0;
    if(R_FAILED(rc))goto failed;
    if(SDL_Init(SDL_INIT_VIDEO)<0)goto failed;
    sdl=1;
    if(TTF_Init()<0)goto failed;
    ttf=1;
    window=SDL_CreateWindow("Switch Web Nav",0,0,WIDTH,HEIGHT,0);if(!window)goto failed;
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);if(!renderer)goto failed;
    SDL_RenderSetLogicalSize(renderer,WIDTH,HEIGHT);
    if(!load_fonts())goto failed;
    padConfigureInput(1,HidNpadStyleSet_NpadStandard);PadState pad;padInitializeDefault(&pad);
    hidInitializeTouchScreen();bool was_touch=false,dragged=false,drag_list=false;
    int touch_last_y=0,touch_start_y=0,touch_start_x=0,touch_target=-1;
    u64 last_dir=0;unsigned repeat_at=0;
    while(appletMainLoop()) {
        SDL_Event e;while(SDL_PollEvent(&e)) { if(e.type==SDL_QUIT)goto done; if(e.type==SDL_MOUSEWHEEL)scroll_by(-e.wheel.y*48); }
        padUpdate(&pad);u64 down=padGetButtonsDown(&pad);
        if(down&HidNpadButton_Plus)break;
        u64 held=padGetButtons(&pad),dir=held&(HidNpadButton_AnyLeft|HidNpadButton_AnyRight|HidNpadButton_AnyUp|HidNpadButton_AnyDown);
        unsigned now=SDL_GetTicks();
        if(dir && (dir!=last_dir || (int)(now-repeat_at)>=0)) {
            if(dir&HidNpadButton_AnyLeft)move(-1,0);else if(dir&HidNpadButton_AnyRight)move(1,0);else if(dir&HidNpadButton_AnyUp)move(0,-1);else move(0,1);
            repeat_at=now+(dir!=last_dir?360:110);
        }last_dir=dir;
        if(down&HidNpadButton_ZR)scroll_by(240);
        if(down&HidNpadButton_ZL)scroll_by(-240);
        if(down&HidNpadButton_B)selected=0;
        if(down&HidNpadButton_X)edit_url();else if(down&HidNpadButton_A)activate();
        HidTouchScreenState touch={0};hidGetTouchScreenStates(&touch,1);
        bool touched=touch.count>0;
        if(touched) {
            int x=touch.touches[0].x,y=touch.touches[0].y;
            if(!was_touch) { touch_start_x=x;touch_last_y=touch_start_y=y;touch_target=hit_test(x,y);dragged=false;drag_list=y>=list_view.y&&y<list_view.y+list_view.h; }
            else { if(abs(y-touch_start_y)>10 || abs(x-touch_start_x)>10)dragged=true;if(dragged&&drag_list)scroll_by(touch_last_y-y);touch_last_y=y; }
        } else if(was_touch && !dragged && touch_target>=0) { selected=touch_target;activate(); }
        was_touch=touched;render();
    }
    goto done;
failed:
    /* Keep startup errors visible even when graphics/font initialization fails. */
    if(renderer){SDL_DestroyRenderer(renderer);renderer=NULL;}
    if(window){SDL_DestroyWindow(window);window=NULL;}
    for(int i=0;i<3;i++){if(latin[i])TTF_CloseFont(latin[i]);if(chinese[i])TTF_CloseFont(chinese[i]);latin[i]=chinese[i]=NULL;}
    if(ttf){TTF_Quit();ttf=0;}if(sdl){SDL_Quit();sdl=0;}
    consoleInit(NULL);printf("Switch Web Nav initialization failed.\npl: %08X\nPress + to exit.\n",(unsigned)rc);
    padConfigureInput(1,HidNpadStyleSet_NpadStandard);PadState errorpad;padInitializeDefault(&errorpad);
    while(appletMainLoop()){padUpdate(&errorpad);if(padGetButtonsDown(&errorpad)&HidNpadButton_Plus)break;consoleUpdate(NULL);}consoleExit(NULL);
done:
    for(int i=0;i<3;i++){if(latin[i])TTF_CloseFont(latin[i]);if(chinese[i])TTF_CloseFont(chinese[i]);}
    if(renderer)SDL_DestroyRenderer(renderer);
    if(window)SDL_DestroyWindow(window);
    if(ttf)TTF_Quit();
    if(sdl)SDL_Quit();
    if(R_SUCCEEDED(rc))plExit();
    return 0;
}
