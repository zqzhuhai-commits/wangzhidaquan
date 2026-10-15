"""Run the actual C layout/navigation functions without a Switch or SDL runtime."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[1]
s=(root/'source/main.c').read_text()
parts=[s[s.index('static int selected='):s.index('static SDL_Color color')],
       s[s.index('static SDL_Rect rect_for'):s.index('static void render')],
       s[s.index('static void ensure_visible'):s.index('static int load_fonts')]]
test='''
#include <assert.h>
#include <stdio.h>
typedef struct { int x,y,w,h; } SDL_Rect;
static int site_count=17;
#define SITE_COUNT site_count
#define WECHAT_SITE_INDEX 16
#define SITE_GRID_COUNT (SITE_COUNT-1)
'''+''.join(parts)+'''
int main(void) {
    for(int count=17;count<=1001;count+=984) {
        site_count=count;scroll_y=0;selected=2;
        for(int step=0;step<(SITE_GRID_COUNT+3)/4+5;step++) {
            move(0,1);
            SDL_Rect r=rect_for(selected);
            assert(r.y>=list_view.y+4);
            assert(r.y+r.h<=list_view.y+list_view.h-4);
            assert(selected>=2 && selected<SITE_COUNT+2);
            assert(selected!=WECHAT_SITE_INDEX+2);
            assert(hit_test(r.x+10,r.y+10)==selected);
        }
        scroll_by(1000000);assert(scroll_y==scroll_max());
        scroll_by(-1000000);assert(scroll_y==0);
        selected=0;move(0,-1);assert(selected==WECHAT_SITE_INDEX+2);
        move(0,1);assert(selected==1);
        SDL_Rect r=rect_for(WECHAT_SITE_INDEX+2);
        assert(hit_test(r.x+10,r.y+10)==WECHAT_SITE_INDEX+2);
    }
    puts("Navigation passed: 17 and 1001 sites, focus visibility, bounds, touch hit regions, WeChat shortcut");
}
'''
with tempfile.TemporaryDirectory() as d:
    p=Path(d);(p/'test.c').write_text(test)
    subprocess.run(['cc','-Wall','-Wextra','-Werror',str(p/'test.c'),'-o',str(p/'test')],check=True)
    subprocess.run([str(p/'test')],check=True)
