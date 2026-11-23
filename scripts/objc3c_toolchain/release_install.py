"""Execute and record a native clean-container Linux SDK installation."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time


DISTRIBUTIONS = ('debian:13', 'ubuntu:26.04')
SCOPES = ('smoke', 'package-build', 'package-surface', 'package-modules',
          'async', 'isolation', 'features', 'carriers')
SCHEMA = 'objc3c.linux-archive-install.v1'
SCOPE_SCHEMA = 'objc3c.linux-archive-install-scope.v1'
IMAGE = re.compile(r'[^\s@]+@sha256:[a-f0-9]{64}')
IMAGE_ID = re.compile(r'sha256:[a-f0-9]{64}')


def _container(reference: str) -> tuple[str, str]:
    subprocess.run(['docker', 'pull', reference], check=True)
    info, = json.loads(subprocess.check_output(['docker', 'image', 'inspect', reference]))
    image = next((value for value in info['RepoDigests'] if IMAGE.fullmatch(value)), None)
    if image is None or not IMAGE_ID.fullmatch(info['Id']):
        raise ValueError(f'Docker did not resolve an immutable image for {reference}')
    return image, info['Id']


def _context(host: str, distribution: str) -> dict:
    return {'host': host, 'distribution': distribution, 'revision': os.environ['GITHUB_SHA'],
            'run': int(os.environ['GITHUB_RUN_ID']), 'attempt': int(os.environ['GITHUB_RUN_ATTEMPT'])}


def _save(output: Path, report: dict) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + '\n')


def qualify(download: Path, output: Path, distribution: str, *, host: str, scope='full') -> None:
    if scope not in ('full', *SCOPES):
        raise ValueError('unknown clean-install scope')
    report = {'schema': SCOPE_SCHEMA if scope != 'full' else SCHEMA,
              **_context(host, distribution), 'scope': scope}
    output.parent.mkdir(parents=True, exist_ok=True)
    log = output.with_name('install.log')
    started = time.monotonic()
    try:
        archive, = download.rglob('*.tar.zst')
        record = json.loads(archive.with_suffix('.json').read_bytes())
        if record['host'] != host:
            raise ValueError('archive receipt differs from the native qualification host')
        report.update({key: record[key] for key in ('archive_sha256', 'seal_sha256', 'source_build_id')})
        image, image_id = _container(distribution)
        report.update(image=image, image_id=image_id)
        command = ['docker', 'run', '--rm', '--security-opt', 'seccomp=unconfined',
                   '--mount', f'type=bind,source={download.resolve()},target=/release,readonly',
                   image, 'sh', '/release/scripts/qualify_linux_archive.sh',
                   '/release/' + archive.relative_to(download).as_posix(),
                   record['archive_sha256'], '/opt/objc3c', scope]
        with log.open('wb') as transcript, subprocess.Popen(
                command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as process:
            for line in process.stdout:
                transcript.write(line)
                transcript.flush()
                sys.stdout.buffer.write(line)
                sys.stdout.buffer.flush()
            if process.wait():
                raise subprocess.CalledProcessError(process.returncode, command)
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        raise
    finally:
        report['elapsed_seconds'] = round(time.monotonic() - started, 3)
        if log.is_file():
            report['log_sha256'] = hashlib.sha256(log.read_bytes()).hexdigest()
        _save(output, report)


def aggregate(directory: Path, output: Path, distribution: str, *, host: str) -> None:
    context = _context(host, distribution)
    rows = {}
    for path in directory.glob('*/result.json'):
        data = path.read_bytes()
        row = json.loads(data)
        if row.get('distribution') != distribution:
            continue
        scope = row.get('scope')
        if (scope not in SCOPES or scope in rows or row.get('schema') != SCOPE_SCHEMA or
                row.get('status') != 'PASS' or any(row.get(key) != value for key, value in context.items()) or
                not IMAGE.fullmatch(row.get('image', '')) or not IMAGE_ID.fullmatch(row.get('image_id', '')) or
                not all(re.fullmatch('[a-f0-9]{64}', row.get(key, ''))
                        for key in ('archive_sha256', 'seal_sha256')) or not row.get('source_build_id') or
                hashlib.sha256(path.with_name('install.log').read_bytes()).hexdigest() != row.get('log_sha256')):
            raise ValueError(f'invalid or duplicate {distribution} clean-install scope: {scope}')
        rows[scope] = (row, hashlib.sha256(data).hexdigest())
    if set(rows) != set(SCOPES):
        raise ValueError(f'{distribution} clean-install scope inventory is incomplete: {sorted(set(SCOPES)-rows.keys())}')
    first = rows[SCOPES[0]][0]
    identity = {key: first[key] for key in ('image', 'image_id', 'archive_sha256',
                                           'seal_sha256', 'source_build_id')}
    if any(any(row[key] != value for key, value in identity.items()) for row, _ in rows.values()):
        raise ValueError(f'{distribution} clean-install scopes used different images or SDK archives')
    _save(output, {'schema': SCHEMA, 'status': 'PASS', **context, **identity,
                   'scopes': {scope: rows[scope][1] for scope in SCOPES}})


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
sh "$OBJC3C_SDK_ROOT/share/objc3c/examples/log-summary/verify.sh"
''', 'sh', record['file']], check=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', required=True)
    parser.add_argument('--directory', type=Path, required=True)
    parser.add_argument('--distribution', choices=DISTRIBUTIONS)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--scope', choices=SCOPES)
    parser.add_argument('--aggregate', action='store_true')
    args = parser.parse_args()
    if args.distribution and args.output:
        if args.aggregate and args.scope:
            parser.error('supply either a single scope or the complete scope aggregate')
        if args.aggregate:
            aggregate(args.directory, args.output, args.distribution, host=args.host)
        else:
            qualify(args.directory, args.output, args.distribution, host=args.host, scope=args.scope or 'full')
    elif not args.distribution and not args.output and not args.scope and not args.aggregate:
        rehearse(args.directory, args.host)
    else:
        parser.error('supply distribution and output together for qualification')


if __name__ == '__main__':
    main()
