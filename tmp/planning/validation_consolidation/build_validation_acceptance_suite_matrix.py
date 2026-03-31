from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C002'
CATALOG_PATH = PLAN_DIR / 'validation_harness_catalog.json'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
OUTPUT_JSON_PATH = PLAN_DIR / 'validation_acceptance_suite_matrix.json'
OUTPUT_MD_PATH = PLAN_DIR / 'validation_acceptance_suite_matrix.md'
REPORT_JSON_PATH = REPORT_DIR / 'validation_acceptance_suite_matrix.json'
REPORT_MD_PATH = REPORT_DIR / 'validation_acceptance_suite_matrix.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    catalog = load_json(CATALOG_PATH)
    policy = load_json(POLICY_PATH)
    families = []
    for family in catalog['public_workflow_validation']['families']:
        if family['family'] in {'misc', 'static-guard-surface'}:
            continue
        families.append({
            'suite_family': family['family'],
            'canonical_actions': family['actions'],
            'package_scripts': family['package_scripts'],
            'tiers': family['tiers'],
            'suite_owner': 'scripts/objc3c_public_workflow_runner.py',
        })

    payload = {
        'issue': 'M313-C002',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'policy_id': policy['policy_id'],
        'shared_acceptance_harness': catalog['shared_acceptance_harness'],
        'suite_family_count': len(families),
        'suite_families': families,
        'aggregate_entrypoints': [
            {
                'action': 'test-fast',
                'package_script': 'test:fast',
                'role': 'developer-fast-aggregate',
            },
            {
                'action': 'test-full',
                'package_script': 'test:objc3c:full',
                'role': 'developer-full-aggregate',
            },
            {
                'action': 'test-nightly',
                'package_script': 'test:objc3c:nightly',
                'role': 'nightly-aggregate',
            },
        ],
        'next_issues': ['M313-D001', 'M313-D002'],
    }

    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

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
        lines.append(f"  - package_scripts: {', '.join(f'`{item}`' for item in family['package_scripts'])}")
    lines.extend(['', '## Aggregate entrypoints'])
    for entry in payload['aggregate_entrypoints']:
        lines.append(f"- `{entry['package_script']}` -> `{entry['action']}` ({entry['role']})")
    lines.extend(['', 'Next issues: `M313-D001`, `M313-D002`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()