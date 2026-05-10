from __future__ import annotations

import json
import subprocess
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_list_command
from objc3c_tooling.subprocesses import python_script_command

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-harness-catalog'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-surface-inventory' / 'validation_surface_inventory.json'
CATALOG_JSON_PATH = PLAN_DIR / 'validation_harness_catalog.json'
CATALOG_MD_PATH = PLAN_DIR / 'validation_harness_catalog.md'
SUMMARY_JSON_PATH = REPORT_DIR / 'validation_harness_catalog.json'
SUMMARY_MD_PATH = REPORT_DIR / 'validation_harness_catalog.md'
HARNESS_LIST_COMMAND = python_script_command('scripts/shared_compiler_runtime_acceptance_harness.py', '--list-suites')
WORKFLOW_LIST_COMMAND = public_workflow_list_command()
PUBLIC_NPM_BRIDGE = 'npm run objc3c -- '
DEFAULT_POLICY = {
    'policy_id': 'objc3c.validation_consolidation_policy.v1',
    'retained_static_guard_classes': [
        'retain:task-hygiene',
        'retain:repo-shape',
        'retain:docs-surface',
        'retain:product-surface',
        'retain:source-surface-contract',
        'retain:schema-contract',
    ],
}




def run_json(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(command, cwd=ROOT, check=True, capture_output=True, text=True)
    return json.loads(result.stdout)


def load_policy() -> dict[str, Any]:
    if POLICY_PATH.is_file():
        return load_json(POLICY_PATH)
    return DEFAULT_POLICY


def classify_family(action: str) -> str:
    for token, family in (
        ('showcase', 'showcase'),
        ('stdlib', 'stdlib'),
        ('performance-governance', 'performance-governance'),
        ('performance', 'performance'),
        ('compiler-throughput', 'compiler-throughput'),
        ('runtime-performance', 'runtime-performance'),
        ('bonus-experiences', 'bonus-experiences'),
        ('conformance-corpus', 'conformance-corpus'),
        ('public-conformance', 'public-conformance'),
        ('external-validation', 'external-validation'),
        ('packaging-channels', 'packaging-channels'),
        ('release-foundation', 'release-foundation'),
        ('release-operations', 'release-operations'),
        ('distribution-credibility', 'distribution-credibility'),
        ('stress', 'stress'),
        ('fuzz', 'stress'),
        ('runtime-architecture', 'runtime-architecture'),
        ('block-arc', 'runtime-closure'),
        ('concurrency', 'runtime-closure'),
        ('object-model', 'runtime-closure'),
        ('storage-reflection', 'runtime-closure'),
        ('error', 'runtime-closure'),
        ('interop', 'runtime-closure'),
        ('metaprogramming', 'runtime-closure'),
        ('release-candidate', 'runtime-closure'),
        ('runnable-bootstrap', 'runtime-closure'),
        ('getting-started', 'onboarding'),
        ('documentation', 'docs'),
        ('repo', 'repo-shape'),
        ('site', 'docs'),
        ('native-docs', 'docs'),
    ):
        if token in action:
            return family
    if action in {'test-full', 'test-nightly', 'test-smoke'}:
        return 'aggregate-validation'
    if action.startswith('check-'):
        return 'static-guard-surface'
    return 'misc'


def describe_tier(action: str) -> str:
    if action.startswith('validate-runnable-') or action.endswith('-end-to-end'):
        return 'runnable'
    if action.endswith('-integration'):
        return 'integration'
    if action.startswith('check-'):
        return 'static-guard'
    if action in {'test-full', 'test-nightly', 'test-smoke'}:
        return 'aggregate'
    return 'acceptance'


def main() -> None:
    policy = load_policy()
    inventory = load_json(INVENTORY_PATH)
    harness = run_json(HARNESS_LIST_COMMAND)
    workflow = run_json(WORKFLOW_LIST_COMMAND)

    workflow_entries: list[dict[str, str]] = []
    family_map: dict[str, dict[str, Any]] = defaultdict(lambda: {
        'family': '',
        'actions': [],
        'public_commands': [],
        'tiers': [],
    })

    for entry in workflow['actions']:
        action = str(entry['action'])
        family = classify_family(action)
        tier = describe_tier(action)
        public_command = f'{PUBLIC_NPM_BRIDGE}{action}'
        workflow_entries.append({
            'action': action,
            'public_command': public_command,
            'family': family,
            'tier': tier,
        })
        bucket = family_map[family]
        bucket['family'] = family
        bucket['actions'].append(action)
        bucket['public_commands'].append(public_command)
        bucket['tiers'].append(tier)

    public_workflow_families = []
    for family in sorted(family_map):
        bucket = family_map[family]
        public_workflow_families.append({
            'family': family,
            'action_count': len(set(bucket['actions'])),
            'actions': sorted(set(bucket['actions'])),
            'public_commands': sorted(set(bucket['public_commands'])),
            'tiers': sorted(set(bucket['tiers'])),
        })

    catalog = {
        'issue': 'validation-harness-catalog',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'policy_id': policy['policy_id'],
        'inventory_issue': inventory['issue'],
        'shared_acceptance_harness': {
            'contract_id': harness['contract_id'],
            'harness_path': harness['harness_path'],
            'suite_count': harness['suite_count'],
            'suite_ids': [entry['suite_id'] for entry in harness['suites']],
        },
        'public_workflow_validation': {
            'action_count': len({entry['action'] for entry in workflow_entries}),
            'command_count': len(workflow_entries),
            'families': public_workflow_families,
        },
        'direct_non_runner_validation_commands': [],
        'retained_static_guard_classes': policy['retained_static_guard_classes'],
        'migration_targets': {
            'primary_shared_harness': 'scripts/shared_compiler_runtime_acceptance_harness.py',
            'primary_public_workflow_module': 'scripts.objc3c_workflow',
            'legacy_namespace_work': 'validation-legacy-surface-map',
            'artifact_contract_work': 'validation-acceptance-artifact-index',
        },
    }

    write_json_file(CATALOG_JSON_PATH, catalog)
    write_json_file(SUMMARY_JSON_PATH, catalog)

    lines = [
        '# Validation Harness Catalog',
        '',
        f"- issue: `{catalog['issue']}`",
        f"- policy_id: `{catalog['policy_id']}`",
        f"- harness_path: `{catalog['shared_acceptance_harness']['harness_path']}`",
        f"- harness_suite_count: `{catalog['shared_acceptance_harness']['suite_count']}`",
        f"- workflow_action_count: `{catalog['public_workflow_validation']['action_count']}`",
        f"- workflow_command_count: `{catalog['public_workflow_validation']['command_count']}`",
        '',
        '## Shared acceptance harness suites',
    ]
    for suite_id in catalog['shared_acceptance_harness']['suite_ids']:
        lines.append(f"- `{suite_id}`")
    lines.extend(['', '## Public workflow validation families'])
    for family in public_workflow_families:
        lines.append(
            f"- `{family['family']}`: `{family['action_count']}` actions, `{len(family['public_commands'])}` public commands, tiers=`{', '.join(family['tiers'])}`"
        )
    lines.extend(['', '## Direct non-runner validation commands', '- none'])
    lines.extend([
        '',
        '## Migration targets',
        f"- primary shared harness: `{catalog['migration_targets']['primary_shared_harness']}`",
        f"- primary public workflow module: `{catalog['migration_targets']['primary_public_workflow_module']}`",
        f"- legacy namespace work: `{catalog['migration_targets']['legacy_namespace_work']}`",
        f"- artifact contract work: `{catalog['migration_targets']['artifact_contract_work']}`",
        '',
    ])
    markdown = '\n'.join(lines)
    write_text(CATALOG_MD_PATH, markdown)
    write_text(SUMMARY_MD_PATH, markdown)


if __name__ == '__main__':
    main()
