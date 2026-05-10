from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from objc3c_tooling.json_io import write_text_file as write_text, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_payload
from objc3c_tooling.subprocesses import python_script_command

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-api-implementation'
CONTRACT_BUILDER = ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'
DEFAULT_CONTRACT_PATH = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'
PLAN_JSON_PATH = PLAN_DIR / 'workflow_api_implementation.json'
PLAN_MD_PATH = PLAN_DIR / 'workflow_api_implementation.md'
REPORT_JSON_PATH = REPORT_DIR / 'workflow_api_implementation_report.json'
REPORT_MD_PATH = REPORT_DIR / 'workflow_api_implementation_report.md'



def main() -> None:
    subprocess.run(python_script_command(CONTRACT_BUILDER), cwd=ROOT, check=True)
    contract = json.loads(DEFAULT_CONTRACT_PATH.read_text(encoding='utf-8'))
    describe_lint = public_workflow_action_payload('lint')

    payload = {
        'issue': 'workflow-api-implementation',
        'contract_builder': 'scripts/build_objc3c_public_command_contract.py',
        'default_contract_path': DEFAULT_CONTRACT_PATH.relative_to(ROOT).as_posix(),
        'runner_internal_actions': [
            'build-public-command-contract',
            'check-public-command-contract',
        ],
        'runner_mode': contract['runner_mode'],
        'package_bridge_count': contract['package_bridge_count'],
        'missing_package_bridge': contract['missing_package_bridge'],
        'unexpected_package_bridges': contract['unexpected_package_bridges'],
        'lint_script_action': describe_lint['action'],
        'next_issue': 'workflow-command-budget',
    }
    write_json_file(PLAN_JSON_PATH, payload)
    write_json_file(REPORT_JSON_PATH, payload)

    lines = [
        '# workflow-api-implementation Workflow API Implementation Report',
        '',
        f"- contract_builder: `{payload['contract_builder']}`",
        f"- default_contract_path: `{payload['default_contract_path']}`",
        f"- runner_mode: `{payload['runner_mode']}`",
        f"- package_bridge_count: `{payload['package_bridge_count']}`",
        f"- lint_script_action: `{payload['lint_script_action']}`",
        '',
        '## Runner internal actions',
    ]
    for action_name in payload['runner_internal_actions']:
        lines.append(f"- `{action_name}`")
    lines.extend(['', '## Drift checks'])
    lines.append(f"- missing_package_bridge: `{len(payload['missing_package_bridge'])}`")
    lines.append(f"- unexpected_package_bridges: `{len(payload['unexpected_package_bridges'])}`")
    lines.extend(['', 'Next issue: `workflow-command-budget`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
