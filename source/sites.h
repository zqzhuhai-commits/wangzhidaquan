#ifndef SITES_H
#define SITES_H
/* Built-in links; no configuration file required. */
typedef struct { const char *name,*url,*tag; unsigned int color; } Site;
static const Site sites[] = {
 {"百度", "https://www.baidu.com/", "搜索", 0x367bf5},
 {"必应", "https://cn.bing.com/", "搜索", 0x13a9a0},
 {"哔哩哔哩", "https://www.bilibili.com/", "视频", 0xec78a0},
 {"腾讯视频", "https://v.qq.com/", "视频", 0x19ac82},
 {"爱奇艺", "https://www.iqiyi.com/", "视频", 0x55a82e},
 {"优酷", "https://www.youku.com/", "视频", 0x2786dc},
 {"知乎", "https://www.zhihu.com/", "社区", 0x3479df},
 {"微博", "https://weibo.com/", "社区", 0xe45b45},
 {"央视网", "https://www.cctv.com/", "资讯", 0xc84d59},
 {"人民网", "https://www.people.com.cn/", "资讯", 0xbf5444},
 {"京东", "https://www.jd.com/", "购物", 0xe44c55},
 {"淘宝", "https://www.taobao.com/", "购物", 0xf28b32},
 {"4399小游戏", "https://www.4399.com/", "小游戏", 0xef8c31},
 {"4399在线玩", "https://h.4399.com/", "H5小游戏", 0x22a67b},
 {"7k7k小游戏", "https://m.7k7k.com/", "小游戏", 0x6f70df},
 {"17yy小游戏", "https://h.17yy.com/", "H5小游戏", 0xe46c8c},
 {"微信文件传输助手", "https://filehelper.weixin.qq.com/", "文件传输", 0x18aa59}
};
#define WECHAT_SITE_INDEX 16
#define SITE_GRID_COUNT (SITE_COUNT-1)
#define SITE_COUNT ((int)(sizeof(sites)/sizeof(sites[0])))
#endif
