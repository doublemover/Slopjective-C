from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file
from objc3c_tooling.public_runner import load_public_workflow_runner

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-public-command-contract'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_workflow' / 'runner.py'
SCHEMA_PATH = ROOT / 'schemas' / 'objc3c-public-command-contract-v1.schema.json'
PLAN_JSON_PATH = PLAN_DIR / 'public_command_contract.json'
PLAN_MD_PATH = PLAN_DIR / 'public_command_contract.md'
REPORT_JSON_PATH = REPORT_DIR / 'public_command_contract.json'
REPORT_MD_PATH = REPORT_DIR / 'public_command_contract.md'




def load_runner() -> Any:
    return load_public_workflow_runner(
        runner_path=RUNNER_PATH,
        module_name='objc3c_workflow_runner_m314_c001',
    )


def main() -> None:
    package = load_json(PACKAGE_JSON_PATH)
    schema = load_json(SCHEMA_PATH)
    runner = load_runner()
    list_payload = runner.list_actions_payload()
    package_scripts = package['scripts']

    package_script_names = sorted(package_scripts)
    bridge_scripts = {'objc3c'}
    unmapped_scripts = sorted(set(package_script_names) - bridge_scripts)
    extra_runner_public_scripts: list[str] = []

    action_payloads = [runner.describe_action_payload(action_name) for action_name in sorted(runner.ACTION_SPECS)]
    package_script_payloads = [runner.describe_package_script_payload(script_name) for script_name in package_script_names]
    operator_script_count = sum(1 for payload in package_script_payloads if payload['audience'] == 'operator')
    maintainer_script_count = sum(1 for payload in package_script_payloads if payload['audience'] == 'maintainer')

    contract = {
        'contract_id': 'objc3c-public-command-contract-v1',
        'issue': 'workflow-public-command-contract',
        'runner_mode': list_payload['mode'],
        'runner_path': list_payload['runner_path'],
        'schema_path': schema['$id'],
        'package_script_count': len(package_script_names),
        'workflow_action_count': list_payload['action_count'],
        'public_script_count': len(package_script_names),
        'internal_action_count': list_payload['internal_action_count'],
        'operator_script_count': operator_script_count,
        'maintainer_script_count': maintainer_script_count,
        'unmapped_scripts': unmapped_scripts,
        'extra_runner_public_scripts': extra_runner_public_scripts,
        'actions': action_payloads,
        'package_scripts': package_script_payloads,
        'next_issue': 'workflow-api-implementation',
    }
    write_json_file(PLAN_JSON_PATH, contract)
    write_json_file(REPORT_JSON_PATH, contract)

    lines = [
        '# workflow-public-command-contract Public Command Contract',
        '',
        f"- contract_id: `{contract['contract_id']}`",
        f"- package_script_count: `{contract['package_script_count']}`",
        f"- workflow_action_count: `{contract['workflow_action_count']}`",
        f"- public_script_count: `{contract['public_script_count']}`",
        f"- internal_action_count: `{contract['internal_action_count']}`",
        f"- operator_script_count: `{contract['operator_script_count']}`",
        f"- maintainer_script_count: `{contract['maintainer_script_count']}`",
        f"- schema: `{SCHEMA_PATH.relative_to(ROOT).as_posix()}`",
        '',
        '## Drift checks',
        f"- unmapped_scripts: `{len(unmapped_scripts)}`",
        f"- extra_runner_public_scripts: `{len(extra_runner_public_scripts)}`",
        '',
        '## Maintainer package scripts',
    ]
    for payload in package_script_payloads:
        if payload['audience'] == 'maintainer':
            lines.append(f"- `{payload['package_script']}` -> `{payload['action']}`")
    lines.extend(['', '## Contract status'])
    status = 'PASS' if not unmapped_scripts and not extra_runner_public_scripts else 'FAIL'
    lines.append(f'- status: `{status}`')
    lines.extend(['', 'Next issue: `workflow-api-implementation`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
