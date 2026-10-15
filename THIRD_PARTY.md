# Third-party components

The homebrew is built with devkitPro's Switch portlibs. Upstream licenses remain applicable.

| Component | License / upstream |
| --- | --- |
| libnx | ISC — https://github.com/switchbrew/libnx |
| SDL2 | zlib — https://github.com/libsdl-org/SDL/tree/SDL2 |
| SDL2_ttf | zlib — https://github.com/libsdl-org/SDL_ttf/tree/SDL2 |
| FreeType | FreeType License — https://freetype.org/license.html |
| HarfBuzz | MIT — https://github.com/harfbuzz/harfbuzz/blob/main/COPYING |
| Mesa / Nouveau | MIT and individual compatible licenses — https://docs.mesa3d.org/license.html |
| libpng | libpng license — http://www.libpng.org/pub/png/src/libpng-LICENSE.txt |
| zlib | zlib — https://zlib.net/zlib_license.html |
| bzip2 | bzip2 license — https://sourceware.org/bzip2/ |
| GCC runtimes | GPL with GCC Runtime Library Exception — https://www.gnu.org/licenses/gcc-exception-3.1.html |

Portions of this software are copyright © The FreeType Project (www.freetype.org). All rights reserved.

Build tools are not bundled in the binary download. hacBrewPack is GPL-2.0; its source and license are at https://github.com/rlaphoenix/hacBrewPack. devkitPro tools are available through https://devkitpro.org/ and their corresponding source repositories.

The Makefile is adapted from the official switchbrew SDL2 example. The NPDM configuration follows hacBrewPack's template with current switch-tools field names and debug flags disabled. The supplied app icon is user-provided artwork.
