"""Install a published preview pinned by an already reviewed release manifest."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import subprocess
import tempfile

from scripts.objc3c_toolchain.install_sdk import install
from scripts.objc3c_toolchain.linux_host import native_linux_host

DOWNLOADS = 'https://github.com/doublemover/Slopjective-C/releases/download'


def install_preview(manifest: Path, directory: Path) -> dict:
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
        result = install(archive, record['sha256'], directory / 'sdk')
        if (result['seal_sha256'] != record['seal_sha256'] or
                result['cost']['source_build_id'] != record['source_build_id']):
            raise ValueError('installed preview differs from the release manifest')
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--directory', type=Path, required=True)
    options = parser.parse_args()
    print(json.dumps(install_preview(options.manifest, options.directory), indent=2))


if __name__ == '__main__':
    main()
