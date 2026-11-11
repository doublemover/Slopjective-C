"""Split a CI artifact payload into bounded, independently verified downloads."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import tarfile
import tempfile

PARTS = 4
MAX_PART_BYTES = 480 * 1024 * 1024


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def split(source: Path, output: Path, source_run: str, source_artifact: str) -> dict:
    source = source.resolve(strict=True)
    output = output.resolve()
    if not source_run.isdecimal() or not source_artifact or not source.is_dir():
        raise ValueError('transport requires one exact source run, artifact and directory')
    if output.is_relative_to(source) or source.is_relative_to(output):
        raise ValueError('transport output must be separate from its source')
    if output.exists():
        raise ValueError('transport output already exists; preserving it')
    files = sorted(source.rglob('*'))
    if not files or any(p.is_symlink() or not (p.is_dir() or p.is_file()) for p in files):
        raise ValueError('artifact transport accepts only regular downloaded files and directories')
    if sum(p.stat().st_size for p in files if p.is_file()) > PARTS * MAX_PART_BYTES:
        raise ValueError('artifact exceeds bounded transport size')
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='artifact-transport-', dir=output.parent) as scratch:
        archive = Path(scratch) / 'payload.tar'
        with tarfile.open(archive, 'w') as tar:
            for path in files:
                tar.add(path, arcname=path.relative_to(source), recursive=False)
        total = archive.stat().st_size
        part_size = (total + PARTS - 1) // PARTS
        if part_size > MAX_PART_BYTES:
            raise ValueError('artifact container exceeds bounded transport size')
        staged = Path(scratch) / 'parts';staged.mkdir()
        manifest = {'schema': 'objc3c.artifact-transport.v1', 'source_run': source_run,
                    'source_artifact': source_artifact, 'payload_bytes': total,
                    'payload_sha256': digest(archive), 'parts': []}
        with archive.open('rb') as stream:
            for index in range(PARTS):
                part = staged / f'payload.part{index}'
                with part.open('wb') as target:
                    remaining = part_size
                    while remaining:
                        data = stream.read(min(remaining, 1024 * 1024))
                        if not data: break
                        target.write(data);remaining -= len(data)
                manifest['parts'].append({'name': part.name, 'bytes': part.stat().st_size,
                                          'sha256': digest(part)})
        (staged / 'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
        staged.rename(output)
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--source-run', required=True)
    parser.add_argument('--source-artifact', required=True)
    print(json.dumps(split(**vars(parser.parse_args())), indent=2))


if __name__ == '__main__':
    main()
