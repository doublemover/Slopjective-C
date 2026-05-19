from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text
from scripts.objc3c_workflow.public_command_api import WORKFLOW_MODULE, public_workflow_action_payload
from objc3c_tooling.subprocesses import python_script_command

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-ci-topology-integration'
TOPOLOGY_PATH = PLAN_DIR / 'validation_ci_topology.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'validation_ci_topology_integration.json'
OUTPUT_MD_PATH = REPORT_DIR / 'validation_ci_topology_integration.md'
PACKAGE_JSON_PATH = ROOT / 'package.json'
TOPOLOGY_BUILDER = ROOT / 'scripts' / 'build_validation_ci_topology.py'
PUBLIC_NPM_BRIDGE = 'npm run objc3c -- '
EXPECTED_PACKAGE_BRIDGE_SCRIPT = ' '.join(('python', '-m', WORKFLOW_MODULE))




def describe_action(action: str) -> dict[str, Any]:
    return public_workflow_action_payload(action)


def ensure_topology() -> None:
    if TOPOLOGY_PATH.is_file():
        return
    subprocess.run(python_script_command(TOPOLOGY_BUILDER), cwd=ROOT, check=True)


def main() -> None:
    ensure_topology()
    topology = load_json(TOPOLOGY_PATH)
    package_json = load_json(PACKAGE_JSON_PATH)
    scripts = package_json['scripts']
    rows = []
    failures: list[str] = []

    if scripts != {'objc3c': EXPECTED_PACKAGE_BRIDGE_SCRIPT}:
        failures.append('package.json must expose only the canonical objc3c npm bridge')

    for row in topology['topology']:
        action = row['action']
        public_command = row['public_command']
        description = describe_action(action)
        expected_public_command = f'{PUBLIC_NPM_BRIDGE}{action}'
        if public_command != expected_public_command:
            failures.append(f'topology command for {action} drifted: {public_command}')
        rows.append({
            'action': action,
            'public_command': public_command,
            'validation_tier': description.get('validation_tier'),
            'guarantee_owner': description.get('guarantee_owner'),
            'family_count': row['family_count'],
            'families': row['families'],
        })

    payload = {
        'issue': 'validation-ci-topology-integration',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'status': 'PASS' if not failures else 'FAIL',
        'failure_count': len(failures),
        'failures': failures,
        'rows': rows,
        'next_issues': ['validation-budget-report', 'validation-closeout-gate'],
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# Validation CI Topology Integration',
        '',
        f"- issue: `{payload['issue']}`",
        f"- status: `{payload['status']}`",
        f"- failure_count: `{payload['failure_count']}`",
        '',
        '## Aggregate entrypoints',
    ]
    for row in rows:
        lines.append(f"- `{row['public_command']}` -> `{row['action']}`")
        lines.append(f"  - validation_tier: `{row['validation_tier']}`")
        lines.append(f"  - family_count: `{row['family_count']}`")
        lines.append(f"  - guarantee_owner: `{row['guarantee_owner']}`")
    if failures:
        lines.extend(['', '## Failures'])
        for failure in failures:
            lines.append(f"- {failure}")
    lines.extend(['', 'Next issues: `validation-budget-report`, `validation-closeout-gate`', ''])
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()
