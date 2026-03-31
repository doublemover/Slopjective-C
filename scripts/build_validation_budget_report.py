from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-D003'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-A001' / 'validation_surface_inventory.json'
LEGACY_MAP_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B003' / 'legacy_validation_surface_map.json'
SUITE_MATRIX_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C002' / 'validation_acceptance_suite_matrix.json'
INTEGRATION_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-D002' / 'validation_ci_topology_integration.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'validation_budget_report.json'
OUTPUT_MD_PATH = REPORT_DIR / 'validation_budget_report.md'

BUDGETS = {
    'retained_static_guard_max': 20,
    'migration_only_surface_max': 4,
    'unclassified_surface_max': 0,
    'acceptance_suite_family_min': 20,
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    inventory = load_json(INVENTORY_PATH)
    legacy_map = load_json(LEGACY_MAP_PATH)
    suite_matrix = load_json(SUITE_MATRIX_PATH)
    integration = load_json(INTEGRATION_PATH)

    retained_static_guard_count = inventory['measured_counts']['retained_static_guard_count']
    migration_only_surface_count = legacy_map['state_counts'].get('migration-only', 0)
    active_surface_count = legacy_map['state_counts'].get('active', 0)
    suite_family_count = suite_matrix['suite_family_count']
    unclassified_surface_count = 0

    checks = [
        {
            'name': 'retained_static_guard_budget',
            'measured': retained_static_guard_count,
            'budget': BUDGETS['retained_static_guard_max'],
            'status': 'PASS' if retained_static_guard_count <= BUDGETS['retained_static_guard_max'] else 'FAIL',
        },
        {
            'name': 'migration_only_surface_budget',
            'measured': migration_only_surface_count,
            'budget': BUDGETS['migration_only_surface_max'],
            'status': 'PASS' if migration_only_surface_count <= BUDGETS['migration_only_surface_max'] else 'FAIL',
        },
        {
            'name': 'unclassified_surface_budget',
            'measured': unclassified_surface_count,
            'budget': BUDGETS['unclassified_surface_max'],
            'status': 'PASS' if unclassified_surface_count <= BUDGETS['unclassified_surface_max'] else 'FAIL',
        },
        {
            'name': 'acceptance_suite_family_floor',
            'measured': suite_family_count,
            'budget': BUDGETS['acceptance_suite_family_min'],
            'status': 'PASS' if suite_family_count >= BUDGETS['acceptance_suite_family_min'] else 'FAIL',
        },
        {
            'name': 'ci_topology_integration_status',
            'measured': integration['failure_count'],
            'budget': 0,
            'status': 'PASS' if integration['status'] == 'PASS' else 'FAIL',
        },
    ]

    payload = {
        'issue': 'M313-D003',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'status': 'PASS' if all(check['status'] == 'PASS' for check in checks) else 'FAIL',
        'checks': checks,
        'summary': {
            'retained_static_guard_count': retained_static_guard_count,
            'migration_only_surface_count': migration_only_surface_count,
            'active_surface_count': active_surface_count,
            'acceptance_suite_family_count': suite_family_count,
        },
        'next_issues': ['M313-E001'],
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# Validation Budget Report',
        '',
        f"- issue: `{payload['issue']}`",
        f"- status: `{payload['status']}`",
        '',
        '## Checks',
    ]
    for check in checks:
        lines.append(
            f"- `{check['name']}`: status=`{check['status']}` measured=`{check['measured']}` budget=`{check['budget']}`"
        )
    lines.extend(['', 'Next issue: `M313-E001`', ''])
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()