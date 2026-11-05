"""Verify retained, exact-revision implementation-analysis evidence."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path

from .plan import validate

SCHEMA = 'objc3c.implementation-assurance.result.v1'
MAX_BYTES = 256 * 1024 * 1024


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def evidence_files(root: Path) -> list[Path]:
    """Inventory durable receipts/diagnostics without compiler build products."""
    return sorted(path for path in root.rglob('*') if path.is_file() and not path.is_symlink() and
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
            receipt.get('inventory_variants') != len(inventory) or not results or not inventory or
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
    return results


def aggregate(plan: dict, root: Path, *, outcomes: dict, tested: str) -> dict:
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
    if (receipt.get('schema') != SCHEMA or receipt.get('status') != 'PASS' or
            receipt.get('revision') != plan['revision'] or receipt.get('plan_sha256') != plan['sha256'] or
            receipt.get('analysis_paths') != plan['analysis_paths'] or
            receipt.get('temporary_builds_reclaimed') is not True):
        raise ValueError('implementation assurance result is stale, failed or belongs to another revision')
    expected = [artifact(root, path) for path in evidence_files(root)]
    if receipt.get('artifacts') != expected:
        raise ValueError('implementation assurance artifact inventory is missing, substituted or changed')
    if any(row.get('exit_code') != 0 for row in receipt.get('commands', [])):
        raise ValueError('implementation assurance build command failed')
    results = analysis_results(root)
    inner = json.loads((root / 'analysis/qualification.json').read_text())
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
