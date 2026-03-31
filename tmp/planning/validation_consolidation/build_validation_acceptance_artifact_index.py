from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C001'
SCHEMA_PATH = ROOT / 'schemas' / 'objc3c-validation-acceptance-artifact-index-v1.schema.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'validation_acceptance_artifact_index.json'
OUTPUT_MD_PATH = REPORT_DIR / 'validation_acceptance_artifact_index.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def validate_payload(payload: dict[str, Any]) -> None:
    if payload.get('contract_id') != 'objc3c.validation.acceptance.artifact.index.v1':
        raise RuntimeError('contract_id drifted')
    artifacts = payload.get('artifacts')
    if not isinstance(artifacts, list) or not artifacts:
        raise RuntimeError('artifacts must be a non-empty list')
    if payload.get('artifact_count') != len(artifacts):
        raise RuntimeError('artifact_count drifted from artifacts length')
    next_issues = payload.get('next_issues')
    if not isinstance(next_issues, list) or not next_issues:
        raise RuntimeError('next_issues must be a non-empty list')


def main() -> None:
    inventory = load_json(ROOT / 'tmp' / 'reports' / 'm313' / 'M313-A001' / 'validation_surface_inventory.json')
    policy_summary = load_json(ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B001' / 'policy_summary.json')
    harness_catalog = load_json(ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B002' / 'validation_harness_catalog.json')
    legacy_map = load_json(ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B003' / 'legacy_validation_surface_map.json')

    payload = {
        'contract_id': 'objc3c.validation.acceptance.artifact.index.v1',
        'generated_at_utc': datetime.now(timezone.utc).isoformat(),
        'milestone_code': 'M313',
        'policy_id': policy_summary['policy_id'],
        'artifact_count': 4,
        'artifacts': [
            {
                'issue_code': 'M313-A001',
                'artifact_kind': 'validation-surface-inventory',
                'report_path': 'tmp/reports/m313/M313-A001/validation_surface_inventory.json',
                'planning_source_paths': [
                    'tmp/planning/validation_consolidation/build_validation_surface_inventory.py'
                ],
                'highlights': {
                    'package_scripts_total': inventory['measured_counts']['package_scripts_total'],
                    'check_py_files': inventory['measured_counts']['check_py_files'],
                    'retained_static_guard_count': inventory['measured_counts']['retained_static_guard_count'],
                    'executable_validation_count': inventory['measured_counts']['executable_validation_count'],
                },
            },
            {
                'issue_code': 'M313-B001',
                'artifact_kind': 'validation-policy',
                'report_path': 'tmp/reports/m313/M313-B001/policy_summary.json',
                'planning_source_paths': [
                    'tmp/planning/validation_consolidation/validation_consolidation_policy.json',
                    'tmp/planning/validation_consolidation/build_validation_policy_summary.py'
                ],
                'highlights': {
                    'canonical_truth_order': policy_summary['canonical_truth_order'],
                    'retained_static_guard_classes': policy_summary['retained_static_guard_classes'],
                    'legacy_surface_states': policy_summary['legacy_surface_states'],
                },
            },
            {
                'issue_code': 'M313-B002',
                'artifact_kind': 'validation-harness-catalog',
                'report_path': 'tmp/reports/m313/M313-B002/validation_harness_catalog.json',
                'planning_source_paths': [
                    'tmp/planning/validation_consolidation/build_validation_harness_catalog.py',
                    'tmp/planning/validation_consolidation/validation_harness_catalog.json'
                ],
                'highlights': {
                    'shared_acceptance_suite_count': harness_catalog['shared_acceptance_harness']['suite_count'],
                    'workflow_action_count': harness_catalog['public_workflow_validation']['action_count'],
                    'workflow_family_count': len(harness_catalog['public_workflow_validation']['families']),
                },
            },
            {
                'issue_code': 'M313-B003',
                'artifact_kind': 'legacy-validation-surface-map',
                'report_path': 'tmp/reports/m313/M313-B003/legacy_validation_surface_map.json',
                'planning_source_paths': [
                    'tmp/planning/validation_consolidation/build_legacy_validation_surface_map.py',
                    'tmp/planning/validation_consolidation/legacy_validation_surface_map.json'
                ],
                'highlights': {
                    'legacy_surface_count': legacy_map['legacy_surface_count'],
                    'state_counts': legacy_map['state_counts'],
                },
            },
        ],
        'next_issues': ['M313-C002', 'M313-C003', 'M313-D001'],
    }

    validate_payload(payload)
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# M313-C001 Validation Acceptance Artifact Index',
        '',
        f"- contract_id: `{payload['contract_id']}`",
        f"- schema_path: `schemas/{SCHEMA_PATH.name}`",
        f"- artifact_count: `{payload['artifact_count']}`",
        '',
        '## Indexed artifacts',
    ]
    for artifact in payload['artifacts']:
        lines.append(f"- `{artifact['issue_code']}` -> `{artifact['artifact_kind']}`")
        lines.append(f"  - report_path: `{artifact['report_path']}`")
        lines.append(f"  - planning_source_paths: {', '.join(f'`{path}`' for path in artifact['planning_source_paths'])}")
    lines.extend(['', 'Next issues: `M313-C002`, `M313-C003`, `M313-D001`', ''])
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()