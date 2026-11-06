"""Fail closed on missing, stale, substituted or incomplete native results."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import re
import os
import shutil
import xml.etree.ElementTree as ET

from .plan import execution_shards, foundation_receipts, selected_suites, validate

SCHEMA = 'objc3c.native-assurance.result.v1'
MAX_BYTES = 256 * 1024 * 1024


def package_execution(result: dict) -> list[dict]:
    """One producer/verifier contract for the SDK pipeline's installed archive.

    build_sdk already seals and installs the exact archive and qualifies its
    installed consumers. The terminal gate verifies that retained owner result;
    it does not need the SDK tree or another archive qualification.
    """
    qualification = result.get('installed_qualification')
    installed = result.get('installed')
    commands = qualification.get('commands') if isinstance(qualification, dict) else None
    signature = result.get('archive_sha256')
    prefix = installed.get('prefix') if isinstance(installed, dict) else None
    if (result.get('status') != 'PASS' or not isinstance(signature, str) or
            not re.fullmatch(r'[0-9a-f]{64}', signature) or not isinstance(prefix, str) or
            not Path(prefix).is_absolute() or not isinstance(commands, list) or not commands or
            qualification.get('status') != 'PASS' or
            any(not isinstance(row, dict) or type(row.get('exit_code')) is not int or
                row['exit_code'] != 0 or row.get('timed_out') for row in commands)):
        raise ValueError('SDK pipeline omitted its passing installed archive qualification')
    return [{'status': 'PASS', 'archive_sha256': signature,
             'installed_prefix': prefix, 'installed_consumer_commands': len(commands)}]


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def artifact(root: Path, path: Path) -> dict:
    if path.is_symlink() or not path.is_file() or not path.resolve().is_relative_to(root.resolve()):
        raise ValueError('assurance artifact is not an owned regular file')
    return {'path': path.relative_to(root).as_posix(), 'bytes': path.stat().st_size, 'sha256': digest(path)}


def native_files(directory: Path) -> list[Path]:
    return sorted(path for path in directory.rglob('*') if path.is_file() and
                  (path.suffix in ('.json', '.xml', '.log') or
                   ((path.parent.name == 'blobs' and path.parent.parent.name == 'product-evidence') or
                    (path.parent.name == 'product-evidence-blobs' and
                     path.parent.parent.is_relative_to(directory))) and
                    len(path.stem) == 64 and path.suffix == '.zst' and
                    all(char in '0123456789abcdef' for char in path.stem)) and not any(
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
        from scripts.objc3c_toolchain.qualify_executor_locks import CASES
        locks = read('executor-locks/qualification.json')
        expected = {'correct-contract', 'current-executor', 'implementation-missing-lock',
                    *(marker for marker, _ in CASES.values())}
        if (len(locks.get('cases', [])) != len(expected) or
                {row['name'] for row in locks['cases'] if row.get('status') == 'PASS'} != expected):
            raise ValueError('executor lock contract controls are incomplete')
        result = read('structured/qualification.json')
        rows = result.get('executions', [])
        if (len(rows) != 4 or {(row['sanitizer'], row['optimization']) for row in rows} !=
                {(mode, opt) for mode in ('address,undefined', 'thread') for opt in ('O0', 'O2')} or
                len(result.get('sanitizer_policy', {})) != 4 or len(result.get('runtime_profiles', [])) != 4):
            raise ValueError('sanitizer execution/control/profile matrix is incomplete')
        return rows
    if name == 'upstream':
        return lit(directory / 'lit.json')
    if name == 'swift-consumer':
        from scripts.objc3c_toolchain.swift_consumer import REQUIRED_ROWS, execution_result
        result = read('consumer/qualification.json')
        cases, controls, rows = (result.get(key, []) for key in ('cases', 'controls', 'rows'))
        expected = {(transport, native, swift) for transport in ('original', 'emitted')
                    for native in ('O0', 'O2') for swift in ('Onone', 'O')}
        control_names = {'original-header-independent-import', 'unmodified-emitted-header-import',
            'old-spelling', 'wrong-type', 'nullability', 'availability',
            'public-name-identity-and-consumer', 'missing-provider-symbol'}
        if (len(cases) != 8 or {(row['transport'], row['native_optimization'], row['swift_optimization'])
                              for row in cases if row.get('status') == 'PASS'} != expected or
                len(controls) != 8 or {row['name'] for row in controls if row.get('status') == 'PASS'} != control_names or
                len(rows) != len(REQUIRED_ROWS) or {row['id'] for row in rows} != set(REQUIRED_ROWS) or
                any(row.get('status') != ('swift-import-qualified' if row['id'] == 'availability'
                                         else 'swift-call-qualified') for row in rows) or
                not result.get('commands') or result.get('relocation') != {
                    'original_header_hidden': True, 'provider_source_hidden': True, 'replayed_header_unchanged': True}):
            raise ValueError('independent Swift import/execution/control inventory is incomplete')
        for row in cases:
            execution_result(json.dumps(row.get('result')))
        return cases
    if name == 'semantic-workloads':
        from scripts.objc3c_toolchain.semantic_benchmarks import semantic_result, summarize, workload_inventory, workload_names
        from scripts.objc3c_toolchain.semantic_runtime_oracles import (CURRENT_WORKLOAD_VERSION,
            completed_task_count, validate_delay_controls, validate_task_frame_controls)
        result = read('measurement/qualification.json')
        expected = workload_names(CURRENT_WORKLOAD_VERSION)
        summaries, samples = result.get('summaries', []), result.get('samples', [])
        if (result.get('correctness') != 'PASS' or result.get('workload_version') != CURRENT_WORKLOAD_VERSION or
                result.get('workloads') != workload_inventory(CURRENT_WORKLOAD_VERSION) or
                result.get('repetitions') != 3 or result.get('timing_decision') != 'advisory-inconclusive-uncalibrated' or
                len(result.get('sdks', [])) != 1 or len(summaries) != len(expected) or
                {row['workload'] for row in summaries} != expected or
                any(row['wall']['samples'] != 3 or row['sdk_index'] != 0 for row in summaries) or
                result.get('delay_control', {}).get('status') != 'PASS' or
                not result['delay_control'].get('semantic_output_unchanged') or
                any(row.get('exit_code') != 0 or row.get('timed_out') for row in samples)):
            raise ValueError('semantic workload correctness/sample/control inventory is incomplete')
        validate_delay_controls(result)
        validate_task_frame_controls(result)
        for workload in expected:
            rows = [row for row in samples if row.get('workload') == workload]
            if len(rows) != 4 or len([row for row in rows if row.get('warmup') is False]) != 3:
                raise ValueError('semantic workload omitted its actual warmup/repetition set')
            measured = [row for row in rows if row.get('warmup') is False]
            summary = next(row for row in summaries if row['workload'] == workload)
            if summary['wall'] != summarize(measured):
                raise ValueError('semantic workload summary differs from its actual samples')
            if workload.startswith('runtime-'):
                for row in rows:
                    semantic = semantic_result(json.dumps(row.get('semantic_result')), workload)
                    if row.get('work_seconds') != semantic['nanoseconds']/1e9:
                        raise ValueError('semantic workload timer differs from its native result')
                    if completed_task_count(semantic) and row.get('task_completions_per_second') != (
                            completed_task_count(semantic)/row['work_seconds']):
                        raise ValueError('semantic workload throughput differs from completed work')
                if summary.get('work') != summarize(measured, 'work_seconds'):
                    raise ValueError('semantic workload execution summary differs from native work')
                if workload.startswith(('runtime-task-', 'runtime-worker-', 'runtime-shared-')) and summary.get(
                        'task_completions_per_second') != summarize(measured, 'task_completions_per_second'):
                    raise ValueError('semantic workload throughput summary differs from native work')
        from scripts.objc3c_toolchain.semantic_import_benchmarks import verify_report
        imports = read('imports/qualification.json')
        if (imports.get('scales') != [32] or imports.get('repetitions') != 3 or
                imports.get('source_graph') is not True or imports.get('budget_seconds') != 600):
            raise ValueError('native semantic imports require the bounded source/PCH/interface and reuse pilot')
        if imports.get('sdk', {}).get('seal_sha256') != result['sdks'][0].get('seal_sha256'):
            raise ValueError('native semantic imports used another SDK')
        return summaries + verify_report(imports, directory / 'imports')
    if name == 'package':
        result = read('release.json')
        return package_execution(result)
    if name == 'sanitizer-products':
        result = read('campaign/qualification.json')
        rows = result.get('owners', [])
        store = result.get('product_evidence_store', {})
        store_path = directory / 'campaign/product-evidence-blobs'
        store_files = list(store_path.iterdir()) if store_path.is_dir() and not store_path.is_symlink() else []
        expected = {'structured', 'native-async', 'coroutines', 'c-frame-bridge', 'installed-ownership',
            *(kind+'-'+sanitizer+'-'+optimization for kind in ('features', 'factories')
              for sanitizer in ('address-undefined', 'thread') for optimization in ('O0', 'O2'))}
        if (len(rows) != len(expected) or {row['name'] for row in rows} != expected or
                any(row.get('status') != 'PASS' for row in rows) or
                len(result.get('runtime_profiles', [])) != 6 or
                result.get('expected_owners') != [row['name'] for row in rows] and
                set(result.get('expected_owners', [])) != expected or
                store.get('schema') != 'objc3c.product-evidence-shared-store.v1' or
                store.get('owners') != len(expected) or not store.get('references') or
                store.get('references') != len(store_files) or
                store.get('stored_bytes') != sum(path.stat().st_size for path in store_files) or
                store.get('private_bytes', 0) < store.get('stored_bytes', 0) or
                store.get('deduplicated_bytes') != store['private_bytes'] - store['stored_bytes'] or
                store.get('path') != 'product-evidence-blobs' or
                any(not path.is_file() or path.is_symlink() or
                    not path.name.endswith('.zst') or len(path.stem) != 64 or
                    any(char not in '0123456789abcdef' for char in path.stem)
                    for path in store_files)):
            raise ValueError('sanitizer product owner/profile matrix is incomplete')
        return sorted(rows, key=lambda row: row['name'])
    if name == 'runtime-schedules':
        result = read('campaign/qualification.json')
        histories, cases = result.get('histories', []), result.get('cases', [])
        scenarios, controls = result.get('scenario_cases', []), result.get('controls', [])
        expected_scenarios = set(result.get('scenario_inventory', {}))
        if (len(histories) != 8 or len(cases) != 8*2*2*3 or len(scenarios) != 2*2*3 or
                len(controls) != 5 or {row.get('name') for row in controls} != {
                    'missing-frame-destruction', 'duplicate-waiter-delivery',
                    'premature-quiescence-publication', 'overlapping-actor-jobs',
                    'type-mismatched-next-consumption'} or
                {scenario for row in scenarios for scenario in row.get('scenarios', [])} != expected_scenarios or
                result.get('reduction', {}).get('status') != 'PASS' or
                result.get('model_coverage', {}).get('missing_transitions') or
                result.get('model_coverage', {}).get('missing_pairs') or
                len(result.get('sanitizer_policy', {})) != 4):
            raise ValueError('generated runtime schedule matrix/controls are incomplete')
        return cases + scenarios + controls
    if name == 'frontend-fuzz':
        result = read('campaign/qualification.json')
        campaigns, controls = result.get('campaigns', []), result.get('controls', [])
        if (len(campaigns) != 3 or {row.get('strategy') for row in campaigns} !=
                {'raw-dictionary', 'structured-pair', 'frontend-syntax-sema'} or
                any(row.get('status') != 'PASS' or row.get('executions', 0) < 5 for row in campaigns) or
                set(result.get('foundation_feedback', {})) != {'libclang-cpp.so', 'libLLVM.so'} or
                result.get('frontend_import_fixture', {}).get('canonical_name') != 'Fuzz.Dependency' or
                set(next((row.get('covered_patched_frontend_functions', {}) for row in campaigns
                          if row.get('strategy') == 'frontend-syntax-sema'), {})) != {'Parse', 'Sema'} or
                len(controls) != 6 or any(row.get('status') != 'PASS' for row in controls) or
                not result.get('temporary_build_reclaimed')):
            raise ValueError('frontend/paired-codec fuzz campaigns or controls are incomplete')
        return campaigns + controls
    if name == 'source-coverage':
        controls = read('controls/qualification.json')
        native = read('first-party/qualification.json')
        compiler = read('compiler/qualification.json')
        if (len(controls.get('controls', [])) != 5 or
                len(native.get('suites', [])) != 14 or
                any(not any(value.get('state') == 'executed' for value in row.get('source_coverage', {}).values())
                    for row in native['suites']) or
                len(compiler.get('suites', [])) != 4 or not compiler.get('eligible') or
                not any(row.get('state') == 'executed' for row in compiler.get('source_coverage', {}).values()) or
                not compiler.get('binary_ids') or not native.get('temporary_build_reclaimed')):
            raise ValueError('first-party/patched compiler source coverage is incomplete')
        return ([{'name': 'coverage-controls', 'status': 'PASS', 'controls': len(controls['controls'])}] +
                [{'name': row['component']+'-'+row['name'], 'status': 'PASS'} for row in native['suites']] +
                [{'name': row['name'], 'status': 'PASS'} for row in compiler['suites']])
    raise ValueError('unrecognized native assurance suite')


def aggregate(plan: dict, root: Path, *, outcomes: dict, tested: str,
              shard: str | None = None) -> dict:
    validate(plan)
    if plan['revision']['tested'] != tested or outcomes.get('plan') != 'success':
        raise ValueError('selection failed or selected the wrong tested revision')
    suites = selected_suites(plan, shard)
    expected = {suite['id'] for suite in suites}
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
    if receipt.get('shard') != shard:
        raise ValueError('native result belongs to a different execution shard')
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
    from scripts.objc3c_toolchain.semantic_build_costs import snapshot
    cost_file = root / 'operational-costs.json'
    saved_costs = receipt.get('operational_costs', {})
    if saved_costs.get('path') != 'operational-costs.json' or artifact(root, cost_file) != saved_costs:
        raise ValueError('native operational measurements are missing or changed')
    observed = json.loads(cost_file.read_text())
    expected_costs = snapshot(foundations=foundation_receipts(root, suites),
        installations=[root / 'package/release.json'] if 'package' in expected else [])
    if observed != expected_costs:
        raise ValueError('native operational measurements differ from actual owner receipts')
    total += saved_costs['bytes']
    if total > MAX_BYTES:
        raise ValueError('native evidence exceeds the 256 MiB artifact budget')
    return {'status': 'PASS', 'classification': 'native', 'revision': plan['revision'],
            'suites': sorted(expected), 'artifact_bytes': total}


def aggregate_shards(plan: dict, root: Path, *, outcomes: dict, tested: str) -> dict:
    shards = execution_shards(plan)
    if not shards:
        return aggregate(plan, root, outcomes=outcomes, tested=tested)
    if (not root.is_dir() or root.is_symlink() or
            {path.name for path in root.iterdir()} != set(shards) or
            any(not (root / name).is_dir() or (root / name).is_symlink() for name in shards)):
        raise ValueError('native evidence has missing, extra or linked execution shards')
    results = [aggregate(plan, root / shard, outcomes=outcomes, tested=tested, shard=shard)
               for shard in shards]
    suites = [name for result in results for name in result['suites']]
    if (len(suites) != len(set(suites)) or
            set(suites) != {row['id'] for row in plan['suites']}):
        raise ValueError('native execution shards overlap or omit selected suites')
    total = sum(row['artifact_bytes'] for row in results)
    if total > MAX_BYTES:
        raise ValueError('native evidence exceeds the 256 MiB artifact budget')
    return {'status': 'PASS', 'classification': 'native', 'revision': plan['revision'],
            'suites': sorted(suites), 'shards': shards, 'artifact_bytes': total}


def publish(plan: dict, root: Path, destination: Path, *, shard: str | None = None) -> dict:
    """Stage only durable evidence, preserving diagnostics from a failed job too.

    Upload a directory containing the shard name so artifact merging cannot
    overwrite another shard's root receipt. Never upload complete build trees or
    follow the per-owner links to the deduplicated product stores.
    """
    suites = selected_suites(plan, shard)
    root = root.resolve(strict=True)
    destination = destination.absolute()
    if destination.exists() or destination.is_symlink() or destination.is_relative_to(root):
        raise ValueError('native evidence publication requires a fresh separate directory')
    files = [root / name for name in ('result.json', 'operational-costs.json')
             if (root / name).exists()]
    for suite in suites:
        files.extend(native_files(root / suite['id']))
    if not files:
        raise ValueError('native job produced no durable evidence to publish')
    destination.mkdir(parents=True)
    total = 0
    for source in files:
        saved = artifact(root, source)
        target = destination / saved['path']
        target.parent.mkdir(parents=True, exist_ok=True)
        try:
            os.link(source, target)
        except OSError:
            shutil.copyfile(source, target)
        if artifact(destination, target) != saved:
            raise ValueError('native evidence changed during publication')
        total += saved['bytes']
    return {'files': len(files), 'bytes': total, 'shard': shard,
            'scope': 'durable native receipts, logs and content-addressed replay inputs'}
