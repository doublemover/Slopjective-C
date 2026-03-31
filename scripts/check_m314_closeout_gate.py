#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-closeout-gate'
JSON_OUT = REPORT_DIR / 'workflow_simplification_closeout_gate.json'
MD_OUT = REPORT_DIR / 'workflow_simplification_closeout_gate.md'
REPO = 'doublemover/Slopjective-C'
WORKFLOW_SIMPLIFICATION_TITLE = 'Command-surface reduction, dead-path removal, and workflow simplification'
EXPECTED_CODES = [
    'workflow-command-surface-inventory',
    'workflow-simplification-policy',
    'workflow-alias-retirement',
    'workflow-runner-unification',
    'workflow-public-command-contract',
    'workflow-api-implementation',
    'workflow-command-budget',
    'workflow-integration',
    'workflow-prototype-retirement',
    'workflow-closeout-gate',
]
CODE_RE = re.compile(r'^\[(M\d+)\]\[Lane-([A-E])\]\[([A-Z]\d{3})\] ')
EVIDENCE_FILES = [
    'tmp/reports/workflow-simplification/command-surface-inventory/command_surface_inventory.json',
    'tmp/reports/workflow-simplification/policy-summary/policy_summary.json',
    'tmp/reports/workflow-simplification/alias-retirement/alias_retirement_report.json',
    'tmp/reports/workflow-simplification/runner-unification/runner_unification_report.json',
    'tmp/reports/workflow-simplification/public-command-contract/public_command_contract.json',
    'tmp/reports/workflow-simplification/workflow-api-implementation/workflow_api_implementation_report.json',
    'tmp/reports/workflow-simplification/command-budget/command_budget_report.json',
    'tmp/reports/workflow-simplification/workflow-integration/workflow_integration_report.json',
    'tmp/reports/workflow-simplification/prototype-retirement/prototype_retirement_report.json',
]


def run(command: list[str]) -> str:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding='utf-8',
        errors='replace',
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"command failed: {' '.join(command)}")
    return completed.stdout


def run_json(command: list[str]) -> Any:
    return json.loads(run(command))


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding='utf-8'))
    if not isinstance(payload, dict):
        raise TypeError(f'expected object in {path}')
    return payload


def flatten_pages(payload: list[Any]) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []
    for page in payload:
        if isinstance(page, list):
            for item in page:
                if isinstance(item, dict):
                    result.append(item)
        elif isinstance(page, dict):
            result.append(page)
    return result


def parse_issue_code(title: str) -> str | None:
    match = CODE_RE.match(title)
    if not match:
        return None
    return f"{match.group(1)}-{match.group(3)}"


def main() -> int:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    mismatches: list[str] = []

    rerun_commands = [
        [sys.executable, str(ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'), '--check'],
        [sys.executable, str(ROOT / 'scripts' / 'check_objc3c_public_command_budget.py'), '--summary-out', str(ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-budget' / 'command_budget_report.json'), '--markdown-out', str(ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-budget' / 'command_budget_report.md')],
        [sys.executable, str(ROOT / 'tmp' / 'planning' / 'workflow_simplification' / 'build_workflow_integration_report.py')],
        [sys.executable, str(ROOT / 'tmp' / 'planning' / 'workflow_simplification' / 'build_prototype_retirement_report.py')],
    ]
    for command in rerun_commands:
        try:
            run(command)
        except RuntimeError as exc:
            mismatches.append(str(exc))

    evidence_presence = {}
    for rel in EVIDENCE_FILES:
        evidence_presence[rel] = (ROOT / rel).exists()
        if not evidence_presence[rel]:
            mismatches.append(f'missing evidence file: {rel}')

    budget = read_json(ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-budget' / 'command_budget_report.json')
    if budget.get('status') != 'PASS':
        mismatches.append('C003 command budget report did not pass')

    integration = read_json(ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-integration' / 'workflow_integration_report.json')
    doc_assertions = integration.get('doc_assertions', {})
    if not all(bool(value) for value in doc_assertions.values()):
        mismatches.append('D001 workflow integration assertions did not all pass')

    prototype_cleanup = read_json(ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-prototype-retirement' / 'prototype_retirement_report.json')
    if prototype_cleanup.get('status') != 'PASS':
        mismatches.append('D002 prototype retirement report did not pass')

    contract = read_json(ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json')
    if contract.get('unmapped_scripts'):
        mismatches.append(f"canonical contract has unmapped scripts: {contract['unmapped_scripts']}")
    if contract.get('extra_runner_public_scripts'):
        mismatches.append(f"canonical contract advertises extra runner public scripts: {contract['extra_runner_public_scripts']}")

    milestone_pages = run_json([
        'gh',
        'api',
        f'repos/{REPO}/milestones?state=open&per_page=100',
        '--paginate',
        '--slurp',
    ])
    milestones = flatten_pages(milestone_pages)
    live_m314 = next((milestone for milestone in milestones if milestone.get('title') == WORKFLOW_SIMPLIFICATION_TITLE), None)
    if not live_m314:
        mismatches.append('live GitHub workflow-simplification milestone not found')
        milestone_number = None
        live_issues = []
    else:
        milestone_number = live_m314.get('number')
        live_issues = run_json([
            'gh',
            'api',
            f'repos/{REPO}/issues?milestone={milestone_number}&state=all&per_page=100',
            '--paginate',
            '--slurp',
        ])
        live_issues = flatten_pages(live_issues)

    live_codes: dict[str, dict[str, Any]] = {}
    open_codes: list[str] = []
    closed_codes: list[str] = []
    for issue in live_issues:
        title = issue.get('title', '')
        if not isinstance(title, str):
            continue
        code = parse_issue_code(title)
        if not code:
            continue
        live_codes[code] = issue
        state = issue.get('state')
        if state == 'open':
            open_codes.append(code)
        elif state == 'closed':
            closed_codes.append(code)

    missing_live_codes = [code for code in EXPECTED_CODES if code not in live_codes]
    historical_extra_codes = sorted(code for code in live_codes if code not in EXPECTED_CODES)
    if missing_live_codes:
        mismatches.append(f'live workflow-simplification issue set is missing current manifest codes: {missing_live_codes}')
    if sorted(open_codes) != ['workflow-closeout-gate']:
        mismatches.append(f'live workflow-simplification open issue set mismatch: {sorted(open_codes)}')
    expected_closed_codes = sorted(code for code in EXPECTED_CODES if code != 'workflow-closeout-gate')
    missing_closed_codes = [code for code in expected_closed_codes if code not in closed_codes]
    if missing_closed_codes:
        mismatches.append(f'live workflow-simplification closed issue set is missing current closed codes: {missing_closed_codes}')

    commit_lines = [line for line in run(['git', 'log', '--oneline', '--grep', 'workflow simplification', '--max-count', '20']).splitlines() if line]

    report = {
        'issue': 'workflow-closeout-gate',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'repo': REPO,
        'live_m314_milestone_number': milestone_number,
        'package_script_count': contract.get('package_script_count'),
        'public_script_count': contract.get('public_script_count'),
        'operator_script_count': contract.get('operator_script_count'),
        'maintainer_script_count': contract.get('maintainer_script_count'),
        'expected_issue_codes': EXPECTED_CODES,
        'historical_extra_issue_codes': historical_extra_codes,
        'open_issue_codes': sorted(open_codes),
        'closed_issue_codes': sorted(closed_codes),
        'evidence_presence': evidence_presence,
        'budget_status': budget.get('status'),
        'integration_doc_assertions': doc_assertions,
        'prototype_cleanup_status': prototype_cleanup.get('status'),
        'commit_lines': commit_lines,
        'mismatch_count': len(mismatches),
        'mismatches': mismatches,
        'passed': not mismatches,
    }

    JSON_OUT.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    lines = [
        '# workflow-closeout-gate Workflow Simplification Closeout Gate',
        '',
        f"- generated_at: `{report['generated_at']}`",
        f"- live_m314_milestone_number: `{milestone_number}`",
        f"- package_script_count: `{report['package_script_count']}`",
        f"- public_script_count: `{report['public_script_count']}`",
        f"- operator_script_count: `{report['operator_script_count']}`",
        f"- maintainer_script_count: `{report['maintainer_script_count']}`",
        f"- budget_status: `{report['budget_status']}`",
        f"- prototype_cleanup_status: `{report['prototype_cleanup_status']}`",
        f"- mismatch_count: `{len(mismatches)}`",
        f"- passed: `{report['passed']}`",
        '',
        '## Open issue codes',
    ]
    if open_codes:
        lines.extend(f'- `{code}`' for code in sorted(open_codes))
    else:
        lines.append('- none')
    lines.extend(['', '## Closed issue codes'])
    if closed_codes:
        lines.extend(f'- `{code}`' for code in sorted(closed_codes))
    else:
        lines.append('- none')
    lines.extend(['', '## Historical extra issue codes'])
    if historical_extra_codes:
        lines.extend(f'- `{code}`' for code in historical_extra_codes)
    else:
        lines.append('- none')
    lines.extend(['', '## Evidence presence'])
    for rel, exists in evidence_presence.items():
        lines.append(f'- `{rel}`: `{exists}`')
    lines.extend(['', '## D001 documentation assertions'])
    for key, value in doc_assertions.items():
        lines.append(f'- `{key}`: `{value}`')
    lines.extend(['', '## Commits'])
    if commit_lines:
        lines.extend(f'- `{line}`' for line in commit_lines)
    else:
        lines.append('- none')
    lines.extend(['', '## Mismatches'])
    if mismatches:
        lines.extend(f'- {item}' for item in mismatches)
    else:
        lines.append('- none')
    MD_OUT.write_text('\n'.join(lines) + '\n', encoding='utf-8')

    return 0 if not mismatches else 1


if __name__ == '__main__':
    raise SystemExit(main())
