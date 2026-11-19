"""Execute and record a native clean-container Linux SDK installation."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import subprocess


DISTRIBUTIONS = ('debian:13', 'ubuntu:26.04')
SCHEMA = 'objc3c.linux-archive-install.v1'
IMAGE = re.compile(r'[^\s@]+@sha256:[a-f0-9]{64}')
IMAGE_ID = re.compile(r'sha256:[a-f0-9]{64}')


def _container(reference: str) -> tuple[str, str]:
    subprocess.run(['docker', 'pull', reference], check=True)
    info, = json.loads(subprocess.check_output(['docker', 'image', 'inspect', reference]))
    image = next((value for value in info['RepoDigests'] if IMAGE.fullmatch(value)), None)
    if image is None or not IMAGE_ID.fullmatch(info['Id']):
        raise ValueError(f'Docker did not resolve an immutable image for {reference}')
    return image, info['Id']


def qualify(download: Path, output: Path, distribution: str, *, host: str) -> None:
    archive, = download.rglob('*.tar.zst')
    record = json.loads(archive.with_suffix('.json').read_bytes())
    if record['host'] != host:
        raise ValueError('archive receipt differs from the native qualification host')
    image, image_id = _container(distribution)
    subprocess.run(['docker', 'run', '--rm', '--security-opt', 'seccomp=unconfined',
                    '--mount', f'type=bind,source={download.resolve()},target=/release,readonly',
                    image, 'sh', '/release/scripts/qualify_linux_archive.sh',
                    '/release/' + archive.relative_to(download).as_posix(),
                    record['archive_sha256'], '/opt/objc3c'], check=True)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({
        'schema': SCHEMA, 'status': 'PASS', 'host': host, 'distribution': distribution,
        'image': image, 'image_id': image_id, 'revision': os.environ['GITHUB_SHA'],
        'run': int(os.environ['GITHUB_RUN_ID']), 'attempt': int(os.environ['GITHUB_RUN_ATTEMPT']),
        **{key: record[key] for key in ('archive_sha256', 'seal_sha256', 'source_build_id')},
    }, indent=2) + '\n')


def rehearse(bundle: Path, host: str) -> None:
    manifest = json.loads((bundle / 'linux-release.json').read_bytes())
    record = manifest['archives'][host]
    for test in record['clean_installs']:
        image, image_id = _container(test['image'])
        if image != test['image'] or image_id != test['image_id']:
            raise ValueError('draft rehearsal image differs from clean-install qualification')
        subprocess.run(['docker', 'run', '--rm',
                        '--mount', f'type=bind,source={bundle.resolve()},target=/release,readonly',
                        '--workdir', '/release', image, 'sh', '-eu', '-c', '''
sha256sum --check --ignore-missing SHA256SUMS
archive=$1
checksum=$(awk -v archive="$archive" '$2 == archive { print $1 }' SHA256SUMS)
sh install-linux.sh --provision --archive "$archive" --sha256 "$checksum" --root /opt/objc3c
. /opt/objc3c/activate.sh
objc3c --version
objc3c "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/main.objc3" -O2 -o /tmp/application
/tmp/application
''', 'sh', record['file']], check=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', required=True)
    parser.add_argument('--directory', type=Path, required=True)
    parser.add_argument('--distribution', choices=DISTRIBUTIONS)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    if args.distribution and args.output:
        qualify(args.directory, args.output, args.distribution, host=args.host)
    elif not args.distribution and not args.output:
        rehearse(args.directory, args.host)
    else:
        parser.error('supply distribution and output together for qualification')


if __name__ == '__main__':
    main()
