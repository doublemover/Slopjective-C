"""Checked native commands and discovered CTest execution records."""
from __future__ import annotations

import json
import math
from pathlib import Path
import subprocess
import time

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.llvm_ir_execution import command as bounded_command
from .evidence import junit


def ctest_inventory(build: Path) -> set[str]:
    tests = json.loads(subprocess.check_output(
        ['ctest', '--test-dir', str(build), '--show-only=json-v1'], text=True))['tests']
    names = [test['name'] for test in tests]
    if not names or len(names) != len(set(names)):
        raise ValueError('CTest discovery is empty or has duplicate names')
    return set(names)


def checked_command(label, argv, *, repo: Path, environment: dict, directory: Path, records: list,
                    timeout: float = 1800):
    if not label or '/' in label or '\\' in label or label in ('.', '..'):
        raise ValueError('command log must belong to its active native suite')
    argv = list(map(str, argv))
    if (not argv or type(timeout) not in (int, float) or not math.isfinite(timeout) or timeout <= 0):
        raise ValueError('native command requires an executable and a finite positive timeout')
    start = time.monotonic()
    log = directory / (label + '.log')
    result_file = None
    if argv[0] == 'ctest':
        result_file = directory / (label + '.xml')
        argv += ['--output-junit', str(result_file)]
    if (log.exists() or log.is_symlink() or
            result_file is not None and (result_file.exists() or result_file.is_symlink())):
        raise ValueError('native command requires fresh log and CTest evidence: ' + label)
    print('native assurance ' + label + ': ' + str(log), flush=True)
    row = {'suite': directory.name, 'name': label, 'argv': argv, 'exit_code': None,
           'log': str(log)}
    records.append(row)
    try:
        row.update(bounded_command(argv, cwd=repo, environment=environment, log=log, timeout=timeout))
        if row['exit_code'] or row['timed_out']:
            raise ValueError('native command failed: ' + label + '; inspect ' + str(log))
        if result_file is not None:
            junit(result_file)
    except BaseException as error:
        row['error'] = str(error)
        raise
    finally:
        row['elapsed_seconds'] = round(time.monotonic()-start, 6)


def execute_ctest(build: Path, label: str, directory: Path, command) -> list[dict]:
    names = ctest_inventory(build)
    write_json(directory / (label + '-inventory.json'), sorted(names))
    command(label, ['ctest', '--test-dir', build, '--no-tests=error',
                    '--output-on-failure', '--timeout', '120'])
    return junit(directory / (label + '.xml'), expected=names)
