from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B001'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-A001' / 'validation_surface_inventory.json'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
SUMMARY_JSON_PATH = REPORT_DIR / 'policy_summary.json'
SUMMARY_MD_PATH = REPORT_DIR / 'policy_summary.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    inventory = load_json(INVENTORY_PATH)
    policy = load_json(POLICY_PATH)
    measured = inventory['measured_counts']
    retained = inventory['retained_static_guards']
    summary = {
        'issue': 'M313-B001',
        'policy_id': policy['policy_id'],
        'inventory_issue': inventory['issue'],
        'measured_counts': measured,
        'canonical_truth_order': policy['validation_model']['canonical_truth_order'],
        'retained_static_guard_classes': policy['retained_static_guard_classes'],
        'legacy_surface_states': policy['legacy_surface_lifecycle']['states'],
        'retained_static_guard_paths': [item['path'] for item in retained],
        'unreferenced_check_surfaces': inventory['unreferenced_check_surfaces'],
        'next_issues': ['M313-B002', 'M313-B003', 'M313-C001'],
    }
    write_text(SUMMARY_JSON_PATH, json.dumps(summary, indent=2) + '\n')

    lines = [
        '# M313-B001 Validation Consolidation Policy Summary',
        '',
        f"- policy_id: `{summary['policy_id']}`",
        f"- inventory_issue: `{summary['inventory_issue']}`",
        f"- package_scripts_total: `{measured['package_scripts_total']}`",
        f"- check_py_files: `{measured['check_py_files']}`",
        f"- retained_static_guard_count: `{measured['retained_static_guard_count']}`",
        f"- executable_validation_count: `{measured['executable_validation_count']}`",
        '',
        '## Canonical truth order',
    ]
    for item in summary['canonical_truth_order']:
        lines.append(f"- `{item}`")
    lines.extend(['', '## Retained static guard classes'])
    for item in summary['retained_static_guard_classes']:
        lines.append(f"- `{item}`")
    lines.extend(['', '## Legacy surface lifecycle states'])
    for item in summary['legacy_surface_states']:
        lines.append(f"- `{item}`")
    lines.extend(['', '## Unreferenced check surfaces queued for M313-B003'])
    for item in summary['unreferenced_check_surfaces']:
        lines.append(f"- `{item}`")
    lines.extend(['', 'Next issues: `M313-B002`, `M313-B003`, `M313-C001`', ''])
    write_text(SUMMARY_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()