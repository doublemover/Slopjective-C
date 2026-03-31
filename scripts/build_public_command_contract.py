from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-public-command-contract'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
SCHEMA_PATH = ROOT / 'schemas' / 'objc3c-public-command-contract-v1.schema.json'
PLAN_JSON_PATH = PLAN_DIR / 'public_command_contract.json'
PLAN_MD_PATH = PLAN_DIR / 'public_command_contract.md'
REPORT_JSON_PATH = REPORT_DIR / 'public_command_contract.json'
REPORT_MD_PATH = REPORT_DIR / 'public_command_contract.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def load_runner() -> Any:
    spec = importlib.util.spec_from_file_location('objc3c_public_workflow_runner_m314_c001', RUNNER_PATH)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def main() -> None:
    package = load_json(PACKAGE_JSON_PATH)
    schema = load_json(SCHEMA_PATH)
    runner = load_runner()
    list_payload = runner.list_actions_payload()
    package_scripts = package['scripts']

    public_script_to_action = runner.public_script_to_action_map()
    distinct_runner_scripts = sorted(public_script_to_action)
    package_script_names = sorted(package_scripts)
    unmapped_scripts = sorted(set(package_script_names) - set(distinct_runner_scripts))
    extra_runner_public_scripts = sorted(set(distinct_runner_scripts) - set(package_script_names))

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
        'public_script_count': len(distinct_runner_scripts),
        'internal_action_count': list_payload['internal_action_count'],
        'operator_script_count': operator_script_count,
        'maintainer_script_count': maintainer_script_count,
        'unmapped_scripts': unmapped_scripts,
        'extra_runner_public_scripts': extra_runner_public_scripts,
        'actions': action_payloads,
        'package_scripts': package_script_payloads,
        'next_issue': 'workflow-api-implementation',
    }
    write_text(PLAN_JSON_PATH, json.dumps(contract, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(contract, indent=2) + '\n')

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
