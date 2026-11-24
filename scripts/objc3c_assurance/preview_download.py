"""Install a published preview pinned by an already reviewed release manifest."""
from __future__ import annotations

import argparse
import fcntl
import json
import os
from pathlib import Path
import subprocess
import tempfile
import time

from scripts.objc3c_toolchain.install_sdk import install
from scripts.objc3c_toolchain.linux_host import native_linux_host
from scripts.objc3c_toolchain.release_install import published_archive
from .evidence import digest

DOWNLOADS = 'https://github.com/doublemover/Slopjective-C/releases/download'
WITNESS_OUTPUT = b'native Linux application\n'


def download_archive(path: Path, tag: str, record: dict) -> None:
    subprocess.run(['curl', '--fail', '--silent', '--show-error', '--location', '--retry', '3',
                    '--max-filesize', str(record['bytes']), '--output', str(path),
                    f'{DOWNLOADS}/{tag}/{record["file"]}'], check=True)
    if path.stat().st_size != record['bytes']:
        raise ValueError('downloaded preview archive length differs from the release manifest')


def acquire_archive(scratch: Path, tag: str, record: dict, cache: Path | None) -> tuple[Path, str]:
    if cache is None:
        archive = scratch / record['file']
        download_archive(archive, tag, record)
        return archive, 'download'

    cache.mkdir(parents=True, exist_ok=True)
    checksum = record['sha256']
    archive = cache / (checksum + '.tar.zst')
    with (cache / (checksum + '.lock')).open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        if archive.is_symlink() or (archive.exists() and not archive.is_file()):
            raise ValueError('preview archive cache entry must be a regular file')
        if archive.is_file() and archive.stat().st_size == record['bytes'] and digest(archive) == checksum:
            return archive, 'cache'
        with tempfile.TemporaryDirectory(prefix='.preview-download-', dir=cache) as pending:
            downloaded = Path(pending) / record['file']
            download_archive(downloaded, tag, record)
            if digest(downloaded) != checksum:
                raise ValueError('downloaded preview archive SHA-256 differs from the release manifest')
            os.replace(downloaded, archive)
    return archive, 'download'


def install_preview(manifest: Path, directory: Path, *, witness_log: Path | None = None,
                    archive_cache: Path | None = None) -> dict:
    index = json.loads(manifest.read_bytes())
    tag, host = index['tag'], native_linux_host()
    record = published_archive(index, host)
    identity = {'expected_seal_sha256': record['seal_sha256'],
                'expected_source_build_id': record['source_build_id']}
    directory.mkdir(parents=True, exist_ok=True)
    destination = directory / 'sdk'
    initial = directory / 'initial SDK' if witness_log else destination
    if witness_log and any(path.exists() or path.is_symlink() for path in (initial, destination)):
        raise ValueError('preview relocation witness requires a fresh installation directory')
    with tempfile.TemporaryDirectory(prefix='preview-download-', dir=directory) as scratch:
        started = time.monotonic()
        archive, acquisition = acquire_archive(Path(scratch), tag, record, archive_cache)
        archive_acquisition = {'source': acquisition, 'sha256': record['sha256'],
                               'bytes': record['bytes'], 'elapsed_seconds': time.monotonic() - started}
        result = install(archive, record['sha256'], initial, **identity)
        if (result['seal_sha256'] != record['seal_sha256'] or
                result['cost']['source_build_id'] != record['source_build_id']):
            raise ValueError('installed preview differs from the release manifest')
        if witness_log:
            initial.rename(destination)
            relocated = install(archive, record['sha256'], destination, **identity)
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
                for command in ([root / 'bin/objc3c', source, '-O2', '-g', '-Werror', '-o', application], [application]):
                    subprocess.run(command, check=True, env=environment, stdout=log,
                                   stderr=subprocess.STDOUT, timeout=120)
            if witness_log.read_bytes() != WITNESS_OUTPUT:
                raise ValueError('relocated preview application produced unexpected output')
            result = {**relocated, 'witness': {'status': 'PASS', 'source': 'native/main.objc3',
                      'log_sha256': digest(witness_log), 'initial_install_cost': result['cost']}}
    return {**result, 'archive_acquisition': archive_acquisition}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--directory', type=Path, required=True)
    parser.add_argument('--witness-log', type=Path)
    parser.add_argument('--archive-cache', type=Path, help='reuse verified compressed archives by SHA-256')
    options = parser.parse_args()
    print(json.dumps(install_preview(options.manifest, options.directory, witness_log=options.witness_log,
                                     archive_cache=options.archive_cache), indent=2))


if __name__ == '__main__':
    main()
