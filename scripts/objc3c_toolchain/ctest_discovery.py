"""Discover CTest inventories with bounded, reproducible diagnostics."""
from __future__ import annotations

import json
from pathlib import Path
import re
import subprocess
import tempfile
import time


MAX_TRANSCRIPT_BYTES = 1024 * 1024


def discover(build: Path, receipt: dict, *, selected: tuple[str, ...] | None = None) -> dict[str, list[str]]:
    argv = ['ctest', '--test-dir', str(build), '--show-only=json-v1']
    if selected is not None:
        argv += ['-R', '^(' + '|'.join(map(re.escape, selected)) + ')$']
    receipt.update(argv=argv, status='FAIL', exit_code=None)
    started = time.monotonic()
    try:
        with tempfile.TemporaryFile() as stdout, tempfile.TemporaryFile() as stderr:
            try:
                result = subprocess.run(argv, stdout=stdout, stderr=stderr)
                receipt['exit_code'] = result.returncode
            finally:
                stdout.seek(0)
                stderr.seek(0)
                transcripts = {name: stream.read(MAX_TRANSCRIPT_BYTES + 1)
                               for name, stream in (('stdout', stdout), ('stderr', stderr))}
                receipt.update({name: data[:MAX_TRANSCRIPT_BYTES].decode(errors='replace')
                                for name, data in transcripts.items()})
        if any(len(data) > MAX_TRANSCRIPT_BYTES for data in transcripts.values()):
            raise ValueError('CTest discovery exceeded its transcript limit')
        if receipt['exit_code'] != 0:
            raise ValueError(f'CTest discovery exited {receipt["exit_code"]}')
        rows = json.loads(receipt['stdout'])['tests']
        names = [row['name'] for row in rows]
        if not names or any(not isinstance(name, str) or not name for name in names) or len(names) != len(set(names)):
            raise ValueError('CTest discovery is empty or has duplicate names')
        if selected is not None and set(names) != set(selected):
            raise ValueError('CTest discovery differs from the exact selected names')
    except (OSError, KeyError, TypeError, ValueError) as error:
        receipt['error'] = str(error)
        raise ValueError(f'CTest discovery failed: {error}') from error
    finally:
        receipt['elapsed_seconds'] = round(time.monotonic() - started, 6)
    receipt.update(status='PASS', names=names)
    return {row['name']: row.get('command', []) for row in rows}
