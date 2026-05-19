from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-policy-summary'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-surface-inventory' / 'validation_surface_inventory.json'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
SUMMARY_JSON_PATH = REPORT_DIR / 'policy_summary.json'
SUMMARY_MD_PATH = REPORT_DIR / 'policy_summary.md'




def main() -> None:
    inventory = load_json(INVENTORY_PATH)
    policy = load_json(POLICY_PATH)
    measured = inventory['measured_counts']
    retained = inventory['retained_static_guards']
    summary = {
        'issue': 'validation-policy-summary',
        'policy_id': policy['policy_id'],
        'inventory_issue': inventory['issue'],
        'measured_counts': measured,
        'canonical_truth_order': policy['validation_model']['canonical_truth_order'],
        'retained_static_guard_classes': policy['retained_static_guard_classes'],
        'legacy_surface_states': policy['legacy_surface_lifecycle']['states'],
        'retained_static_guard_paths': [item['path'] for item in retained],
        'unreferenced_check_surfaces': inventory['unreferenced_check_surfaces'],
        'next_issues': ['validation-harness-catalog', 'validation-legacy-surface-map', 'validation-acceptance-artifact-index'],
    }
    write_json_file(SUMMARY_JSON_PATH, summary)

    lines = [
        '# validation-policy-summary Validation Consolidation Policy Summary',
        '',
        f"- policy_id: `{summary['policy_id']}`",
        f"- inventory_issue: `{summary['inventory_issue']}`",
        f"- package_bridge_count: `{measured['package_bridge_count']}`",
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
    lines.extend(['', '## Unreferenced check surfaces queued for validation-legacy-surface-map'])
    for item in summary['unreferenced_check_surfaces']:
        lines.append(f"- `{item}`")
    lines.extend(['', 'Next issues: `validation-harness-catalog`, `validation-legacy-surface-map`, `validation-acceptance-artifact-index`', ''])
    write_text(SUMMARY_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()
