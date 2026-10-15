"""Run the actual C layout/navigation functions without a Switch or SDL runtime."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[1]
s=(root/'source/main.c').read_text()
parts=[s[s.index('static int selected='):s.index('static SDL_Color color')],
       s[s.index('static SDL_Rect rect_for'):s.index('static void render')],
       s[s.index('static void ensure_visible(void) {'):s.index('static int load_fonts')]]
test='''
#include <assert.h>
#include <stdio.h>
typedef struct { int x,y,w,h; } SDL_Rect;
static int site_count=21;
#define SITE_COUNT 21
#define WECHAT_SITE_INDEX 16
#define SITE_GRID_COUNT (site_count-1)
#define ACTION_ADD (-1)
#define ACTION_EDIT (-2)
#define ACTION_DELETE (-3)
#define ACTION_MODE (-4)
static int manage_target=-1;
static struct { int count; } custom={0};
static int total_sites(void){return site_count;}
static int grid_count(void){return site_count-1;}
'''+''.join(parts)+'''
int main(void) {
    for(int count=21;count<=1005;count+=984) {
        site_count=count;custom.count=count-21;manage_target=-1;scroll_y=0;selected=2;
        for(int step=0;step<(SITE_GRID_COUNT+3)/4+5;step++) {
            move(0,1);
            if(selected==ACTION_ADD)break;
            SDL_Rect r=rect_for(selected);
            assert(r.y>=list_view.y+4);
            assert(r.y+r.h<=list_view.y+list_view.h-4);
            assert(selected>=2 && selected<site_count+2);
            assert(selected!=WECHAT_SITE_INDEX+2);
            assert(hit_test(r.x+10,r.y+10)==selected);
        }
        assert(selected==ACTION_ADD);
        assert(hit_test(70,590)==ACTION_ADD);
        move(1,0);assert(selected==ACTION_EDIT);move(1,0);assert(selected==ACTION_DELETE);
        move(0,-1);assert(selected>=2);
        scroll_by(1000000);assert(scroll_y==scroll_max());
        scroll_by(-1000000);assert(scroll_y==0);
        selected=0;move(0,-1);assert(selected==WECHAT_SITE_INDEX+2);
        move(0,1);assert(selected==1);
        SDL_Rect r=rect_for(WECHAT_SITE_INDEX+2);
        assert(hit_test(r.x+10,r.y+10)==WECHAT_SITE_INDEX+2);
        selected=WECHAT_SITE_INDEX+2;move(-1,0);assert(selected==ACTION_MODE);
        r=rect_for(ACTION_MODE);assert(hit_test(r.x+10,r.y+10)==ACTION_MODE);
        move(1,0);assert(selected==WECHAT_SITE_INDEX+2);
        move(-1,0);move(0,1);assert(selected==0);
    }
    puts("Navigation passed: 21 and 1005 sites, focus visibility, bounds, touch hit regions, WeChat shortcut");
}
'''
with tempfile.TemporaryDirectory() as d:
    p=Path(d);(p/'test.c').write_text(test)
    subprocess.run(['cc','-Wall','-Wextra','-Werror',str(p/'test.c'),'-o',str(p/'test')],check=True)
    subprocess.run([str(p/'test')],check=True)
