from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-E001'
REQUIRED_ARTIFACTS = {
    'A001': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-A001' / 'validation_surface_inventory.json',
    'B001': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B001' / 'policy_summary.json',
    'B002': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B002' / 'validation_harness_catalog.json',
    'B003': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B003' / 'legacy_validation_surface_map.json',
    'C001': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C001' / 'validation_acceptance_artifact_index.json',
    'C002': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C002' / 'validation_acceptance_suite_matrix.json',
    'C003': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C003' / 'validation_legacy_bridge_matrix.json',
    'D001': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-D001' / 'validation_ci_topology.json',
    'D002': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-D002' / 'validation_ci_topology_integration.json',
    'D003': ROOT / 'tmp' / 'reports' / 'm313' / 'M313-D003' / 'validation_budget_report.json',
}
OUTPUT_JSON_PATH = REPORT_DIR / 'validation_consolidation_closeout_gate.json'
OUTPUT_MD_PATH = REPORT_DIR / 'validation_consolidation_closeout_gate.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    missing = [name for name, path in REQUIRED_ARTIFACTS.items() if not path.is_file()]
    integration = load_json(REQUIRED_ARTIFACTS['D002']) if REQUIRED_ARTIFACTS['D002'].is_file() else {'status': 'FAIL'}
    budget = load_json(REQUIRED_ARTIFACTS['D003']) if REQUIRED_ARTIFACTS['D003'].is_file() else {'status': 'FAIL'}

    failures = []
    if missing:
        failures.append(f'missing artifacts: {missing}')
    if integration.get('status') != 'PASS':
        failures.append('D002 integration status is not PASS')
    if budget.get('status') != 'PASS':
        failures.append('D003 budget status is not PASS')

    payload = {
        'issue': 'M313-E001',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'status': 'PASS' if not failures else 'FAIL',
        'failure_count': len(failures),
        'failures': failures,
        'required_artifacts': {name: path.relative_to(ROOT).as_posix() for name, path in REQUIRED_ARTIFACTS.items()},
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# Validation Consolidation Closeout Gate',
        '',
        f"- issue: `{payload['issue']}`",
        f"- status: `{payload['status']}`",
        f"- failure_count: `{payload['failure_count']}`",
    ]
    if failures:
        lines.extend(['', '## Failures'])
        for failure in failures:
            lines.append(f"- {failure}")
    lines.extend(['', '## Required artifacts'])
    for name, path in payload['required_artifacts'].items():
        lines.append(f"- `{name}` -> `{path}`")
    lines.append('')
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()