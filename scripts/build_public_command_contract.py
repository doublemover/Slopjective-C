from __future__ import annotations

from pathlib import Path
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file
from objc3c_tooling.public_runner import (
    public_workflow_action_payloads,
    public_workflow_list_payload,
)
from scripts.objc3c_workflow.actions.command_facades_inventory import (
    package_bridge_inventory_fields,
    package_bridge_payloads_from_scripts,
)

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-public-command-contract'
PACKAGE_JSON_PATH = ROOT / 'package.json'
SCHEMA_PATH = ROOT / 'schemas' / 'objc3c-public-command-contract-v1.schema.json'
PLAN_JSON_PATH = PLAN_DIR / 'public_command_contract.json'
PLAN_MD_PATH = PLAN_DIR / 'public_command_contract.md'
REPORT_JSON_PATH = REPORT_DIR / 'public_command_contract.json'
REPORT_MD_PATH = REPORT_DIR / 'public_command_contract.md'



def main() -> None:
    package = load_json(PACKAGE_JSON_PATH)
    schema = load_json(SCHEMA_PATH)
    list_payload = public_workflow_list_payload()
    package_scripts = package['scripts']

    bridge_inventory = package_bridge_inventory_fields(package_scripts)
    action_payloads = sorted(public_workflow_action_payloads(), key=lambda payload: str(payload.get('action')))
    package_bridge_payloads = package_bridge_payloads_from_scripts(package_scripts)
    operator_action_count = sum(1 for payload in action_payloads if payload.get('audience') == 'operator')
    maintainer_action_count = sum(1 for payload in action_payloads if payload.get('audience') == 'maintainer')

    contract = {
        'contract_id': 'objc3c-public-command-contract-v1',
        'issue': 'workflow-public-command-contract',
        'runner_mode': list_payload['mode'],
        'runner_path': list_payload['runner_path'],
        'schema_path': schema['$id'],
        'package_bridge_count': bridge_inventory['package_bridge_count'],
        'workflow_action_count': list_payload['action_count'],
        'internal_action_count': list_payload['internal_action_count'],
        'operator_action_count': operator_action_count,
        'maintainer_action_count': maintainer_action_count,
        'missing_package_bridge': bridge_inventory['missing_package_bridge'],
        'unexpected_package_bridges': bridge_inventory['unexpected_package_bridges'],
        'actions': action_payloads,
        'package_bridges': package_bridge_payloads,
        'next_issue': 'workflow-api-implementation',
    }
    write_json_file(PLAN_JSON_PATH, contract)
    write_json_file(REPORT_JSON_PATH, contract)

    lines = [
        '# workflow-public-command-contract Public Command Contract',
        '',
        f"- contract_id: `{contract['contract_id']}`",
        f"- package_bridge_count: `{contract['package_bridge_count']}`",
        f"- workflow_action_count: `{contract['workflow_action_count']}`",
        f"- internal_action_count: `{contract['internal_action_count']}`",
        f"- operator_action_count: `{contract['operator_action_count']}`",
        f"- maintainer_action_count: `{contract['maintainer_action_count']}`",
        f"- schema: `{SCHEMA_PATH.relative_to(ROOT).as_posix()}`",
        '',
        '## Drift checks',
        f"- missing_package_bridge: `{len(contract['missing_package_bridge'])}`",
        f"- unexpected_package_bridges: `{len(contract['unexpected_package_bridges'])}`",
        '',
        '## Package bridges',
    ]
    for payload in package_bridge_payloads:
        lines.append(f"- `{payload['package_bridge']}` -> `{payload['backend']}`")
    lines.extend(['', '## Contract status'])
    status = 'PASS' if not contract['missing_package_bridge'] and not contract['unexpected_package_bridges'] else 'FAIL'
    lines.append(f'- status: `{status}`')
    lines.extend(['', 'Next issue: `workflow-api-implementation`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
