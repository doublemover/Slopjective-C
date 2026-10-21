"""Qualify frontend CTests with one linked test executable resident at a time."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import re
import subprocess

from .sdk_example import write_json


def qualify(build: Path, output: Path, run, *, jobs: int) -> dict:
    """Build, execute and reclaim each owned test; retain failed outputs."""
    build = build.resolve(strict=True)
    report = {'status': 'running', 'schema': 'objc3c.frontend-serial-ctest.v1',
              'tests': [], 'reclaimed': [], 'ctest_test_names': []}
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    write_json(receipt, report)
    try:
        plan = json.loads((build / 'frontend-test-targets.json').read_text())
        metadata = json.loads(subprocess.check_output(
            ['ctest', '--test-dir', str(build), '--show-only=json-v1'], text=True))
        names = [test['name'] for test in metadata['tests']]
        rows = plan.get('tests', [])
        if (not names or len(names) != len(set(names)) or
                len(rows) != len(names) or {row['name'] for row in rows} != set(names)):
            raise ValueError('frontend target manifest does not match the CTest inventory')
        for row in rows:
            name, target, executable = row['name'], row['target'], Path(row['executable'])
            if (not re.fullmatch(r'objc3_[a-z0-9_]+', name) or
                    target != name + ('_test' if name != 'objc3_text_file_cli' else '') or
                    executable.name != target or not executable.is_absolute() or
                    not executable.resolve().is_relative_to(build) or executable.is_symlink()):
                raise ValueError('frontend test target is not an owned build output')
        report['ctest_test_names'] = names
        write_json(receipt, report)
        for row in rows:
            name, target, executable = row['name'], row['target'], Path(row['executable'])
            run('frontend-' + name + '-build',
                ['cmake', '--build', build, '--target', target, '--parallel', str(jobs)])
            run('frontend-' + name + '-ctest',
                ['ctest', '--test-dir', build, '--no-tests=error', '--output-on-failure',
                 '-R', '^' + re.escape(name) + '$'])
            if executable.is_symlink() or not executable.is_file():
                raise ValueError('frontend test executable changed during qualification')
            with executable.open('rb') as stream:
                signature = hashlib.file_digest(stream, 'sha256').hexdigest()
            record = {**row, 'bytes': executable.stat().st_size, 'sha256': signature}
            report['tests'].append(record)
            # Reclaim only successfully executed, generated test programs.
            # Keep the installed compiler, scanner, libraries and object files.
            if target.endswith('_test'):
                executable.unlink()
                report['reclaimed'].append(record)
            write_json(receipt, report)
        report.update(status='PASS', frontend_ctests=len(report['tests']),
                      test_executables=len(report['reclaimed']),
                      bytes=sum(row['bytes'] for row in report['reclaimed']))
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(receipt, report)
    return report
