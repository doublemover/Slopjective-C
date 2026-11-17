"""Native Linux architecture and independently pinned bootstrap assets."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import platform
import re
from urllib.parse import urlsplit


ROOT = Path(__file__).resolve().parents[2]
HOSTS = {
    'linux-x86_64': ('x86_64', 'x86_64-unknown-linux-gnu', 62),
    'linux-aarch64': ('aarch64', 'aarch64-unknown-linux-gnu', 183),
}
ALIASES = {'x86_64': 'linux-x86_64', 'amd64': 'linux-x86_64',
           'aarch64': 'linux-aarch64', 'arm64': 'linux-aarch64'}


def native_linux_host() -> str:
    if platform.system() != 'Linux' or platform.machine().lower() not in ALIASES:
        raise ValueError('native Linux development requires x86-64 or AArch64')
    return ALIASES[platform.machine().lower()]


def host_identity(host: str | None = None, *, require_native=False) -> dict:
    selected = host or native_linux_host()
    if selected not in HOSTS:
        raise ValueError('unsupported native Linux host: ' + selected)
    if require_native and selected != native_linux_host():
        raise ValueError('requested Linux architecture differs from the native host')
    machine, triple, elf_machine = HOSTS[selected]
    return {'host': selected, 'machine': machine, 'target_triple': triple,
            'elf_machine': elf_machine}


def native_target() -> str:
    return host_identity()['target_triple']


def native_loader() -> Path:
    path = Path('/lib/ld-linux-aarch64.so.1' if native_linux_host() == 'linux-aarch64'
                else '/lib64/ld-linux-x86-64.so.2').resolve(strict=True)
    require_elf(path)
    return path


def load(repo: Path = ROOT, host: str | None = None) -> dict:
    selected = host_identity(host)
    document = json.loads((repo / 'toolchains/linux-hosts.json').read_text())
    manifest = json.loads((repo / 'toolchains/llvm/manifest.json').read_text())
    expected = {'llvm': manifest['upstream']['tag'].removeprefix('llvmorg-'),
                **{name: manifest['host_tools'][name] for name in ('node', 'cmake', 'ccache')}}
    if (document.get('schema') != 'objc3c.linux-native-hosts.v1' or
            document.get('versions') != expected or set(document.get('hosts', {})) != set(HOSTS)):
        raise ValueError('Linux bootstrap host pins differ from the toolchain versions')
    for name, identity in document['hosts'].items():
        if any(identity.get(key) != value for key, value in host_identity(name).items() if key != 'host'):
            raise ValueError('Linux bootstrap host architecture differs from its identity')
        if set(identity.get('assets', {})) != {'llvm', 'node', 'cmake', 'ccache', 'ccache_signature', 'minisign', 'libsodium'}:
            raise ValueError('Linux bootstrap host asset inventory is incomplete')
        for asset in identity['assets'].values():
            url = urlsplit(asset.get('url', ''))
            if (url.scheme != 'https' or url.netloc not in {'github.com', 'nodejs.org', 'deb.debian.org'} or
                    not re.fullmatch('[0-9a-f]{64}', asset.get('sha256', '')) or
                    ('asset_id' in asset and (type(asset['asset_id']) is not int or asset['asset_id'] <= 0))):
                raise ValueError('Linux bootstrap asset is unpinned or uses an unapproved publisher')
        if set(identity.get('prefixes', {})) != {'llvm', 'node', 'cmake', 'ccache'} or any(
                value in ('.', '..') or not re.fullmatch(r'[A-Za-z0-9_.+-]+', value)
                for value in identity['prefixes'].values()):
            raise ValueError('Linux bootstrap prefix inventory is invalid')
    result = {**document['hosts'][selected['host']], **selected}
    result['versions'] = document['versions']
    result['ccache_minisign_public_key'] = document['ccache_minisign_public_key']
    return result


def require_elf(path: Path, host: str | None = None) -> None:
    identity = host_identity(host, require_native=True)
    with path.open('rb') as stream:
        header = stream.read(64)
    if (len(header) != 64 or header[:6] != b'\x7fELF\x02\x01' or
            int.from_bytes(header[18:20], 'little') != identity['elf_machine']):
        raise ValueError('bootstrap executable differs from the native ELF architecture: ' + str(path))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=ROOT)
    parser.add_argument('--host', choices=tuple(HOSTS))
    parser.add_argument('--expect-host', choices=tuple(HOSTS),
                        help='reject a workflow scheduled onto another architecture')
    parser.add_argument('--field')
    parser.add_argument('--elf', type=Path)
    args = parser.parse_args()
    if args.expect_host:
        host_identity(args.expect_host, require_native=True)
    selected = args.host or native_linux_host()
    if args.elf:
        require_elf(args.elf, selected)
        return
    value = load(args.repo, selected)
    if args.field:
        for part in args.field.split('.'):
            value = value[part]
        if not isinstance(value, (str, int)):
            parser.error('selected bootstrap field is not a scalar')
        print(value)
    else:
        print(json.dumps(value, sort_keys=True))


if __name__ == '__main__':
    main()
