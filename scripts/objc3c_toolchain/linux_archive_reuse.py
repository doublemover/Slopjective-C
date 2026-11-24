"""Measure exact release archive readmission in the producer's Linux checkout."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import time

from .install_sdk import file_hash
from .linux_host import native_linux_host
from .linux_sdk import ReleaseInputChanged, build_sdk
from .sdk_example import write_json
from .semantic_build_costs import bounded_installation_receipt


SCHEMA = 'objc3c.linux-archive-reuse.v1'
PACKAGE_KEYS = ('archive', 'archive_sha256', 'archive_bytes', 'seal_sha256', 'source_build_id', 'host', 'target')


def fingerprint(path: Path) -> dict:
    if path.is_symlink() or not path.is_file():
        raise ValueError(f'archive replay requires an owned regular file: {path}')
    return {'bytes': path.stat().st_size, 'sha256': file_hash(path)}


def qualify(repo: Path, producer_receipt: Path) -> dict:
    repo = repo.resolve(strict=True)
    root = repo / 'out/toolchains'
    producer = json.loads(producer_receipt.read_bytes())
    if (not isinstance(producer, dict) or producer.get('status') != 'PASS' or
            producer.get('cache_hit') is not False or not isinstance(producer.get('input_identity'), str) or
            not re.fullmatch(r'[0-9a-f]{64}', producer['input_identity'])):
        raise ValueError('archive replay requires a fresh passing producer receipt for an exact input identity')
    identity = producer['input_identity']
    build = root / 'linux-sdk-builds' / identity
    release = build / 'release.json'
    published = json.loads(release.read_bytes())
    if {**published, 'cache_hit': False} != producer:
        raise ValueError('producer receipt differs from the canonical published release')
    archive = Path(published['archive'])
    if (archive.parent != root / 'sdk-cache' or archive.parent.resolve() != archive.parent or
            archive.suffixes[-2:] != ['.tar', '.zst'] or producer.get('host') != native_linux_host()):
        raise ValueError('producer archive is outside the native release cache')
    sidecar = archive.with_suffix('.json')
    packaged = json.loads(sidecar.read_bytes())
    if any(packaged[key] != published[key] for key in PACKAGE_KEYS):
        raise ValueError('archive package receipt differs from the published release')

    paths = {'archive': archive, 'package_receipt': sidecar, 'release_receipt': release,
             'producer_receipt': producer_receipt}
    before = {name: fingerprint(path) for name, path in paths.items()}
    if before['archive'] != {'sha256': published['archive_sha256'], 'bytes': published['archive_bytes']}:
        raise ValueError('archive bytes differ from the published release')
    active = root / 'linux-sdk/current'
    prefix = active.resolve(strict=True)
    if str(prefix) != producer['installed']['prefix'] or (build / 'prefix').exists():
        raise ValueError('archive replay requires the unchanged installation and reclaimed candidate')

    started = time.monotonic()
    replay = build_sdk(repo, reuse_input_identity=identity)
    replay_elapsed = time.monotonic() - started
    replay_cost = replay['release_reuse']
    preserved = ('input_identity', *PACKAGE_KEYS)
    if (not replay['cache_hit'] or any(replay[key] != published[key] for key in preserved) or
            replay_cost['release_receipt_sha256'] != before['release_receipt']['sha256'] or
            replay['installed']['prefix'] != str(prefix) or replay['installed']['cost']['prefix_reused'] is not True):
        raise ValueError('warm replay missed the exact release or existing sealed prefix')
    installation = bounded_installation_receipt(replay)

    stale = hashlib.sha256(('stale release ' + identity).encode()).hexdigest()
    started = time.monotonic()
    try:
        build_sdk(repo, reuse_input_identity=stale)
    except ReleaseInputChanged as error:
        rejection = {'status': 'REJECTED', 'requested_input_identity': stale,
                     'reason': str(error), 'elapsed_seconds': time.monotonic() - started}
    else:
        raise ValueError('a stale release input identity reached archive installation')

    after = {name: fingerprint(path) for name, path in paths.items()}
    if before != after or active.resolve(strict=True) != prefix or (build / 'prefix').exists():
        raise ValueError('warm replay changed archive, receipt or candidate state')
    return {'schema': SCHEMA, 'status': 'PASS', 'host': producer['host'],
            'revision': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=repo, text=True).strip(),
            'input_identity': identity, 'source_build_id': published['source_build_id'],
            'archive_name': archive.name, 'seal_sha256': published['seal_sha256'],
            'before': before, 'after': after, 'replay_elapsed_seconds': replay_elapsed,
            'phases_seconds': replay_cost['phases_seconds'], 'installation': installation,
            'stale_request': rejection}


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--producer-receipt', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    try:
        result = qualify(args.repo, args.producer_receipt)
    except Exception as error:
        write_json(args.output, {'schema': SCHEMA, 'status': 'FAIL', 'error': str(error)})
        raise
    write_json(args.output, result)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
