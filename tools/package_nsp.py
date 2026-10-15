#!/usr/bin/env python3
"""Package the compiled ELF as a self-contained homebrew NSP (not a forwarder)."""
import argparse
import os
import re
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
TITLE_ID = '0100c0de57560000'

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--keys', required=True, type=Path, help='Local private keyset; never commit it')
    p.add_argument('--hacbrewpack', default='hacbrewpack')
    p.add_argument('--output', type=Path, default=ROOT/'dist'/'SwitchWebNav.nsp')
    args = p.parse_args()
    keys = args.keys.expanduser().resolve()
    if not keys.is_file(): p.error('Keyset file does not exist')
    for filename in ['SwitchWebNav.elf', 'SwitchWebNav.nacp', 'icon.jpg', 'packaging/npdm.json']:
        if not (ROOT/filename).is_file(): p.error('Missing '+filename+'; run make first')
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    dkp = env.get('DEVKITPRO','/opt/devkitpro')
    env['PATH'] = f'{dkp}/tools/bin:' + env.get('PATH','')
    with tempfile.TemporaryDirectory(prefix='webnav-nsp-') as tmp:
        tmp = Path(tmp)
        exefs,control = tmp/'exefs',tmp/'control'
        exefs.mkdir(); control.mkdir()
        subprocess.run(['elf2nso',str(ROOT/'SwitchWebNav.elf'),str(exefs/'main')],check=True,env=env)
        subprocess.run(['npdmtool',str(ROOT/'packaging/npdm.json'),str(exefs/'main.npdm')],check=True,env=env)
        nacp = bytearray((ROOT/'SwitchWebNav.nacp').read_bytes())
        # No account selection or save allocation is required by this stateless launcher.
        nacp[0x3025] = 0
        for offset in [0x3080,0x3088,0x3090,0x3098]: nacp[offset:offset+8] = bytes(8)
        (control/'control.nacp').write_bytes(nacp)
        for lang in ['AmericanEnglish','BritishEnglish','SimplifiedChinese','TraditionalChinese']:
            shutil.copyfile(ROOT/'icon.jpg',control/f'icon_{lang}.dat')
        # Older hacBrewPack readers cannot safely parse newer, longer key names.
        pairs = dict(re.findall(r'^\s*(\w+)\s*=\s*([0-9a-fA-F]+)\s*$',keys.read_text(),re.M))
        needed = {'header_key':64,'key_area_key_application_00':32}
        if any(len(pairs.get(name,''))!=size for name,size in needed.items()):
            raise SystemExit('Keyset must contain header_key and key_area_key_application_00')
        minimal_keys = tmp/'keys.dat'
        minimal_keys.write_text(''.join(f'{name} = {pairs[name]}\n' for name in needed))
        minimal_keys.chmod(0o600)
        log = subprocess.run([args.hacbrewpack,'--keyset',str(minimal_keys),
            '--titleid',TITLE_ID,'--titlename','Switch 网址大全','--titlepublisher','ZQZH',
            '--exefsdir',str(exefs),'--controldir',str(control),
            '--nspdir',str(tmp/'out'),'--ncadir',str(tmp/'nca'),
            '--tempdir',str(tmp/'temp'),'--backupdir',str(tmp/'backup'),
            '--noromfs','--nologo'],cwd=tmp,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        # Do not dump packer logs; third-party key readers may log unsupported private entries.
        if log.returncode:
            raise SystemExit('NSP packaging failed. Check tool version and required header/application key entries locally.')
        candidates = list((tmp/'out').glob('*.nsp'))
        if len(candidates)!=1: raise SystemExit('Packer did not produce exactly one NSP')
        shutil.copyfile(candidates[0],output)
    print(f'Built {output.name} ({output.stat().st_size:,} bytes), Title ID {TITLE_ID}')

if __name__ == '__main__': main()
