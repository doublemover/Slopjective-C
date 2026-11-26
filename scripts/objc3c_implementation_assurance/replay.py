"""Replay the terminal gate for reviewed Actions archives without rebuilding LLVM."""
from __future__ import annotations

import argparse
import json
from pathlib import Path, PurePosixPath
import stat
import tempfile
from zipfile import ZipFile

from scripts.objc3c_toolchain.binary_packages import digest
from scripts.objc3c_toolchain.linux_host import HOSTS

from .evidence import MAX_BYTES, aggregate


def reviewed_zip(directory: Path, artifact_id: int, expected_sha256: str) -> ZipFile:
    path = directory / f'{artifact_id}.zip'
    if (path.is_symlink() or not path.is_file() or path.stat().st_size > MAX_BYTES or
            digest(path) != expected_sha256):
        raise ValueError(f'implementation artifact {artifact_id} differs from the reviewed archive')
    archive = ZipFile(path)
    members = archive.infolist()
    if (len({member.filename for member in members}) != len(members) or
            sum(member.file_size for member in members) > MAX_BYTES or
            any(PurePosixPath(member.filename).is_absolute() or '..' in PurePosixPath(member.filename).parts or
                '\\' in member.filename or stat.S_IFMT(member.external_attr >> 16) not in (0, stat.S_IFREG, stat.S_IFDIR)
                for member in members)):
        archive.close()
        raise ValueError(f'implementation artifact {artifact_id} has unowned or excessive contents')
    return archive


def replay(repo: Path, directory: Path, *, host: str) -> dict:
    index = json.loads((repo / 'docs/support/linux_analysis_receipts.json').read_text())
    if index.get('schema') != 'objc3c.reviewed-implementation-receipts.v1' or host not in index['hosts']:
        raise ValueError('implementation replay requires a reviewed artifact inventory for its host')
    reviewed = index['hosts'][host]
    with reviewed_zip(directory, reviewed['plan_artifact_id'], reviewed['plan_archive_sha256']) as archive:
        if archive.namelist() != ['plan.json']:
            raise ValueError('implementation plan artifact must contain one exact plan')
        plan = json.loads(archive.read('plan.json'))
    partitions = reviewed['partitions']
    if [row['partition'] for row in partitions] != list(range(plan['analysis_partitions'])):
        raise ValueError('implementation review omitted an expected partition')
    with tempfile.TemporaryDirectory(prefix='objc3-analysis-replay-') as scratch:
        root = Path(scratch)
        for row in partitions:
            with reviewed_zip(directory, row['artifact_id'], row['archive_sha256']) as archive:
                archive.extractall(root / str(row['partition']))
        result = aggregate(plan, root, outcomes={'plan': 'success', 'analysis': 'success',
            'foundation': 'success', 'foundation_archive': reviewed['shared_foundation_sha256']}, tested=index['revision'])
    if any(result[name] != reviewed[name] for name in ('analyzed_variants', 'inventory_sha256')):
        raise ValueError('implementation replay differs from the reviewed result')
    return {**result, 'host': host, 'run_id': reviewed['run_id'], 'plan_artifact_id': reviewed['plan_artifact_id']}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--archive-directory', type=Path, required=True)
    parser.add_argument('--host', choices=HOSTS, required=True)
    args = parser.parse_args()
    print(json.dumps(replay(args.repo, args.archive_directory, host=args.host), indent=2))


if __name__ == '__main__':
    main()
