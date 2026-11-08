"""Provision the pinned GNU runtime used by frontend test executables."""
from __future__ import annotations

import json
import os
from pathlib import Path
import subprocess
import sys

from .binary_packages import digest


def prepare(repo: Path, bootstrap: Path, output: Path, *, fetch: bool, jobs: int) -> dict:
    output.mkdir(parents=True, exist_ok=False)
    receipt = output / 'runtime.json'
    command = [sys.executable, '-B', '-m', 'scripts.objc3c_toolchain_runtime.build_linux',
        '--repo', str(repo), '--compiler-root', str(bootstrap), '--jobs', str(jobs),
        '--report-file', str(receipt), *(['--fetch'] if fetch else [])]
    environment = {**os.environ, 'LD_LIBRARY_PATH': str(bootstrap / 'lib'),
                   'LLVM_PROFILE_FILE': os.devnull}
    log = output / 'build.log'
    with log.open('w') as stream:
        result = subprocess.run(command, cwd=repo, env=environment,
            stdout=stream, stderr=subprocess.STDOUT, timeout=900)
    if result.returncode or not receipt.is_file():
        raise ValueError('pinned frontend test runtime build failed; see ' + str(log))
    record = json.loads(receipt.read_text())
    profile = json.loads((repo / 'scripts/objc3c_toolchain_runtime/runtime-profile.json').read_text())['profiles']['linux-x86_64']
    root = Path(record['stage']).resolve(strict=True)
    library = (root / 'lib/libobjc.so').resolve(strict=True)
    if (record.get('status') != 'passed' or record.get('source_commit') != profile['runtime_commit'] or
            record.get('robin_map_commit') != profile['robin_map_commit'] or
            record['configuration'].get('compiler_sha256') != digest(bootstrap / 'bin/clang') or
            not root.is_relative_to(repo / 'out/toolchains/libobjc2-v2.3') or
            not library.is_relative_to(root) or not library.is_file()):
        raise ValueError('frontend test runtime differs from its pinned compiler/source inputs')
    return {'root': str(root), 'library': str(library), 'library_sha256': digest(library),
            'receipt': str(receipt), 'receipt_sha256': digest(receipt),
            'command': command, 'log': str(log), 'log_sha256': digest(log)}
