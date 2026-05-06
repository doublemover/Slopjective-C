from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-acceptance-suite-matrix'
CATALOG_PATH = PLAN_DIR / 'validation_harness_catalog.json'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
OUTPUT_JSON_PATH = PLAN_DIR / 'validation_acceptance_suite_matrix.json'
OUTPUT_MD_PATH = PLAN_DIR / 'validation_acceptance_suite_matrix.md'
REPORT_JSON_PATH = REPORT_DIR / 'validation_acceptance_suite_matrix.json'
REPORT_MD_PATH = REPORT_DIR / 'validation_acceptance_suite_matrix.md'
DEFAULT_POLICY = {
    'policy_id': 'objc3c.validation_consolidation_policy.v1',
}


def load_policy() -> dict[str, Any]:
    if POLICY_PATH.is_file():
        return load_json(POLICY_PATH)
    return DEFAULT_POLICY




def main() -> None:
    catalog = load_json(CATALOG_PATH)
    policy = load_policy()
    families = []
    for family in catalog['public_workflow_validation']['families']:
        if family['family'] in {'misc', 'static-guard-surface'}:
            continue
        families.append({
            'suite_family': family['family'],
            'canonical_actions': family['actions'],
            'public_commands': family['public_commands'],
            'tiers': family['tiers'],
            'suite_owner': 'scripts.objc3c_workflow',
        })

    payload = {
        'issue': 'validation-acceptance-suite-matrix',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'policy_id': policy['policy_id'],
        'shared_acceptance_harness': catalog['shared_acceptance_harness'],
        'suite_family_count': len(families),
        'suite_families': families,
        'aggregate_entrypoints': [
            {
                'action': 'test-smoke',
                'public_command': 'npm run objc3c -- test-smoke',
                'role': 'developer-smoke-aggregate',
            },
            {
                'action': 'test-full',
                'public_command': 'npm run objc3c -- test-full',
                'role': 'developer-full-aggregate',
            },
            {
                'action': 'test-nightly',
                'public_command': 'npm run objc3c -- test-nightly',
                'role': 'nightly-aggregate',
            },
        ],
        'next_issues': ['validation-ci-topology', 'validation-ci-topology-integration'],
    }

    write_json_file(OUTPUT_JSON_PATH, payload)
    write_json_file(REPORT_JSON_PATH, payload)

    lines = [
        '# Validation Acceptance Suite Matrix',
        '',
        f"- issue: `{payload['issue']}`",
        f"- policy_id: `{payload['policy_id']}`",
        f"- suite_family_count: `{payload['suite_family_count']}`",
        '',
        '## Shared acceptance harness suites',
    ]
    for suite_id in payload['shared_acceptance_harness']['suite_ids']:
        lines.append(f"- `{suite_id}`")
    lines.extend(['', '## Canonical suite families'])
    for family in families:
        lines.append(f"- `{family['suite_family']}`")
        lines.append(f"  - tiers: `{', '.join(family['tiers'])}`")
        lines.append(f"  - owner: `{family['suite_owner']}`")
        lines.append(f"  - public_commands: {', '.join(f'`{item}`' for item in family['public_commands'])}")
    lines.extend(['', '## Aggregate entrypoints'])
    for entry in payload['aggregate_entrypoints']:
        lines.append(f"- `{entry['public_command']}` -> `{entry['action']}` ({entry['role']})")
    lines.extend(['', 'Next issues: `validation-ci-topology`, `validation-ci-topology-integration`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
