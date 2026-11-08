"""Verify retained, exact-revision implementation-analysis evidence."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path

from scripts.objc3c_toolchain.analysis_checkpoint import analysis_argv, tidy_diagnostics
from scripts.objc3c_toolchain.implementation_analysis import (
    checker_configuration, owner_changes, selected_variants, translation_unit_timeout)
from scripts.objc3c_toolchain.sanitizer_policy import require_clean_diagnostic
from scripts.objc3c_assurance.evidence import junit, lit

from .plan import validate
from scripts.objc3c_toolchain.analysis_partition import partition_rows, partition_spec

SCHEMA = 'objc3c.implementation-assurance.result.v1'
MAX_BYTES = 256 * 1024 * 1024


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def evidence_files(root: Path) -> list[Path]:
    """Inventory durable receipts/diagnostics without compiler build products."""
    if root.is_symlink() or any(path.is_symlink() for path in root.rglob('*')):
        raise ValueError('implementation assurance evidence contains a symlink')
    return sorted(path for path in root.rglob('*') if path.is_file() and
                  path != root / 'result.json' and path.suffix in ('.json', '.log', '.xml', '.yaml'))


def artifact(root: Path, path: Path) -> dict:
    if path.is_symlink() or not path.is_file() or not path.resolve().is_relative_to(root.resolve()):
        raise ValueError('implementation assurance artifact is not an owned regular file')
    return {'path': path.relative_to(root).as_posix(), 'bytes': path.stat().st_size,
            'sha256': digest(path)}


def analysis_results(root: Path) -> list[dict]:
    receipt = json.loads((root / 'analysis/qualification.json').read_text())
    policy = json.loads((Path(__file__).resolve().parents[2] /
                         'toolchains/llvm/implementation-analysis.json').read_text())
    results, inventory = receipt.get('results', []), receipt.get('inventory', [])
    commands = receipt.get('commands', [])
    control_rows = [row for row in commands if row.get('name', '').startswith('control-')]
    controls = {row.get('name'): row for row in control_rows}
    owners = {'frontend', 'standalone-runtime', 'maintained-clang'}
    inventory_ids = [row.get('identity') for row in inventory]
    result_ids = [row.get('identity') for row in results]
    exclusions = receipt.get('exclusions', [])
    if (receipt.get('schema') != 'objc3c.implementation-analysis.v1' or
            receipt.get('status') != 'PASS' or receipt.get('policy') != policy or
            receipt.get('subject') != 'native-and-maintained-clang-implementation' or
            not receipt.get('source_build_id') or not receipt.get('build_config_digest') or
            not receipt.get('tool', {}).get('sha256') or not receipt.get('tool_graph', {}).get('libraries') or
            not receipt.get('tool_objects') or not receipt.get('inputs') or
            receipt.get('analyzed_variants') != len(results) or
            receipt.get('inventory_variants') != len(inventory) or not inventory or
            receipt.get('fresh_native_executions', 0) + receipt.get('verified_checkpoint_actions', 0) != len(results) or
            any(row.get('status') != 'PASS' or row.get('diagnostics') for row in results) or
            len(set(inventory_ids)) != len(inventory_ids) or
            len(set(result_ids)) != len(result_ids) or not set(result_ids) <= set(inventory_ids) or
            {row.get('owner') for row in inventory} != owners or
            any(not row.get('identity') or not row.get('dependencies') or
                row.get('dependency_origin') not in ('actual-ninja-compilation',
                                                     'fresh-original-compiler-preprocessing')
                for row in inventory) or
            len(exclusions) != len(owners) or {row.get('owner') for row in exclusions} != owners or
            sum(row.get('selected_variants', -1) for row in exclusions) != len(inventory) or
            sum(row.get('analyzed_variants', -1) for row in exclusions) != len(results) or
            len(control_rows) != 2 or set(controls) != {'control-corrected', 'control-defective'} or
            controls['control-corrected'].get('exit_code') != 0 or
            controls['control-corrected'].get('expected_exit_code') != 0 or
            controls['control-defective'].get('exit_code') != 1 or
            controls['control-defective'].get('expected_exit_code') != 1 or
            any(row.get('status') != 'PASS' or row.get('timed_out') or
                row.get('exit_code') != row.get('expected_exit_code') for row in commands)):
        raise ValueError('implementation analysis inventory, controls or clean result is incomplete')
    verify_analysis_proof(root / 'analysis', receipt)
    return results


def retained_path(root: Path, original_root: Path, original: str) -> Path:
    """Relocate producer evidence without admitting traversal or linked inputs."""
    path = Path(original)
    if not path.is_absolute() or '..' in path.parts or not path.is_relative_to(original_root):
        raise ValueError('implementation diagnostic escaped its producer evidence root')
    relocated = root / path.relative_to(original_root)
    if (not relocated.is_file() or relocated.is_symlink() or
            not relocated.resolve().is_relative_to(root.resolve())):
        raise ValueError('implementation diagnostic is missing or linked')
    return relocated


def verify_analysis_proof(root: Path, receipt: dict) -> None:
    """Recompute selection and bind each fresh action to its database and log."""
    configuration = Path(receipt['configuration'])
    original_root = configuration.parent
    if configuration.name != 'checks.json' or not original_root.is_absolute():
        raise ValueError('implementation checker configuration is not owned')
    config = retained_path(root, original_root, str(configuration))
    repo = Path(receipt['repository_output']).parent
    if (digest(config) != receipt['inputs'].get(str(configuration)) or
            json.loads(config.read_text()) != checker_configuration(repo,
                repo / 'out/toolchains/objc3-development/src', receipt['policy'])):
        raise ValueError('implementation checker configuration changed')
    commands = receipt['commands']
    by_name = {row['name']: row for row in commands}
    if len(by_name) != len(commands):
        raise ValueError('implementation analysis commands are duplicated')
    texts = {}
    for command in commands:
        log = retained_path(root, original_root, command['log'])
        if digest(log) != command.get('log_sha256'):
            raise ValueError('implementation analysis log differs from its native command')
        text = log.read_text(errors='replace')
        require_clean_diagnostic(text)
        texts[command['name']] = text
    for name, expected in (('control-corrected', 0), ('control-defective', 1)):
        diagnostics = tidy_diagnostics(texts[name])
        if (len(diagnostics) != expected or expected and
                (diagnostics[0]['check'] != 'clang-analyzer-core.NullDereference' or
                 Path(diagnostics[0]['path']).name != 'control.cpp' or diagnostics[0]['line'] <= 0)):
            raise ValueError('implementation detector control omitted its intended source diagnostic')

    changed = receipt['changed_paths']
    expected_ids = set()
    for owner in ('frontend', 'standalone-runtime', 'maintained-clang'):
        rows = [row for row in receipt['inventory'] if row['owner'] == owner]
        headers = {row['output']: row['dependencies'] for row in rows}
        scoped = owner_changes(repo, owner, changed)
        selected = [] if scoped is None else selected_variants(rows, headers, scoped)
        selected = partition_rows(selected, receipt.get('partition'))
        ids = {row['identity'] for row in selected}
        expected_ids.update(ids)
        if any(type(row.get('selected')) is not bool or row['selected'] != (row['identity'] in ids)
               for row in rows):
            raise ValueError('implementation analysis omitted a dependency-selected translation unit')
        excluded = next(row for row in receipt['exclusions'] if row['owner'] == owner)
        if excluded['selected_variants'] != len(rows) or excluded['analyzed_variants'] != len(ids):
            raise ValueError('implementation owner counts differ from actual selection')
    if expected_ids != {row['identity'] for row in receipt['results']}:
        raise ValueError('implementation analysis results differ from selected translation units')
    # CI deliberately executes all selected actions afresh. Manual checkpoint
    # reuse remains in the development owner with its separate provenance checks.
    if (receipt.get('verified_checkpoint_actions') != 0 or
            receipt.get('fresh_native_executions') != len(expected_ids) or
            any(row.get('execution_reused') is not False for row in receipt['results'])):
        raise ValueError('implementation CI requires fresh native analysis actions')
    for row in receipt['inventory']:
        if not row['selected']:
            continue
        name = row['owner']+'-'+row['identity'][:16]
        timeout = translation_unit_timeout(receipt['policy'], row,
            repo / 'out/toolchains/objc3-development/src')
        result = next(value for value in receipt['results'] if value['identity'] == row['identity'])
        if (row.get('analysis_timeout_seconds') != timeout or
                result.get('timeout_seconds') != timeout):
            raise ValueError('implementation action differs from its reviewed resource bound')
        command = by_name.get(name)
        expected = analysis_argv(receipt['tool']['path'], original_root / name,
                                  row['file'], configuration, receipt['resource_directory'])
        if not command or command.get('argv') != expected or tidy_diagnostics(texts[name]):
            raise ValueError('implementation action lacks its exact clean native command')
        database = retained_path(root, original_root,
                                 str(original_root / name / 'compile_commands.json'))
        if json.loads(database.read_text()) != [{'file': row['file'], 'directory': row['directory'],
                                                'arguments': row['analysis_arguments']}]:
            raise ValueError('implementation action database differs from its retained variant')


def verify_build_proof(root: Path, receipt: dict) -> None:
    commands = receipt.get('commands', [])
    names = {row.get('name') for row in commands}
    if len(commands) != 4 or names != {'frontend-configure', 'frontend-build',
                                      'runtime-configure', 'runtime-build'}:
        raise ValueError('implementation assurance omitted its native build commands')
    original_root = Path(receipt['evidence_root'])
    for command in commands:
        log = retained_path(root, original_root, command['log'])
        if (command.get('exit_code') != 0 or command.get('timed_out') is not False or
                digest(log) != command.get('log_sha256')):
            raise ValueError('implementation assurance build command or diagnostic is incomplete')
        require_clean_diagnostic(log.read_text(errors='replace'))
        if command['name'] == 'frontend-configure' and not {
                '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-asan-ubsan',
                '-DOBJC3C_LINK_CLANG_CPP=ON'} <= set(command.get('argv', [])):
            raise ValueError('implementation frontend did not select the matching instrumented foundation')
    foundation = json.loads((root / 'foundation/foundation.json').read_text())
    phases = foundation.get('phases', [])
    if (foundation.get('status') != 'passed' or
            foundation.get('source_identity') != receipt.get('source_build_id') or
            {row.get('phase') for row in phases} != {'configure', 'build', 'test', 'stage'} or
            any(row.get('exit_code') != 0 for row in phases)):
        raise ValueError('implementation foundation is incomplete or mismatched')
    tests = []
    for phase in phases:
        if phase['phase'] == 'test':
            name = Path(phase['test_results']).name
            path = root / 'foundation' / name
            tests.extend(lit(path) if path.suffix == '.json' else junit(path))
    if not tests:
        raise ValueError('implementation foundation omitted actual native test results')


def _aggregate_partition(plan: dict, root: Path, *, outcomes: dict, tested: str,
                         partition=None) -> dict:
    validate(plan)
    if plan['revision']['tested'] != tested or outcomes.get('plan') != 'success':
        raise ValueError('implementation assurance selected the wrong source revision')
    if not plan['required']:
        if outcomes.get('analysis') != 'skipped':
            raise ValueError('unaffected implementation assurance must be skipped')
        return {'status': 'PASS', 'classification': 'unaffected', 'revision': plan['revision']}
    if outcomes.get('analysis') != 'success':
        raise ValueError('required implementation analysis failed, was cancelled, skipped or incomplete')
    receipt = json.loads((root / 'result.json').read_text())
    if receipt.get('partition', partition_spec()) != (partition or partition_spec()):
        raise ValueError('implementation result belongs to the wrong partition')
    if (receipt.get('schema') != SCHEMA or receipt.get('status') != 'PASS' or
            receipt.get('revision') != plan['revision'] or receipt.get('plan_sha256') != plan['sha256'] or
            receipt.get('analysis_paths') != plan['analysis_paths'] or
            receipt.get('temporary_builds_reclaimed') is not True):
        raise ValueError('implementation assurance result is stale, failed or belongs to another revision')
    expected = [artifact(root, path) for path in evidence_files(root)]
    if receipt.get('artifacts') != expected:
        raise ValueError('implementation assurance artifact inventory is missing, substituted or changed')
    verify_build_proof(root, receipt)
    results = analysis_results(root)
    inner = json.loads((root / 'analysis/qualification.json').read_text())
    if inner.get('partition', partition_spec()) != (partition or partition_spec()):
        raise ValueError('implementation inner partition differs from its owner')
    producer_repo = Path(inner['repository_output']).parent
    if inner.get('changed_paths') != [str(producer_repo / path) for path in plan['analysis_paths']]:
        raise ValueError('implementation analysis used a different changed-path selection')
    if (receipt.get('analyzed_variants') != len(results) or
            receipt.get('inventory_variants') != inner.get('inventory_variants') or
            receipt.get('source_build_id') != inner.get('source_build_id') or
            receipt.get('build_config_digest') != inner.get('build_config_digest')):
        raise ValueError('implementation assurance outer result differs from its analyzer receipt')
    total = sum(row['bytes'] for row in expected)
    if total > MAX_BYTES:
        raise ValueError('implementation assurance evidence exceeds the 256 MiB artifact budget')
    return {'status': 'PASS', 'classification': plan['selection'], 'revision': plan['revision'],
            'analyzed_variants': len(results), 'artifact_bytes': total}


def aggregate(plan: dict, root: Path, *, outcomes: dict, tested: str) -> dict:
    validate(plan)
    count = plan['analysis_partitions']
    if count == 1 or not plan['required']:
        return _aggregate_partition(plan, root, outcomes=outcomes, tested=tested)
    if root.is_symlink() or not root.is_dir():
        raise ValueError('implementation partition evidence is missing or linked')
    roots = sorted(root.iterdir())
    if len(roots) != count or any(path.is_symlink() or not path.is_dir() for path in roots):
        raise ValueError('implementation analysis requires every partition artifact')
    indices, results, variants, signatures = set(), [], [], set()
    logical = None
    total = 0
    for directory in roots:
        outer = json.loads((directory / 'result.json').read_text())
        spec = outer.get('partition', {})
        if (type(spec) is not dict or spec != partition_spec(spec.get('index'), count) or
                spec['index'] in indices):
            raise ValueError('implementation partition is missing, duplicated or substituted')
        indices.add(spec['index'])
        admitted = _aggregate_partition(plan, directory, outcomes=outcomes, tested=tested, partition=spec)
        total += admitted['artifact_bytes']
        inner = json.loads((directory / 'analysis/qualification.json').read_text())
        signatures.add((inner['source_build_id'], inner['build_config_digest']))
        # PCH projection and per-action arguments exist only on that action's owner.
        common = sorted([{key:value for key,value in row.items()
                          if key not in ('selected', 'analysis_arguments', 'pch_projection')}
                         for row in inner['inventory']], key=lambda row:row['identity'])
        hashes = {path:inner['inputs'].get(path) for row in common for path in row['dependencies']}
        if any(not isinstance(value, str) or len(value)!=64 for value in hashes.values()):
            raise ValueError('implementation partition omitted frozen source/header inputs')
        current = {'inventory':common, 'dependency_sha256':hashes,
                   'changed_paths':inner['changed_paths']}
        first = logical is None
        if not first and current != logical:
            raise ValueError('implementation partitions analyzed different inventories or inputs')
        logical = current
        results.extend(row['identity'] for row in inner['results'])
        if first:
            repo = Path(inner['repository_output']).parent
            for owner in ('frontend','standalone-runtime','maintained-clang'):
                rows = [row for row in common if row['owner']==owner]
                headers = {row['output']:row['dependencies'] for row in rows}
                scoped = owner_changes(repo, owner, inner['changed_paths'])
                if scoped is not None:
                    variants.extend(row['identity'] for row in selected_variants(rows,headers,scoped))
    if (indices != set(range(count)) or len(signatures)!=1 or
            len(results)!=len(set(results)) or set(results)!=set(variants)):
        raise ValueError('implementation partitions do not cover the complete selected inventory once')
    if total > MAX_BYTES:
        raise ValueError('combined implementation partition evidence exceeds its artifact budget')
    return {'status':'PASS', 'classification':plan['selection'], 'revision':plan['revision'],
            'partitions':count, 'analyzed_variants':len(results), 'artifact_bytes':total,
            'inventory_sha256':hashlib.sha256(json.dumps(logical,sort_keys=True).encode()).hexdigest()}
