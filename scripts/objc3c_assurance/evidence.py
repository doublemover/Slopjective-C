"""Fail closed on missing, stale, substituted or incomplete native results."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET

from .plan import validate

SCHEMA = 'objc3c.native-assurance.result.v1'
MAX_BYTES = 256 * 1024 * 1024


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def artifact(root: Path, path: Path) -> dict:
    if path.is_symlink() or not path.is_file() or not path.resolve().is_relative_to(root.resolve()):
        raise ValueError('assurance artifact is not an owned regular file')
    return {'path': path.relative_to(root).as_posix(), 'bytes': path.stat().st_size, 'sha256': digest(path)}


def native_files(directory: Path) -> list[Path]:
    return sorted(path for path in directory.rglob('*') if path.is_file() and
                  path.suffix in ('.json', '.xml', '.log') and not any(
                      part in ('build', 'isolated-install') for part in path.relative_to(directory).parts))


def junit(path: Path, *, expected: set[str] | None = None) -> list[dict]:
    root = ET.parse(path).getroot()
    rows = []
    for suite in root.iter():
        if suite.tag in ('testsuite', 'testsuites'):
            for attribute in ('failures', 'errors', 'skipped', 'disabled'):
                if int(suite.get(attribute, '0')) != 0:
                    raise ValueError('native JUnit suite reports failures or unexpected skips')
            if 'tests' in suite.attrib and int(suite.attrib['tests']) != len(list(suite.iter('testcase'))):
                raise ValueError('native JUnit suite has incomplete execution inventory')
    for case in root.iter('testcase'):
        if (case.find('failure') is not None or case.find('error') is not None or
                case.find('skipped') is not None or case.get('status') == 'notrun' or
                case.get('result') in ('skipped', 'suppressed')):
            raise ValueError('native JUnit contains failure, error or unexpected skip')
        rows.append({'name': case.attrib['name'], 'class': case.get('classname', '')})
    names = [row['name'] for row in rows]
    if not rows or len({(row['class'], row['name']) for row in rows}) != len(rows):
        raise ValueError('native JUnit has empty or duplicate execution inventory')
    if expected is not None and (set(names) != expected or len(names) != len(expected)):
        raise ValueError('native JUnit differs from discovered execution inventory')
    return rows


def lit(path: Path) -> list[dict]:
    report = json.loads(path.read_text())
    rows = report.get('tests', [])
    if not rows or len({row['name'] for row in rows}) != len(rows):
        raise ValueError('lit execution inventory is empty or duplicated')
    if any(row.get('code') not in ('PASS', 'XFAIL', 'UNSUPPORTED') for row in rows):
        raise ValueError('lit execution contains failed, missing or unexpected outcomes')
    if not any(row['code'] == 'PASS' for row in rows):
        raise ValueError('lit execution has no passing supported tests')
    inventory = []
    for row in rows:
        entry = {'name': row['name'], 'outcome': row['code']}
        if row['code'] == 'UNSUPPORTED':
            if not row.get('output', '').strip():
                raise ValueError('unsupported lit result has no capability exclusion reason')
            entry['exclusion'] = {'reason': row['output'], 'owner': 'LLVM fixture capability directives',
                                  'issue': '#8283', 'review': 'Every fixture, host or toolchain input change'}
        inventory.append(entry)
    return inventory


def suite_results(root: Path, name: str) -> list[dict]:
    """Validate the canonical owners' contents, rather than trust an outer PASS."""
    directory = root / name

    def read(path, status='PASS'):
        result = json.loads((directory / path).read_text())
        if result.get('status') != status:
            raise ValueError('native owner did not pass: ' + name)
        return result

    if name == 'foundation':
        result = read('foundation.json', 'passed')
        phases = result.get('phases', [])
        if ({row['phase'] for row in phases} != {'configure', 'build', 'test', 'stage'} or
                any(row.get('exit_code') != 0 for row in phases)):
            raise ValueError('foundation configure/build/test/stage evidence is incomplete')
        rows = []
        for phase in phases:
            if phase['phase'] == 'test':
                path = directory / Path(phase['test_results']).name
                rows.extend(lit(path) if path.suffix == '.json' else junit(path))
        return rows
    if name == 'runtime':
        read('runtime.json', 'passed')
        names = json.loads((directory / 'runtime-ctest-inventory.json').read_text())
        if not names or len(names) != len(set(names)):
            raise ValueError('runtime CTest inventory is empty or duplicated')
        return junit(directory / 'runtime-ctest.xml', expected=set(names))
    if name == 'frontend':
        result = read('frontend-tests/qualification.json')
        names = result.get('ctest_test_names', [])
        tests = result.get('tests', [])
        if (not names or len(names) != len(set(names)) or
                len(tests) != len(names) or {row['name'] for row in tests} != set(names) or
                result.get('frontend_ctests') != len(names)):
            raise ValueError('frontend manifest and executed tests are incomplete')
        for row in tests:
            junit(directory / ('frontend-' + row['name'] + '-ctest.xml'), expected={row['name']})
        return tests
    if name == 'installed':
        result = read('installed-tests/summary.json')
        commands = result.get('commands', [])
        if not commands or any(row.get('exit_code') != 0 for row in commands):
            raise ValueError('installed native execution is incomplete')
        return commands
    if name == 'sanitizers':
        result = read('structured/qualification.json')
        rows = result.get('executions', [])
        if (len(rows) != 4 or {(row['sanitizer'], row['optimization']) for row in rows} !=
                {(mode, opt) for mode in ('address,undefined', 'thread') for opt in ('O0', 'O2')} or
                len(result.get('sanitizer_policy', {})) != 4 or len(result.get('runtime_profiles', [])) != 4):
            raise ValueError('sanitizer execution/control/profile matrix is incomplete')
        return rows
    if name == 'upstream':
        return lit(directory / 'lit.json')
    if name == 'package':
        result = read('release.json')
        return [{'archive_sha256': result['archive_sha256'], 'status': 'PASS'}]
    raise ValueError('unrecognized native assurance suite')


def aggregate(plan: dict, root: Path, *, outcomes: dict, tested: str) -> dict:
    validate(plan)
    if plan['revision']['tested'] != tested or outcomes.get('plan') != 'success':
        raise ValueError('selection failed or selected the wrong tested revision')
    expected = {suite['id'] for suite in plan['suites']}
    if not expected:
        if plan['classification'] != 'prose-only' or outcomes.get('native') != 'skipped':
            raise ValueError('only justified prose-only changes may skip native execution')
        return {'status': 'PASS', 'classification': 'prose-only', 'revision': plan['revision'], 'suites': []}
    if outcomes.get('native') != 'success':
        raise ValueError('native prerequisite failed, was cancelled, skipped or did not finish')
    receipt = json.loads((root / 'result.json').read_text())
    if (receipt.get('schema') != SCHEMA or receipt.get('status') != 'PASS' or
            receipt.get('revision') != plan['revision'] or receipt.get('plan_sha256') != plan['sha256']):
        raise ValueError('native result is incomplete, stale, failed or belongs to another revision')
    records = receipt.get('suites', [])
    if len(records) != len(expected) or {row['id'] for row in records} != expected:
        raise ValueError('native result differs from the exact required suite set')
    total = 0
    paths = set()
    for row in records:
        if row.get('status') != 'PASS' or not row.get('executions') or not row.get('artifacts'):
            raise ValueError('native suite lacks passing executions or mandatory evidence')
        if {value['path'] for value in row['artifacts']} != {
                path.relative_to(root).as_posix() for path in native_files(root / row['id'])}:
            raise ValueError('native artifacts do not cover the complete durable suite evidence')
        for saved in row['artifacts']:
            path = root / saved['path']
            if saved['path'] in paths or artifact(root, path) != saved:
                raise ValueError('native artifact is missing, duplicated or changed')
            paths.add(saved['path'])
            total += saved['bytes']
        if suite_results(root, row['id']) != row['executions']:
            raise ValueError('native execution inventory differs from canonical owner results')
    commands = receipt.get('commands', [])
    if any(row.get('exit_code') != 0 for row in commands):
        raise ValueError('native command evidence contains a failed execution')
    if total > MAX_BYTES:
        raise ValueError('native evidence exceeds the 256 MiB artifact budget')
    return {'status': 'PASS', 'classification': 'native', 'revision': plan['revision'],
            'suites': sorted(expected), 'artifact_bytes': total}
