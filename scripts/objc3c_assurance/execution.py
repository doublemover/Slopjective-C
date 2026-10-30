"""Checked native commands and discovered CTest execution records."""
from __future__ import annotations

import json
from pathlib import Path
import subprocess
import time

from scripts.objc3c_toolchain.binary_packages import write_json
from .evidence import junit


def ctest_inventory(build: Path) -> set[str]:
    tests = json.loads(subprocess.check_output(
        ['ctest', '--test-dir', str(build), '--show-only=json-v1'], text=True))['tests']
    names = [test['name'] for test in tests]
    if not names or len(names) != len(set(names)):
        raise ValueError('CTest discovery is empty or has duplicate names')
    return set(names)


def checked_command(label, argv, *, repo: Path, environment: dict, directory: Path, records: list):
    if not label or '/' in label or '\\' in label or label in ('.', '..'):
        raise ValueError('command log must belong to its active native suite')
    argv = list(map(str, argv))
    start = time.monotonic()
    log = directory / (label + '.log')
    result_file = None
    if argv[0] == 'ctest':
        result_file = directory / (label + '.xml')
        argv += ['--output-junit', str(result_file)]
    print('native assurance ' + label + ': ' + str(log), flush=True)
    row = {'suite': directory.name, 'name': label, 'argv': argv, 'exit_code': None,
           'log': str(log)}
    records.append(row)
    try:
        with log.open('w') as stream:
            result = subprocess.run(argv, cwd=repo, env=environment, stdout=stream,
                                    stderr=subprocess.STDOUT, timeout=1800)
        row['exit_code'] = result.returncode
        if result.returncode:
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
