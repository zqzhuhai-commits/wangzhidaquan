# Switch 网址大全 · Switch Web Nav

简易 Nintendo Switch Homebrew 网址导航。输入网址并确认，或选择站点后，调用 Switch 内置网页组件打开页面。退出网页组件后回到导航。

## 1.2.1-test 网页组件诊断版

本分支源码加入实验性 **Wi-Fi 网页模式**，用于排查“所有网址均提示无法使用此功能”。**尚未经 Switch 23.0.1 或其他版本实机验证，不保证能解决该提示。** 此次未更新 `dist/` 二进制或 Releases；下面原有下载链接仍为 1.2.0，不含这个功能。测试版需从本分支重新编译，并用原有打包脚本生成 NSP。

- 默认使用原来的普通网页模式。按 **L** 或点击顶部模式按钮切换为 **Wi-Fi（实验）**，再打开同一个网址进行对比；再次切换可恢复普通模式。选择只对本次运行有效，不会自动重试其他模式。
- 手柄也可从网址输入框按上进入微信按钮，再按左进入模式按钮，按 A 切换。
- 普通模式调用 `webPageCreate` / `webConfigShow`；Wi-Fi 模式调用独立的 `webWifiCreate` / `webWifiShow`。Wi-Fi 模式不设置连接测试 URL，因此不指定或修改网络配置、不更改 DNS。它原本是联网认证页面入口，不是完整通用浏览器。
- 返回后底部显示诊断：`R` 为调用结果，非零表示失败；普通模式 `exit=7` 表示组件显示了系统错误弹窗。Wi-Fi 的 `reply` 为组件返回结果，非零表示组件报告错误。**`R=0` / `reply=0` 不能证明目标页面已成功加载。** Wi-Fi 返回结构没有普通模式的退出原因字段，不会伪造此信息。
- 每次调用后尝试追加保存 `/switch/SwitchWebNav/browser.log`，内容仅有系统版本、应用运行类型、模式、失败阶段、调用结果、组件结果及可用的退出原因，不记录访问网址或网页内容。保存失败会在底部提示“日志未保存”。日志可手动删除，不影响书签。
- 测试时分别以两种模式打开同一个简单网页，记下页面是否出现、是否弹窗和支持代码；退出后提供 `browser.log`。仅凭错误提示不能确定系统禁用了某个入口。视频、登录、云游戏等兼容性另行验证。

接口结构与限制参考 [libnx 网页组件文档](https://switchbrew.github.io/libnx/web_8h.html)。HTML 预览仍只模拟导航界面，不能测试 Switch 网页组件。

## 下载

- [Releases 正式下载](https://github.com/zqzhuhai-commits/wangzhidaquan/releases/latest)
- [独立 NSP 安装包](https://github.com/zqzhuhai-commits/wangzhidaquan/releases/download/v1.2.0/SwitchWebNav.nsp)
- [NRO 版本](https://github.com/zqzhuhai-commits/wangzhidaquan/releases/download/v1.2.0/SwitchWebNav.nro)
- [HTML 界面预览](preview/index.html)（下载后在电脑浏览器打开）

版本 **1.2.0**，Title ID **0100C0DE57560000**。

**NSP 是独立应用包，不是指向外置 NRO 的转发器。** 安装后无需在 SD 卡保留 NRO、config.ini、cacert.pem 或密钥文件。用户新增的网址会自动保存在 SD 卡的 `/switch/SwitchWebNav/bookmarks.tsv` 中。NRO 另行提供，供 hbmenu 用户使用。

## 网站和操作

输入框下方保留百度、必应、哔哩哔哩、腾讯视频、爱奇艺、优酷、知乎、微博、央视网、人民网、京东、淘宝。

新增一行在线小游戏：**4399 小游戏、4399 在线玩（H5）、7k7k 小游戏、17yy 小游戏（H5）**。顶部独立加入 [微信文件传输助手](https://filehelper.weixin.qq.com/)。另外新增一行网页版云游戏：腾讯 START、网易云游戏、腾讯先锋、咪咕快游。共 21 个默认入口。

| 操作 | 功能 |
| --- | --- |
| 方向键 / 左摇杆 | 移动焦点；长按连续移动，列表自动跟随滚动 |
| A | 确认、打开所选站点 |
| X | 系统键盘输入网址，键盘确认后直接打开 |
| B | 焦点回到网址输入框 |
| ZL / ZR | 列表向上 / 向下翻页 |
| L（测试分支） | 切换普通 / Wi-Fi（实验）网页模式 |
| 触屏点击 | 输入网址或进入站点 |
| 触屏上下拖动 | 滚动网站列表，拖动不会误触打开站点 |
| + | 退出导航前端 |

网站列表使用独立滚动区域，没有固定屏幕行数限制。新增源码中的站点后，滚动范围自动计算；已用 1,005 个站点的布局进行检查。当前内置 21 个链接，滚到底部后停止，不会重复生成相同站点。

输入网址自动补全 `https://`；支持 HTTP、HTTPS、IPv4、端口和中文路径。主机名限 ASCII，中文域名使用 Punycode；目前不支持 IPv6 字面地址。系统键盘输入最多 500 个字符。

## 自定义网址

列表下方新增 **添加自定义网址 / 编辑选中网址 / 删除选中网址**：

1. 点击“添加自定义网址”，依次输入名称和网址，确认保存。
2. 自定义网址显示在“我的网址”分组中，和其他站点一样可点击打开、上下滚动。
3. 先选中一个自定义网址，再点击编辑或删除；也可以使用 Y 编辑、− 删除。删除前显示确认框（A 确认、B 取消）。
4. 重启应用后自动恢复。更新 NSP 时不要删除 SD 卡中的书签数据文件。

名称最多 32 字，网址最多 500 字符。保存前验证网址、检查重复；写入失败时不会保留未保存的内存更改。无需自己准备数据文件，首次保存时自动创建。书签只保存在本机，不会上传到 GitHub 或其他服务器。HTML 预览使用当前浏览器的本地存储，与 Switch 数据独立。

网页版云游戏仅提供官方入口；Switch 内置网页组件可能不支持所需的视频编解码、WebRTC、手柄接口或登录流程，不保证能实际运行云游戏。

## 安装与运行

### NSP

下载 `dist/SwitchWebNav.nsp`，在可安装 Homebrew NSP 的 Switch 环境中使用已有安装工具安装，之后从主界面图标启动。安装包包含程序及图标，无需额外文件。原厂未修改系统无法直接运行此 Homebrew 应用。

### NRO

将 `dist/SwitchWebNav.nro` 放入 SD 卡 `/switch/SwitchWebNav/SwitchWebNav.nro`。按住 R 启动已安装的游戏进入完整应用模式 hbmenu，再运行本程序。相册小程序模式下可以显示前端，但打开网页前会提示使用完整应用模式。

Switch 内置网页组件与桌面浏览器的兼容性不同。小游戏入口优先提供 H5 页面；是否可运行、是否需要触屏、是否需要登录，由站点与系统网页组件决定。此项目不包含 Flash 播放器，也不把手柄按钮映射成网页游戏按键。微信登录和文件操作同样依赖系统网页组件支持。

## 构建状态

- 已用 devkitA64 r30 / GCC 16.1.0、libnx 4.12.0、SDL2 2.28.5、SDL2_ttf 2.22.0 成功交叉编译。
- NSP 已检查 PFS0 结构、三个 NCA 的内容哈希、NCA 头部、Title ID、大小及区段边界。
- 网址逻辑检查通过；导航检查覆盖 21 / 1,005 个站点、焦点可见性、滚动上下界及触屏命中区域。
- 自定义网址已检查保存恢复、编辑、删除、重复与无效数据、129 条记录及写入失败回滚。
- HTML JavaScript 语法检查通过。
- **尚未经 Switch 实机验证；系统网页组件启动、站点登录、小游戏和视频兼容性需在实机确认。**

## 本地编译

安装 devkitPro 的 `switch-dev`、`switch-sdl2` 和 `switch-sdl2_ttf`（包含其依赖）：

```sh
export DEVKITPRO=/opt/devkitpro
export PATH="$DEVKITPRO/portlibs/switch/bin:$DEVKITPRO/tools/bin:$DEVKITPRO/devkitA64/bin:$PATH"
make -j2
```

输出 `SwitchWebNav.nro` 和用于独立 NSP 打包的 ELF / NACP。仓库附带的 GitHub Actions 构建 NRO，不需要私密文件。

独立 NSP 打包需要在本地安装 [hacBrewPack](https://github.com/rlaphoenix/hacBrewPack)，并提供自己的本地 keyset：

```sh
python3 tools/package_nsp.py \
  --keys /absolute/private/path/prod.keys \
  --hacbrewpack /absolute/path/hacbrewpack
```

输出 `dist/SwitchWebNav.nsp`。打包脚本在临时目录提取所需的两个条目，完成后自动清理；不要将密钥或临时打包目录提交到仓库。

## 修改站点与检查

编辑 `source/sites.h`。微信快捷入口由 `WECHAT_SITE_INDEX` 指定；在其后追加站点即可继续扩展滚动列表。站点总数由数组自动计算。HTML 预览为独立界面副本，源码站点有更改时须同步修改。

```sh
cc -Wall -Wextra -Werror source/url.c tests/test_url.c -o /tmp/webnav-test
/tmp/webnav-test
python3 tests/test_navigation.py
python3 tests/test_browser.py
cc -Wall -Wextra -Werror source/url.c source/bookmarks.c tests/test_bookmarks.c -o /tmp/bookmarks-test
/tmp/bookmarks-test
```

## 参考与致谢

- [libnx](https://github.com/switchbrew/libnx)：系统键盘、网页组件、手柄与触屏接口。
- [switchbrew 官方 SDL2 示例](https://github.com/switchbrew/switch-examples/tree/master/graphics/sdl2/sdl2-simple)：Makefile 基础。
- [hacBrewPack](https://github.com/rlaphoenix/hacBrewPack)：独立 NSP 打包；NPDM 配置参考其模板并适配现行 npdmtool 字段。
- SDL2、SDL2_ttf、FreeType、HarfBuzz、Mesa 等组件保留各自上游许可；见 `THIRD_PARTY.md`。
- 应用图标使用用户提供的截图；中文使用主机系统共享字体，不随包分发系统字体。
