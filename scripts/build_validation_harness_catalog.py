from __future__ import annotations

import json
import re
import subprocess
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-harness-catalog'
PACKAGE_JSON_PATH = ROOT / 'package.json'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-surface-inventory' / 'validation_surface_inventory.json'
CATALOG_JSON_PATH = PLAN_DIR / 'validation_harness_catalog.json'
CATALOG_MD_PATH = PLAN_DIR / 'validation_harness_catalog.md'
SUMMARY_JSON_PATH = REPORT_DIR / 'validation_harness_catalog.json'
SUMMARY_MD_PATH = REPORT_DIR / 'validation_harness_catalog.md'
HARNESS_LIST_COMMAND = ['python', 'scripts/shared_compiler_runtime_acceptance_harness.py', '--list-suites']
WORKFLOW_PREFIX = 'python scripts/objc3c_public_workflow_runner.py '


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def run_json(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(command, cwd=ROOT, check=True, capture_output=True, text=True)
    return json.loads(result.stdout)


def extract_workflow_action(command: str) -> str | None:
    if not command.startswith(WORKFLOW_PREFIX):
        return None
    suffix = command[len(WORKFLOW_PREFIX):].strip()
    if not suffix or suffix.startswith('--'):
        return None
    return suffix.split()[0]


def classify_family(script_name: str, action: str) -> str:
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
        if token in script_name or token in action:
            return family
    if action in {'test-full', 'test-nightly', 'test-fast'}:
        return 'aggregate-validation'
    if script_name.startswith('check:'):
        return 'static-guard-surface'
    return 'misc'


def describe_tier(script_name: str, action: str) -> str:
    if ':e2e' in script_name or action.startswith('validate-runnable-'):
        return 'runnable'
    if ':integration' in script_name or action.endswith('-integration'):
        return 'integration'
    if script_name.startswith('check:'):
        return 'static-guard'
    return 'acceptance'


def main() -> None:
    package_json = load_json(PACKAGE_JSON_PATH)
    policy = load_json(POLICY_PATH)
    inventory = load_json(INVENTORY_PATH)
    harness = run_json(HARNESS_LIST_COMMAND)
    scripts = package_json['scripts']

    workflow_entries: list[dict[str, str]] = []
    direct_entries: list[dict[str, str]] = []
    family_map: dict[str, dict[str, Any]] = defaultdict(lambda: {
        'family': '',
        'actions': [],
        'package_scripts': [],
        'tiers': [],
    })

    for script_name, command in scripts.items():
        action = extract_workflow_action(command)
        if action is not None:
            family = classify_family(script_name, action)
            tier = describe_tier(script_name, action)
            workflow_entries.append({
                'package_script': script_name,
                'action': action,
                'family': family,
                'tier': tier,
            })
            bucket = family_map[family]
            bucket['family'] = family
            bucket['actions'].append(action)
            bucket['package_scripts'].append(script_name)
            bucket['tiers'].append(tier)
        elif script_name.startswith('test:') or script_name.startswith('check:'):
            direct_entries.append({
                'package_script': script_name,
                'command': command,
                'family': classify_family(script_name, command),
            })

    public_workflow_families = []
    for family in sorted(family_map):
        bucket = family_map[family]
        public_workflow_families.append({
            'family': family,
            'action_count': len(set(bucket['actions'])),
            'actions': sorted(set(bucket['actions'])),
            'package_scripts': sorted(set(bucket['package_scripts'])),
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
            'script_count': len(workflow_entries),
            'families': public_workflow_families,
        },
        'direct_non_runner_validation_scripts': direct_entries,
        'retained_static_guard_classes': policy['retained_static_guard_classes'],
        'migration_targets': {
            'primary_shared_harness': 'scripts/shared_compiler_runtime_acceptance_harness.py',
            'primary_public_runner': 'scripts/objc3c_public_workflow_runner.py',
            'legacy_namespace_work': 'validation-legacy-surface-map',
            'artifact_contract_work': 'validation-acceptance-artifact-index',
        },
    }

    write_text(CATALOG_JSON_PATH, json.dumps(catalog, indent=2) + '\n')
    write_text(SUMMARY_JSON_PATH, json.dumps(catalog, indent=2) + '\n')

    lines = [
        '# Validation Harness Catalog',
        '',
        f"- issue: `{catalog['issue']}`",
        f"- policy_id: `{catalog['policy_id']}`",
        f"- harness_path: `{catalog['shared_acceptance_harness']['harness_path']}`",
        f"- harness_suite_count: `{catalog['shared_acceptance_harness']['suite_count']}`",
        f"- workflow_action_count: `{catalog['public_workflow_validation']['action_count']}`",
        f"- workflow_script_count: `{catalog['public_workflow_validation']['script_count']}`",
        '',
        '## Shared acceptance harness suites',
    ]
    for suite_id in catalog['shared_acceptance_harness']['suite_ids']:
        lines.append(f"- `{suite_id}`")
    lines.extend(['', '## Public workflow validation families'])
    for family in public_workflow_families:
        lines.append(
            f"- `{family['family']}`: `{family['action_count']}` actions, `{len(family['package_scripts'])}` package scripts, tiers=`{', '.join(family['tiers'])}`"
        )
    lines.extend(['', '## Direct non-runner validation scripts'])
    if direct_entries:
        for entry in direct_entries:
            lines.append(f"- `{entry['package_script']}` -> `{entry['command']}`")
    else:
        lines.append('- none')
    lines.extend([
        '',
        '## Migration targets',
        f"- primary shared harness: `{catalog['migration_targets']['primary_shared_harness']}`",
        f"- primary public runner: `{catalog['migration_targets']['primary_public_runner']}`",
        f"- legacy namespace work: `{catalog['migration_targets']['legacy_namespace_work']}`",
        f"- artifact contract work: `{catalog['migration_targets']['artifact_contract_work']}`",
        '',
    ])
    markdown = '\n'.join(lines)
    write_text(CATALOG_MD_PATH, markdown)
    write_text(SUMMARY_MD_PATH, markdown)


if __name__ == '__main__':
    main()