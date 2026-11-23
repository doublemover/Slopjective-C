"""Install a published preview pinned by an already reviewed release manifest."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile

from scripts.objc3c_toolchain.install_sdk import install
from scripts.objc3c_toolchain.linux_host import native_linux_host

DOWNLOADS = 'https://github.com/doublemover/Slopjective-C/releases/download'


def install_preview(manifest: Path, directory: Path, *, witness_log: Path | None = None) -> dict:
    index = json.loads(manifest.read_bytes())
    tag, host = index['tag'], native_linux_host()
    record = index['archives'][host]
    name = f'objc3-sdk-{tag}-{host}.tar.zst'
    if (not re.fullmatch(r'v[0-9]+\.[0-9]+\.[0-9]+(?:-[a-z0-9.-]+)?', tag) or
            record['file'] != name or not re.fullmatch(r'[a-f0-9]{64}', record['sha256'])):
        raise ValueError('invalid published preview archive coordinates')
    directory.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='preview-download-', dir=directory) as scratch:
        archive = Path(scratch) / name
        subprocess.run(['curl', '--fail', '--silent', '--show-error', '--location', '--retry', '3',
                        '--output', str(archive), f'{DOWNLOADS}/{tag}/{name}'], check=True)
        if archive.stat().st_size != record['bytes']:
            raise ValueError('downloaded preview archive length differs from the release manifest')
        destination = directory / 'sdk'
        initial = directory / 'initial SDK' if witness_log else destination
        result = install(archive, record['sha256'], initial)
        if (result['seal_sha256'] != record['seal_sha256'] or
                result['cost']['source_build_id'] != record['source_build_id']):
            raise ValueError('installed preview differs from the release manifest')
        if witness_log:
            initial.rename(destination)
            relocated = install(archive, record['sha256'], destination)
            if (relocated['seal_sha256'] != result['seal_sha256'] or
                    relocated['cost']['source_build_id'] != result['cost']['source_build_id'] or
                    relocated['cost']['prefix_reused'] is not True):
                raise ValueError('relocated preview did not readmit the same installed generation')
            root = destination / 'current'
            source = root / 'share/objc3c/examples/native/main.objc3'
            application = Path(scratch) / 'application'
            environment = {key: value for key, value in os.environ.items() if key not in (
                'LD_LIBRARY_PATH', 'LD_PRELOAD', 'OBJC3C_SDK_ROOT', 'OBJC3C_CLANG_FOUNDATION_ROOT')}
            with witness_log.open('w') as log:
                for command in ([root / 'bin/objc3c', source, '-O2', '-g', '-o', application], [application]):
                    subprocess.run(command, check=True, env=environment, stdout=log,
                                   stderr=subprocess.STDOUT, timeout=120)
            result = {**relocated, 'witness': {'status': 'PASS', 'source': 'native/main.objc3',
                      'log': str(witness_log), 'initial_install_cost': result['cost']}}
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--directory', type=Path, required=True)
    parser.add_argument('--witness-log', type=Path)
    options = parser.parse_args()
    print(json.dumps(install_preview(options.manifest, options.directory, witness_log=options.witness_log), indent=2))


if __name__ == '__main__':
    main()
