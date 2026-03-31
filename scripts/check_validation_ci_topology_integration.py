from __future__ import annotations

import json
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-ci-topology-integration'
TOPOLOGY_PATH = PLAN_DIR / 'validation_ci_topology.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'validation_ci_topology_integration.json'
OUTPUT_MD_PATH = REPORT_DIR / 'validation_ci_topology_integration.md'
WORKFLOW_RUNNER = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
PACKAGE_JSON_PATH = ROOT / 'package.json'
ACTION_MAP = {
    'test:fast': 'test-fast',
    'test:objc3c:full': 'test-full',
    'test:objc3c:nightly': 'test-nightly',
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def describe_action(action: str) -> dict[str, Any]:
    result = subprocess.run(
        ['python', str(WORKFLOW_RUNNER), '--describe', action],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=True,
    )
    return json.loads(result.stdout)


def main() -> None:
    topology = load_json(TOPOLOGY_PATH)
    package_json = load_json(PACKAGE_JSON_PATH)
    scripts = package_json['scripts']
    rows = []
    failures: list[str] = []

    for row in topology['topology']:
        package_script = row['package_script']
        action = ACTION_MAP[package_script]
        package_command = scripts.get(package_script)
        if package_command is None:
            failures.append(f'missing package script: {package_script}')
            continue
        description = describe_action(action)
        public_scripts = description.get('public_scripts', [])
        if package_script not in public_scripts:
            failures.append(f'workflow describe for {action} does not publish public script {package_script}')
        rows.append({
            'package_script': package_script,
            'action': action,
            'package_command': package_command,
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
        lines.append(f"- `{row['package_script']}` -> `{row['action']}`")
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